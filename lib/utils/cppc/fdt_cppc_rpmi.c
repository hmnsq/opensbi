/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Subrahmanya Lingappa <slingappa@ventanamicro.com>
 */

#అన్తర్భావ <libfdt.h>
#అన్తర్భావ <sbi/riscv_io.h>
#అన్తర్భావ <sbi/sbi_cppc.h>
#అన్తర్భావ <sbi/sbi_ecall_interface.h>
#అన్తర్భావ <sbi/sbi_scratch.h>
#అన్తర్భావ <sbi/sbi_timer.h>
#అన్తర్భావ <sbi_utils/fdt/fdt_driver.h>
#అన్తర్భావ <sbi_utils/fdt/fdt_helper.h>
#అన్తర్భావ <sbi_utils/mailbox/fdt_mailbox.h>
#అన్తర్భావ <sbi_utils/mailbox/rpmi_mailbox.h>

/**
 * Per hart RPMI CPPC fast channel size (bytes)
 * PASSIVE MODE:
 *	0x0: DESIRED_PERFORMANCE (4-byte)
 *	0x4: __RESERVED	(4-byte)
 * ACTIVE MODE: (not supported yet)
 *	0x0: MINIMUM PERFORMANCE (4-byte)
 *	0x4: MAXIMUM PERFORMANCE (4-byte)
 */
#నిర్వచించు RPMI_CPPC_HART_FASTCHAN_కొలత		0x8

నిర్మాణం rpmi_cppc {
	నిర్మాణం mbox_chan *chan;
	bool fc_supported;
	bool fc_db_supported;
	గణన rpmi_cppc_fast_channel_db_వెడల్పు fc_db_వెడల్పు;
	గణన rpmi_cppc_fast_channel_cppc_బహుళకం బహుళకం;
	ulong fc_perf_request_addr;
	ulong fc_perf_feedback_addr;
	ulong fc_db_addr;
	u64 fc_db_setmask;
	u64 fc_db_preservemask;
	u64 prev_సమయం_csr;
	u64 prev_delivered_counter;
};

స్థిర గుర్తులేని దీర్ఘ rpmi_cppc_స్థానభ్రంశం;

స్థిర నిర్మాణం rpmi_cppc *rpmi_cppc_get_సూచిక(u32 hartid)
{
	నిర్మాణం sbi_scratch *scratch;

	scratch = sbi_hartid_గమ్యం_scratch(hartid);
	ఒకవేళ (!scratch || !rpmi_cppc_స్థానభ్రంశం)
		తిరిగి ఖాళీ;

	తిరిగి sbi_scratch_స్థానభ్రంశం_ptr(scratch, rpmi_cppc_స్థానభ్రంశం);
}

స్థిర శూన్య rpmi_cppc_fc_db_trigger(నిర్మాణం rpmi_cppc *cppc)
{
	u8 db_val_u8 = 0;
	u16 db_val_u16 = 0;
	u32 db_val_u32 = 0;
	u64 db_val_u64 = 0;

	ఎంపిక (cppc->fc_db_వెడల్పు) {
	స్థితి RPMI_CPPC_FAST_CHANNEL_DB_వెడల్పు_8:
		db_val_u8 = readb((శూన్య *)cppc->fc_db_addr);
		db_val_u8 = (u8)cppc->fc_db_setmask |
				(db_val_u8 & (u8)cppc->fc_db_preservemask);

		writeb(db_val_u8, (శూన్య *)cppc->fc_db_addr);
		ఆపు;
	స్థితి RPMI_CPPC_FAST_CHANNEL_DB_వెడల్పు_16:
		db_val_u16 = readw((శూన్య *)cppc->fc_db_addr);
		db_val_u16 = (u16)cppc->fc_db_setmask |
				(db_val_u16 & (u16)cppc->fc_db_preservemask);

		writew(db_val_u16, (శూన్య *)cppc->fc_db_addr);
		ఆపు;
	స్థితి RPMI_CPPC_FAST_CHANNEL_DB_వెడల్పు_32:
		db_val_u32 = readl((శూన్య *)cppc->fc_db_addr);
		db_val_u32 = (u32)cppc->fc_db_setmask |
				(db_val_u32 & (u32)cppc->fc_db_preservemask);

		writel(db_val_u32, (శూన్య *)cppc->fc_db_addr);
		ఆపు;
	స్థితి RPMI_CPPC_FAST_CHANNEL_DB_వెడల్పు_64:
#ఒకవేళ __riscv_xlen != 32
		db_val_u64 = readq((శూన్య *)cppc->fc_db_addr);
		db_val_u64 = cppc->fc_db_setmask |
				(db_val_u64 & cppc->fc_db_preservemask);

		writeq(db_val_u64, (శూన్య *)cppc->fc_db_addr);
#లేకపోతే
		db_val_u64 = readl((శూన్య *)(cppc->fc_db_addr + 4));
		db_val_u64 <<= 32;
		db_val_u64 |= readl((శూన్య *)cppc->fc_db_addr);
		db_val_u64 = cppc->fc_db_setmask |
				(db_val_u64 & cppc->fc_db_preservemask);
		writel(db_val_u64, (శూన్య *)cppc->fc_db_addr);
		writel(db_val_u64 >> 32, (శూన్య *)(cppc->fc_db_addr + 4));
#అయితే_ముగింపు
		ఆపు;
	అప్రమేయ:
		ఆపు;
	}
}

స్థిర పూర్ణాంకం rpmi_cppc_read(గుర్తులేని దీర్ఘ reg, u64 *val)
{
	u64 curr_freq, curr_సమయం_csr, సమయం_csr_delta, freq;
	పూర్ణాంకం rc = SBI_SUCCESS;
	నిర్మాణం rpmi_cppc_read_reg_req req;
	నిర్మాణం rpmi_cppc_read_reg_resp resp;
	నిర్మాణం rpmi_cppc *cppc;

	req.hart_id = current_hartid();
	req.reg_id = reg;
	cppc = rpmi_cppc_get_సూచిక(req.hart_id);

	ఒకవేళ ((reg == SBI_CPPC_DELIVERED_CTR) && cppc->fc_supported) {
#ఒకవేళ __riscv_xlen != 32
		curr_freq = readq((శూన్య *)cppc->fc_perf_feedback_addr);
#లేకపోతే
		curr_freq = readl((శూన్య *)cppc->fc_perf_feedback_addr + 4);
		curr_freq <<= 32;
		curr_freq |= readl((శూన్య *)cppc->fc_perf_feedback_addr);
#అయితే_ముగింపు
		curr_సమయం_csr = sbi_timer_విలువ();
		సమయం_csr_delta = curr_సమయం_csr - cppc->prev_సమయం_csr;
		cppc->prev_సమయం_csr = curr_సమయం_csr;

		freq = sbi_timer_frequency();
		ఒకవేళ (freq)
			cppc->prev_delivered_counter +=
				curr_freq * (సమయం_csr_delta / freq) +
				(curr_freq * (సమయం_csr_delta % freq)) / freq;

		resp.దత్తాంశం_lo = cpu_గమ్యం_le32(cppc->prev_delivered_counter & UINT32_MAX);
		resp.దత్తాంశం_hi = cpu_గమ్యం_le32(cppc->prev_delivered_counter >> 32);
	} లేకపోతే ఒకవేళ (reg == SBI_CPPC_REFERENCE_CTR) {
		curr_సమయం_csr = sbi_timer_విలువ();
		resp.దత్తాంశం_lo = cpu_గమ్యం_le32(curr_సమయం_csr & UINT32_MAX);
		resp.దత్తాంశం_hi = cpu_గమ్యం_le32(curr_సమయం_csr >> 32);
	} లేకపోతే {
		rc = rpmi_సాధారణ_request_with_స్థితి(
				cppc->chan, RPMI_CPPC_SRV_READ_REG,
				&req, rpmi_u32_లెక్క(req), rpmi_u32_లెక్క(req),
				&resp, rpmi_u32_లెక్క(resp), rpmi_u32_లెక్క(resp));
		ఒకవేళ (rc)
			తిరిగి rc;
	}

#ఒకవేళ __riscv_xlen == 32
	*val = resp.దత్తాంశం_lo;
#లేకపోతే
	*val = (u64)resp.దత్తాంశం_hi << 32 | resp.దత్తాంశం_lo;
#అయితే_ముగింపు
	తిరిగి rc;
}

స్థిర పూర్ణాంకం rpmi_cppc_write(గుర్తులేని దీర్ఘ reg, u64 val)
{
	పూర్ణాంకం rc = SBI_SUCCESS;
	u32 hart_id = current_hartid();
	నిర్మాణం rpmi_cppc_write_reg_req req;
	నిర్మాణం rpmi_cppc_write_reg_resp resp;
	నిర్మాణం rpmi_cppc *cppc = rpmi_cppc_get_సూచిక(hart_id);

	ఒకవేళ (reg != SBI_CPPC_DESIRED_PERF || !cppc->fc_supported) {
		req.hart_id = hart_id;
		req.reg_id = reg;
		req.దత్తాంశం_lo = val & 0xFFFFFFFF;
		req.దత్తాంశం_hi = val >> 32;

		rc = rpmi_సాధారణ_request_with_స్థితి(
			cppc->chan, RPMI_CPPC_SRV_WRITE_REG,
			&req, rpmi_u32_లెక్క(req), rpmi_u32_లెక్క(req),
			&resp, rpmi_u32_లెక్క(resp), rpmi_u32_లెక్క(resp));
	} లేకపోతే {
		/* use fast path writes for desired_perf in passive mode */
		writel((u32)val, (శూన్య *)cppc->fc_perf_request_addr);

		ఒకవేళ (cppc->fc_db_supported)
			rpmi_cppc_fc_db_trigger(cppc);
	}

	తిరిగి rc;
}

స్థిర పూర్ణాంకం rpmi_cppc_probe(గుర్తులేని దీర్ఘ reg)
{
	పూర్ణాంకం rc;
	నిర్మాణం rpmi_cppc *cppc;
	నిర్మాణం rpmi_cppc_probe_resp resp;
	నిర్మాణం rpmi_cppc_probe_req req;

	req.hart_id = current_hartid();
	req.reg_id = reg;

	cppc = rpmi_cppc_get_సూచిక(req.hart_id);
	ఒకవేళ (!cppc)
		తిరిగి SBI_ENOSYS;

	rc = rpmi_సాధారణ_request_with_స్థితి(
			cppc->chan, RPMI_CPPC_SRV_PROBE_REG,
			&req, rpmi_u32_లెక్క(req), rpmi_u32_లెక్క(req),
			&resp, rpmi_u32_లెక్క(resp), rpmi_u32_లెక్క(resp));
	ఒకవేళ (rc)
		తిరిగి rc;

	తిరిగి resp.reg_len;
}

స్థిర నిర్మాణం sbi_cppc_device sbi_rpmi_cppc = {
	.name		= "rpmi-cppc",
	.cppc_read	= rpmi_cppc_read,
	.cppc_write	= rpmi_cppc_write,
	.cppc_probe	= rpmi_cppc_probe,
};

స్థిర పూర్ణాంకం rpmi_cppc_update_hart_scratch(నిర్మాణం mbox_chan *chan)
{
	పూర్ణాంకం rc, i;
	bool fc_supported = false;
	bool fc_db_supported = false;
	నిర్మాణం rpmi_cppc_hart_list_req req;
	నిర్మాణం rpmi_cppc_hart_list_resp resp;
	నిర్మాణం rpmi_cppc_get_fastchan_స్థానభ్రంశం_req hfreq;
	నిర్మాణం rpmi_cppc_get_fastchan_స్థానభ్రంశం_resp hfresp;
	నిర్మాణం rpmi_cppc_get_fastchan_region_resp fresp;
	గణన rpmi_cppc_fast_channel_db_వెడల్పు fc_db_వెడల్పు = 0;
	గణన rpmi_cppc_fast_channel_cppc_బహుళకం cppc_బహుళకం = 0;
	నిర్మాణం rpmi_cppc *cppc;
	గుర్తులేని దీర్ఘ fc_region_addr = 0;
	గుర్తులేని దీర్ఘ fc_region_కొలత = 0;
	గుర్తులేని దీర్ఘ fc_db_addr = 0;
	u64 fc_db_setmask = 0;
	u64 fc_db_preservemask = 0;

	rc = rpmi_సాధారణ_request_with_స్థితి(
		chan, RPMI_CPPC_SRV_GET_FAST_CHANNEL_REGION,
		ఖాళీ, 0, 0,
		&fresp, rpmi_u32_లెక్క(fresp), rpmi_u32_లెక్క(fresp));
	ఒకవేళ (rc && rc != SBI_ENOTSUPP)
		తిరిగి rc;

	/* At this point fast channel availability is confirmed */
	fc_supported = (rc != SBI_ENOTSUPP)? true : false;

	/* If fast channel is supported, add the fast channel
	 * region in root domain as MMIO RW. And, get the doorbell
	 * information from the response */
	ఒకవేళ (fc_supported) {
#ఒకవేళ __riscv_xlen == 32
		fc_region_addr = fresp.region_addr_lo;
		fc_region_కొలత = fresp.region_కొలత_lo;
		fc_db_addr = fresp.db_addr_lo;
#లేకపోతే
		fc_region_addr = (ulong)fresp.region_addr_hi << 32 |
							fresp.region_addr_lo;
		fc_region_కొలత = (ulong)fresp.region_కొలత_hi << 32 |
							fresp.region_కొలత_lo;
		fc_db_addr = (ulong)fresp.db_addr_hi << 32 | fresp.db_addr_lo;

#అయితే_ముగింపు
		rc = sbi_domain_మూలం_కూడు_memrange(fc_region_addr,
					fc_region_కొలత,
					RPMI_CPPC_HART_FASTCHAN_కొలత,
					(SBI_DOMAIN_MEMREGION_MMIO |
					SBI_DOMAIN_MEMREGION_M_READABLE |
					SBI_DOMAIN_MEMREGION_M_WRITABLE));
		ఒకవేళ (rc)
			తిరిగి rc;

		cppc_బహుళకం = (fresp.flags & RPMI_CPPC_FAST_CHANNEL_CPPC_బహుళకం_ముసుగు) >>
					RPMI_CPPC_FAST_CHANNEL_CPPC_బహుళకం_POS;
		fc_db_supported = fresp.flags &
				RPMI_CPPC_FAST_CHANNEL_FLAGS_DB_SUPPORTED;
		fc_db_వెడల్పు = (fresp.flags &
				RPMI_CPPC_FAST_CHANNEL_FLAGS_DB_వెడల్పు_ముసుగు) >>
				RPMI_CPPC_FAST_CHANNEL_FLAGS_DB_వెడల్పు_POS;
		fc_db_setmask = (u64)fresp.db_setmask_hi << 32 |
						fresp.db_setmask_lo;
		fc_db_preservemask = (u64)fresp.db_preservemask_hi << 32 |
						fresp.db_preservemask_lo;
	}

	/* Get the hart list and depending on the fast channel support
	 * initialize the per hart cppc structure */
	req.ప్రారంభం_సూచీ = 0;
	చేయి {
		rc = rpmi_సాధారణ_request_with_స్థితి(
			chan, RPMI_CPPC_SRV_GET_HART_LIST,
			&req, rpmi_u32_లెక్క(req), rpmi_u32_లెక్క(req),
			&resp, rpmi_u32_లెక్క(resp), rpmi_u32_లెక్క(resp));
		ఒకవేళ (rc)
			తిరిగి rc;

		/* For each returned harts, get their fastchan offset and
		 * complete the initialization of per hart cppc structure */
		కొరకు (i = 0; i < resp.returned; i++) {
			cppc = rpmi_cppc_get_సూచిక(resp.hartid[i]);
			ఒకవేళ (!cppc)
				తిరిగి SBI_ENOSYS;

			cppc->prev_సమయం_csr = 0;
			cppc->prev_delivered_counter = 0;
			cppc->chan = chan;
			cppc->బహుళకం = cppc_బహుళకం;
			cppc->fc_supported = fc_supported;

			ఒకవేళ (fc_supported) {
				hfreq.hart_id = resp.hartid[i];
				rc = rpmi_సాధారణ_request_with_స్థితి(
						chan,
						RPMI_CPPC_SRV_GET_FAST_CHANNEL_స్థానభ్రంశం,
						&hfreq,
						rpmi_u32_లెక్క(hfreq),
						rpmi_u32_లెక్క(hfreq),
						&hfresp,
						rpmi_u32_లెక్క(hfresp),
						rpmi_u32_లెక్క(hfresp));
				ఒకవేళ (rc)
					కొనసాగు;

#ఒకవేళ __riscv_xlen == 32
				cppc->fc_perf_request_addr = fc_region_addr +
							hfresp.fc_perf_request_స్థానభ్రంశం_lo;
				cppc->fc_perf_feedback_addr = fc_region_addr +
							hfresp.fc_perf_feedback_స్థానభ్రంశం_lo;
#లేకపోతే
				cppc->fc_perf_request_addr = fc_region_addr +
					((ulong)hfresp.fc_perf_request_స్థానభ్రంశం_hi << 32 |
					hfresp.fc_perf_request_స్థానభ్రంశం_lo);

				cppc->fc_perf_feedback_addr = fc_region_addr +
					((ulong)hfresp.fc_perf_feedback_స్థానభ్రంశం_hi << 32 |
					hfresp.fc_perf_feedback_స్థానభ్రంశం_lo);
#అయితే_ముగింపు
				cppc->fc_db_supported = fc_db_supported;
				cppc->fc_db_addr = fc_db_addr;
				cppc->fc_db_వెడల్పు = fc_db_వెడల్పు;

				cppc->fc_db_setmask = fc_db_setmask;
				cppc->fc_db_preservemask = fc_db_preservemask;
			}
			లేకపోతే {
				cppc->fc_perf_request_addr = 0;
				cppc->fc_perf_feedback_addr = 0;
				cppc->fc_db_supported = 0;
				cppc->fc_db_addr = 0;
				cppc->fc_db_వెడల్పు = 0;
				cppc->fc_db_setmask = 0;
				cppc->fc_db_preservemask = 0;
			}
		}
		req.ప్రారంభం_సూచీ += resp.returned;
	} అయితే (resp.మిగిలిన);

	తిరిగి 0;
}

స్థిర పూర్ణాంకం rpmi_cppc_cold_init(స్థిరరాశి శూన్య *fdt, పూర్ణాంకం nodeoff,
			       స్థిరరాశి నిర్మాణం fdt_సరిపోలిక *సరిపోలిక)
{
	పూర్ణాంకం rc;
	నిర్మాణం mbox_chan *chan;

	ఒకవేళ (!rpmi_cppc_స్థానభ్రంశం) {
		rpmi_cppc_స్థానభ్రంశం =
			sbi_scratch_alloc_type_స్థానభ్రంశం(నిర్మాణం rpmi_cppc);
		ఒకవేళ (!rpmi_cppc_స్థానభ్రంశం)
			తిరిగి SBI_ENOMEM;
	}

	/*
	 * If channel request failed then other end does not support
	 * CPPC service group so do nothing.
	 */
	rc = fdt_mailbox_request_chan(fdt, nodeoff, 0, &chan);
	ఒకవేళ (rc)
		తిరిగి SBI_ENODEV;

	/* Update per-HART scratch space */
	rc = rpmi_cppc_update_hart_scratch(chan);
	ఒకవేళ (rc)
		తిరిగి rc;

	sbi_cppc_సమితి_device(&sbi_rpmi_cppc);

	తిరిగి 0;
}

స్థిర స్థిరరాశి నిర్మాణం fdt_సరిపోలిక rpmi_cppc_సరిపోలిక[] = {
	{ .compatible = "riscv,rpmi-cppc" },
	{},
};

స్థిరరాశి నిర్మాణం fdt_driver fdt_cppc_rpmi = {
	.సరిపోలిక_పట్టిక = rpmi_cppc_సరిపోలిక,
	.init = rpmi_cppc_cold_init,
};
