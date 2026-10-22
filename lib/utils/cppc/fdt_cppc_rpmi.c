/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Subrahmanya Lingappa <slingappa@ventanamicro.com>
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <libfdt.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/riscv_io.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_cppc.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_ecall_interface.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_scratch.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_timer.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/fdt/fdt_driver.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/fdt/fdt_helper.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/mailbox/fdt_mailbox.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/mailbox/rpmi_mailbox.h>

/**
 * Per hart RPMI CPPC fast channel size (bytes)
 * PASSIVE MODE:
 *	0x0: DESIRED_PERFORMANCE (4-byte)
 *	0x4: __RESERVED	(4-byte)
 * ACTIVE MODE: (not supported yet)
 *	0x0: MINIMUM PERFORMANCE (4-byte)
 *	0x4: MAXIMUM PERFORMANCE (4-byte)
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RPMI_CPPC_HART_FASTCHAN_ਮਾਪ		0x8

ਬਣਤਰ rpmi_cppc {
	ਬਣਤਰ mbox_chan *chan;
	bool fc_supported;
	bool fc_db_supported;
	ਗਿਣਤੀ rpmi_cppc_fast_channel_db_ਚੌੜਾਈ fc_db_ਚੌੜਾਈ;
	ਗਿਣਤੀ rpmi_cppc_fast_channel_cppc_ਬਹੁਲਕ ਬਹੁਲਕ;
	ulong fc_perf_request_addr;
	ulong fc_perf_feedback_addr;
	ulong fc_db_addr;
	u64 fc_db_setmask;
	u64 fc_db_preservemask;
	u64 prev_ਸਮਾਂ_csr;
	u64 prev_delivered_counter;
};

ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ rpmi_cppc_ਵਿੱਥ;

ਸਥਿਰ ਬਣਤਰ rpmi_cppc *rpmi_cppc_get_ਸੂਚਕ(u32 hartid)
{
	ਬਣਤਰ sbi_scratch *scratch;

	scratch = sbi_hartid_ਮੰਜ਼ਿਲ_scratch(hartid);
	ਜੇ (!scratch || !rpmi_cppc_ਵਿੱਥ)
		ਵਾਪਸ ਨਿਰਾ;

	ਵਾਪਸ sbi_scratch_ਵਿੱਥ_ptr(scratch, rpmi_cppc_ਵਿੱਥ);
}

ਸਥਿਰ ਖਾਲੀ rpmi_cppc_fc_db_trigger(ਬਣਤਰ rpmi_cppc *cppc)
{
	u8 db_val_u8 = 0;
	u16 db_val_u16 = 0;
	u32 db_val_u32 = 0;
	u64 db_val_u64 = 0;

	ਚੋਣ (cppc->fc_db_ਚੌੜਾਈ) {
	ਹਾਲਤ RPMI_CPPC_FAST_CHANNEL_DB_ਚੌੜਾਈ_8:
		db_val_u8 = readb((ਖਾਲੀ *)cppc->fc_db_addr);
		db_val_u8 = (u8)cppc->fc_db_setmask |
				(db_val_u8 & (u8)cppc->fc_db_preservemask);

		writeb(db_val_u8, (ਖਾਲੀ *)cppc->fc_db_addr);
		ਰੋਕੋ;
	ਹਾਲਤ RPMI_CPPC_FAST_CHANNEL_DB_ਚੌੜਾਈ_16:
		db_val_u16 = readw((ਖਾਲੀ *)cppc->fc_db_addr);
		db_val_u16 = (u16)cppc->fc_db_setmask |
				(db_val_u16 & (u16)cppc->fc_db_preservemask);

		writew(db_val_u16, (ਖਾਲੀ *)cppc->fc_db_addr);
		ਰੋਕੋ;
	ਹਾਲਤ RPMI_CPPC_FAST_CHANNEL_DB_ਚੌੜਾਈ_32:
		db_val_u32 = readl((ਖਾਲੀ *)cppc->fc_db_addr);
		db_val_u32 = (u32)cppc->fc_db_setmask |
				(db_val_u32 & (u32)cppc->fc_db_preservemask);

		writel(db_val_u32, (ਖਾਲੀ *)cppc->fc_db_addr);
		ਰੋਕੋ;
	ਹਾਲਤ RPMI_CPPC_FAST_CHANNEL_DB_ਚੌੜਾਈ_64:
#ਜੇ __riscv_xlen != 32
		db_val_u64 = readq((ਖਾਲੀ *)cppc->fc_db_addr);
		db_val_u64 = cppc->fc_db_setmask |
				(db_val_u64 & cppc->fc_db_preservemask);

		writeq(db_val_u64, (ਖਾਲੀ *)cppc->fc_db_addr);
#ਨਹੀਂਤਾਂ
		db_val_u64 = readl((ਖਾਲੀ *)(cppc->fc_db_addr + 4));
		db_val_u64 <<= 32;
		db_val_u64 |= readl((ਖਾਲੀ *)cppc->fc_db_addr);
		db_val_u64 = cppc->fc_db_setmask |
				(db_val_u64 & cppc->fc_db_preservemask);
		writel(db_val_u64, (ਖਾਲੀ *)cppc->fc_db_addr);
		writel(db_val_u64 >> 32, (ਖਾਲੀ *)(cppc->fc_db_addr + 4));
#ਜੇ_ਅੰਤ
		ਰੋਕੋ;
	ਮੂਲ:
		ਰੋਕੋ;
	}
}

ਸਥਿਰ ਪੂਰਨ rpmi_cppc_read(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ reg, u64 *val)
{
	u64 curr_freq, curr_ਸਮਾਂ_csr, ਸਮਾਂ_csr_delta, freq;
	ਪੂਰਨ rc = SBI_SUCCESS;
	ਬਣਤਰ rpmi_cppc_read_reg_req req;
	ਬਣਤਰ rpmi_cppc_read_reg_resp resp;
	ਬਣਤਰ rpmi_cppc *cppc;

	req.hart_id = current_hartid();
	req.reg_id = reg;
	cppc = rpmi_cppc_get_ਸੂਚਕ(req.hart_id);

	ਜੇ ((reg == SBI_CPPC_DELIVERED_CTR) && cppc->fc_supported) {
#ਜੇ __riscv_xlen != 32
		curr_freq = readq((ਖਾਲੀ *)cppc->fc_perf_feedback_addr);
#ਨਹੀਂਤਾਂ
		curr_freq = readl((ਖਾਲੀ *)cppc->fc_perf_feedback_addr + 4);
		curr_freq <<= 32;
		curr_freq |= readl((ਖਾਲੀ *)cppc->fc_perf_feedback_addr);
#ਜੇ_ਅੰਤ
		curr_ਸਮਾਂ_csr = sbi_timer_ਮੁੱਲ();
		ਸਮਾਂ_csr_delta = curr_ਸਮਾਂ_csr - cppc->prev_ਸਮਾਂ_csr;
		cppc->prev_ਸਮਾਂ_csr = curr_ਸਮਾਂ_csr;

		freq = sbi_timer_frequency();
		ਜੇ (freq)
			cppc->prev_delivered_counter +=
				curr_freq * (ਸਮਾਂ_csr_delta / freq) +
				(curr_freq * (ਸਮਾਂ_csr_delta % freq)) / freq;

		resp.ਅੰਕੜੇ_lo = cpu_ਮੰਜ਼ਿਲ_le32(cppc->prev_delivered_counter & UINT32_MAX);
		resp.ਅੰਕੜੇ_hi = cpu_ਮੰਜ਼ਿਲ_le32(cppc->prev_delivered_counter >> 32);
	} ਨਹੀਂਤਾਂ ਜੇ (reg == SBI_CPPC_REFERENCE_CTR) {
		curr_ਸਮਾਂ_csr = sbi_timer_ਮੁੱਲ();
		resp.ਅੰਕੜੇ_lo = cpu_ਮੰਜ਼ਿਲ_le32(curr_ਸਮਾਂ_csr & UINT32_MAX);
		resp.ਅੰਕੜੇ_hi = cpu_ਮੰਜ਼ਿਲ_le32(curr_ਸਮਾਂ_csr >> 32);
	} ਨਹੀਂਤਾਂ {
		rc = rpmi_ਸਾਧਾਰਨ_request_with_ਸਥਿਤੀ(
				cppc->chan, RPMI_CPPC_SRV_READ_REG,
				&req, rpmi_u32_ਗਿਣਤੀ(req), rpmi_u32_ਗਿਣਤੀ(req),
				&resp, rpmi_u32_ਗਿਣਤੀ(resp), rpmi_u32_ਗਿਣਤੀ(resp));
		ਜੇ (rc)
			ਵਾਪਸ rc;
	}

#ਜੇ __riscv_xlen == 32
	*val = resp.ਅੰਕੜੇ_lo;
#ਨਹੀਂਤਾਂ
	*val = (u64)resp.ਅੰਕੜੇ_hi << 32 | resp.ਅੰਕੜੇ_lo;
#ਜੇ_ਅੰਤ
	ਵਾਪਸ rc;
}

ਸਥਿਰ ਪੂਰਨ rpmi_cppc_write(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ reg, u64 val)
{
	ਪੂਰਨ rc = SBI_SUCCESS;
	u32 hart_id = current_hartid();
	ਬਣਤਰ rpmi_cppc_write_reg_req req;
	ਬਣਤਰ rpmi_cppc_write_reg_resp resp;
	ਬਣਤਰ rpmi_cppc *cppc = rpmi_cppc_get_ਸੂਚਕ(hart_id);

	ਜੇ (reg != SBI_CPPC_DESIRED_PERF || !cppc->fc_supported) {
		req.hart_id = hart_id;
		req.reg_id = reg;
		req.ਅੰਕੜੇ_lo = val & 0xFFFFFFFF;
		req.ਅੰਕੜੇ_hi = val >> 32;

		rc = rpmi_ਸਾਧਾਰਨ_request_with_ਸਥਿਤੀ(
			cppc->chan, RPMI_CPPC_SRV_WRITE_REG,
			&req, rpmi_u32_ਗਿਣਤੀ(req), rpmi_u32_ਗਿਣਤੀ(req),
			&resp, rpmi_u32_ਗਿਣਤੀ(resp), rpmi_u32_ਗਿਣਤੀ(resp));
	} ਨਹੀਂਤਾਂ {
		/* use fast path writes for desired_perf in passive mode */
		writel((u32)val, (ਖਾਲੀ *)cppc->fc_perf_request_addr);

		ਜੇ (cppc->fc_db_supported)
			rpmi_cppc_fc_db_trigger(cppc);
	}

	ਵਾਪਸ rc;
}

ਸਥਿਰ ਪੂਰਨ rpmi_cppc_probe(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ reg)
{
	ਪੂਰਨ rc;
	ਬਣਤਰ rpmi_cppc *cppc;
	ਬਣਤਰ rpmi_cppc_probe_resp resp;
	ਬਣਤਰ rpmi_cppc_probe_req req;

	req.hart_id = current_hartid();
	req.reg_id = reg;

	cppc = rpmi_cppc_get_ਸੂਚਕ(req.hart_id);
	ਜੇ (!cppc)
		ਵਾਪਸ SBI_ENOSYS;

	rc = rpmi_ਸਾਧਾਰਨ_request_with_ਸਥਿਤੀ(
			cppc->chan, RPMI_CPPC_SRV_PROBE_REG,
			&req, rpmi_u32_ਗਿਣਤੀ(req), rpmi_u32_ਗਿਣਤੀ(req),
			&resp, rpmi_u32_ਗਿਣਤੀ(resp), rpmi_u32_ਗਿਣਤੀ(resp));
	ਜੇ (rc)
		ਵਾਪਸ rc;

	ਵਾਪਸ resp.reg_len;
}

ਸਥਿਰ ਬਣਤਰ sbi_cppc_device sbi_rpmi_cppc = {
	.name		= "rpmi-cppc",
	.cppc_read	= rpmi_cppc_read,
	.cppc_write	= rpmi_cppc_write,
	.cppc_probe	= rpmi_cppc_probe,
};

ਸਥਿਰ ਪੂਰਨ rpmi_cppc_update_hart_scratch(ਬਣਤਰ mbox_chan *chan)
{
	ਪੂਰਨ rc, i;
	bool fc_supported = false;
	bool fc_db_supported = false;
	ਬਣਤਰ rpmi_cppc_hart_list_req req;
	ਬਣਤਰ rpmi_cppc_hart_list_resp resp;
	ਬਣਤਰ rpmi_cppc_get_fastchan_ਵਿੱਥ_req hfreq;
	ਬਣਤਰ rpmi_cppc_get_fastchan_ਵਿੱਥ_resp hfresp;
	ਬਣਤਰ rpmi_cppc_get_fastchan_region_resp fresp;
	ਗਿਣਤੀ rpmi_cppc_fast_channel_db_ਚੌੜਾਈ fc_db_ਚੌੜਾਈ = 0;
	ਗਿਣਤੀ rpmi_cppc_fast_channel_cppc_ਬਹੁਲਕ cppc_ਬਹੁਲਕ = 0;
	ਬਣਤਰ rpmi_cppc *cppc;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ fc_region_addr = 0;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ fc_region_ਮਾਪ = 0;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ fc_db_addr = 0;
	u64 fc_db_setmask = 0;
	u64 fc_db_preservemask = 0;

	rc = rpmi_ਸਾਧਾਰਨ_request_with_ਸਥਿਤੀ(
		chan, RPMI_CPPC_SRV_GET_FAST_CHANNEL_REGION,
		ਨਿਰਾ, 0, 0,
		&fresp, rpmi_u32_ਗਿਣਤੀ(fresp), rpmi_u32_ਗਿਣਤੀ(fresp));
	ਜੇ (rc && rc != SBI_ENOTSUPP)
		ਵਾਪਸ rc;

	/* At this point fast channel availability is confirmed */
	fc_supported = (rc != SBI_ENOTSUPP)? true : false;

	/* If fast channel is supported, add the fast channel
	 * region in root domain as MMIO RW. And, get the doorbell
	 * information from the response */
	ਜੇ (fc_supported) {
#ਜੇ __riscv_xlen == 32
		fc_region_addr = fresp.region_addr_lo;
		fc_region_ਮਾਪ = fresp.region_ਮਾਪ_lo;
		fc_db_addr = fresp.db_addr_lo;
#ਨਹੀਂਤਾਂ
		fc_region_addr = (ulong)fresp.region_addr_hi << 32 |
							fresp.region_addr_lo;
		fc_region_ਮਾਪ = (ulong)fresp.region_ਮਾਪ_hi << 32 |
							fresp.region_ਮਾਪ_lo;
		fc_db_addr = (ulong)fresp.db_addr_hi << 32 | fresp.db_addr_lo;

#ਜੇ_ਅੰਤ
		rc = sbi_domain_ਜੜ੍ਹ_ਜੋੜੋ_memrange(fc_region_addr,
					fc_region_ਮਾਪ,
					RPMI_CPPC_HART_FASTCHAN_ਮਾਪ,
					(SBI_DOMAIN_MEMREGION_MMIO |
					SBI_DOMAIN_MEMREGION_M_READABLE |
					SBI_DOMAIN_MEMREGION_M_WRITABLE));
		ਜੇ (rc)
			ਵਾਪਸ rc;

		cppc_ਬਹੁਲਕ = (fresp.flags & RPMI_CPPC_FAST_CHANNEL_CPPC_ਬਹੁਲਕ_ਨਕਾਬ) >>
					RPMI_CPPC_FAST_CHANNEL_CPPC_ਬਹੁਲਕ_POS;
		fc_db_supported = fresp.flags &
				RPMI_CPPC_FAST_CHANNEL_FLAGS_DB_SUPPORTED;
		fc_db_ਚੌੜਾਈ = (fresp.flags &
				RPMI_CPPC_FAST_CHANNEL_FLAGS_DB_ਚੌੜਾਈ_ਨਕਾਬ) >>
				RPMI_CPPC_FAST_CHANNEL_FLAGS_DB_ਚੌੜਾਈ_POS;
		fc_db_setmask = (u64)fresp.db_setmask_hi << 32 |
						fresp.db_setmask_lo;
		fc_db_preservemask = (u64)fresp.db_preservemask_hi << 32 |
						fresp.db_preservemask_lo;
	}

	/* Get the hart list and depending on the fast channel support
	 * initialize the per hart cppc structure */
	req.ਸ਼ੁਰੂ_ਸੂਚਕਾਂਕ = 0;
	ਕਰੋ {
		rc = rpmi_ਸਾਧਾਰਨ_request_with_ਸਥਿਤੀ(
			chan, RPMI_CPPC_SRV_GET_HART_LIST,
			&req, rpmi_u32_ਗਿਣਤੀ(req), rpmi_u32_ਗਿਣਤੀ(req),
			&resp, rpmi_u32_ਗਿਣਤੀ(resp), rpmi_u32_ਗਿਣਤੀ(resp));
		ਜੇ (rc)
			ਵਾਪਸ rc;

		/* For each returned harts, get their fastchan offset and
		 * complete the initialization of per hart cppc structure */
		ਲਈ (i = 0; i < resp.returned; i++) {
			cppc = rpmi_cppc_get_ਸੂਚਕ(resp.hartid[i]);
			ਜੇ (!cppc)
				ਵਾਪਸ SBI_ENOSYS;

			cppc->prev_ਸਮਾਂ_csr = 0;
			cppc->prev_delivered_counter = 0;
			cppc->chan = chan;
			cppc->ਬਹੁਲਕ = cppc_ਬਹੁਲਕ;
			cppc->fc_supported = fc_supported;

			ਜੇ (fc_supported) {
				hfreq.hart_id = resp.hartid[i];
				rc = rpmi_ਸਾਧਾਰਨ_request_with_ਸਥਿਤੀ(
						chan,
						RPMI_CPPC_SRV_GET_FAST_CHANNEL_ਵਿੱਥ,
						&hfreq,
						rpmi_u32_ਗਿਣਤੀ(hfreq),
						rpmi_u32_ਗਿਣਤੀ(hfreq),
						&hfresp,
						rpmi_u32_ਗਿਣਤੀ(hfresp),
						rpmi_u32_ਗਿਣਤੀ(hfresp));
				ਜੇ (rc)
					ਜਾਰੀ;

#ਜੇ __riscv_xlen == 32
				cppc->fc_perf_request_addr = fc_region_addr +
							hfresp.fc_perf_request_ਵਿੱਥ_lo;
				cppc->fc_perf_feedback_addr = fc_region_addr +
							hfresp.fc_perf_feedback_ਵਿੱਥ_lo;
#ਨਹੀਂਤਾਂ
				cppc->fc_perf_request_addr = fc_region_addr +
					((ulong)hfresp.fc_perf_request_ਵਿੱਥ_hi << 32 |
					hfresp.fc_perf_request_ਵਿੱਥ_lo);

				cppc->fc_perf_feedback_addr = fc_region_addr +
					((ulong)hfresp.fc_perf_feedback_ਵਿੱਥ_hi << 32 |
					hfresp.fc_perf_feedback_ਵਿੱਥ_lo);
#ਜੇ_ਅੰਤ
				cppc->fc_db_supported = fc_db_supported;
				cppc->fc_db_addr = fc_db_addr;
				cppc->fc_db_ਚੌੜਾਈ = fc_db_ਚੌੜਾਈ;

				cppc->fc_db_setmask = fc_db_setmask;
				cppc->fc_db_preservemask = fc_db_preservemask;
			}
			ਨਹੀਂਤਾਂ {
				cppc->fc_perf_request_addr = 0;
				cppc->fc_perf_feedback_addr = 0;
				cppc->fc_db_supported = 0;
				cppc->fc_db_addr = 0;
				cppc->fc_db_ਚੌੜਾਈ = 0;
				cppc->fc_db_setmask = 0;
				cppc->fc_db_preservemask = 0;
			}
		}
		req.ਸ਼ੁਰੂ_ਸੂਚਕਾਂਕ += resp.returned;
	} ਜਦੋਂ (resp.ਬਚਿਆ);

	ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ rpmi_cppc_cold_init(ਅਚਲ ਖਾਲੀ *fdt, ਪੂਰਨ nodeoff,
			       ਅਚਲ ਬਣਤਰ fdt_ਮੇਲ *ਮੇਲ)
{
	ਪੂਰਨ rc;
	ਬਣਤਰ mbox_chan *chan;

	ਜੇ (!rpmi_cppc_ਵਿੱਥ) {
		rpmi_cppc_ਵਿੱਥ =
			sbi_scratch_alloc_type_ਵਿੱਥ(ਬਣਤਰ rpmi_cppc);
		ਜੇ (!rpmi_cppc_ਵਿੱਥ)
			ਵਾਪਸ SBI_ENOMEM;
	}

	/*
	 * If channel request failed then other end does not support
	 * CPPC service group so do nothing.
	 */
	rc = fdt_mailbox_request_chan(fdt, nodeoff, 0, &chan);
	ਜੇ (rc)
		ਵਾਪਸ SBI_ENODEV;

	/* Update per-HART scratch space */
	rc = rpmi_cppc_update_hart_scratch(chan);
	ਜੇ (rc)
		ਵਾਪਸ rc;

	sbi_cppc_ਸੈੱਟ_device(&sbi_rpmi_cppc);

	ਵਾਪਸ 0;
}

ਸਥਿਰ ਅਚਲ ਬਣਤਰ fdt_ਮੇਲ rpmi_cppc_ਮੇਲ[] = {
	{ .compatible = "riscv,rpmi-cppc" },
	{},
};

ਅਚਲ ਬਣਤਰ fdt_driver fdt_cppc_rpmi = {
	.ਮੇਲ_ਸਾਰਣੀ = rpmi_cppc_ਮੇਲ,
	.init = rpmi_cppc_cold_init,
};
