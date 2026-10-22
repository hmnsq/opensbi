/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Subrahmanya Lingappa <slingappa@ventanamicro.com>
 */

#অন্তর্ভাব <libfdt.h>
#অন্তর্ভাব <sbi/riscv_io.h>
#অন্তর্ভাব <sbi/sbi_cppc.h>
#অন্তর্ভাব <sbi/sbi_ecall_interface.h>
#অন্তর্ভাব <sbi/sbi_scratch.h>
#অন্তর্ভাব <sbi/sbi_timer.h>
#অন্তর্ভাব <sbi_utils/fdt/fdt_driver.h>
#অন্তর্ভাব <sbi_utils/fdt/fdt_helper.h>
#অন্তর্ভাব <sbi_utils/mailbox/fdt_mailbox.h>
#অন্তর্ভাব <sbi_utils/mailbox/rpmi_mailbox.h>

/**
 * Per hart RPMI CPPC fast channel size (bytes)
 * PASSIVE MODE:
 *	0x0: DESIRED_PERFORMANCE (4-byte)
 *	0x4: __RESERVED	(4-byte)
 * ACTIVE MODE: (not supported yet)
 *	0x0: MINIMUM PERFORMANCE (4-byte)
 *	0x4: MAXIMUM PERFORMANCE (4-byte)
 */
#define RPMI_CPPC_HART_FASTCHAN_আকার		0x8

কাঠামো rpmi_cppc {
	কাঠামো mbox_chan *chan;
	bool fc_supported;
	bool fc_db_supported;
	গণনা rpmi_cppc_fast_channel_db_width fc_db_width;
	গণনা rpmi_cppc_fast_channel_cppc_mode mode;
	ulong fc_perf_request_addr;
	ulong fc_perf_feedback_addr;
	ulong fc_db_addr;
	u64 fc_db_setmask;
	u64 fc_db_preservemask;
	u64 পূর্ব_time_csr;
	u64 পূর্ব_delivered_গণক;
};

স্থির unsigned long rpmi_cppc_offset;

স্থির কাঠামো rpmi_cppc *rpmi_cppc_get_pointer(u32 hartid)
{
	কাঠামো sbi_scratch *scratch;

	scratch = sbi_hartid_to_scratch(hartid);
	যদি (!scratch || !rpmi_cppc_offset)
		ফেরত ফাঁকা;

	ফেরত sbi_scratch_offset_ptr(scratch, rpmi_cppc_offset);
}

স্থির শূন্য rpmi_cppc_fc_db_trigger(কাঠামো rpmi_cppc *cppc)
{
	u8 db_val_u8 = 0;
	u16 db_val_u16 = 0;
	u32 db_val_u32 = 0;
	u64 db_val_u64 = 0;

	নির্বাচন (cppc->fc_db_width) {
	ক্ষেত্র RPMI_CPPC_FAST_CHANNEL_DB_WIDTH_8:
		db_val_u8 = readb((শূন্য *)cppc->fc_db_addr);
		db_val_u8 = (u8)cppc->fc_db_setmask |
				(db_val_u8 & (u8)cppc->fc_db_preservemask);

		writeb(db_val_u8, (শূন্য *)cppc->fc_db_addr);
		বিরতি;
	ক্ষেত্র RPMI_CPPC_FAST_CHANNEL_DB_WIDTH_16:
		db_val_u16 = readw((শূন্য *)cppc->fc_db_addr);
		db_val_u16 = (u16)cppc->fc_db_setmask |
				(db_val_u16 & (u16)cppc->fc_db_preservemask);

		writew(db_val_u16, (শূন্য *)cppc->fc_db_addr);
		বিরতি;
	ক্ষেত্র RPMI_CPPC_FAST_CHANNEL_DB_WIDTH_32:
		db_val_u32 = readl((শূন্য *)cppc->fc_db_addr);
		db_val_u32 = (u32)cppc->fc_db_setmask |
				(db_val_u32 & (u32)cppc->fc_db_preservemask);

		writel(db_val_u32, (শূন্য *)cppc->fc_db_addr);
		বিরতি;
	ক্ষেত্র RPMI_CPPC_FAST_CHANNEL_DB_WIDTH_64:
#যদি __riscv_xlen != 32
		db_val_u64 = readq((শূন্য *)cppc->fc_db_addr);
		db_val_u64 = cppc->fc_db_setmask |
				(db_val_u64 & cppc->fc_db_preservemask);

		writeq(db_val_u64, (শূন্য *)cppc->fc_db_addr);
#অন্যথা
		db_val_u64 = readl((শূন্য *)(cppc->fc_db_addr + 4));
		db_val_u64 <<= 32;
		db_val_u64 |= readl((শূন্য *)cppc->fc_db_addr);
		db_val_u64 = cppc->fc_db_setmask |
				(db_val_u64 & cppc->fc_db_preservemask);
		writel(db_val_u64, (শূন্য *)cppc->fc_db_addr);
		writel(db_val_u64 >> 32, (শূন্য *)(cppc->fc_db_addr + 4));
#endif
		বিরতি;
	পূর্বনির্ধারিত:
		বিরতি;
	}
}

স্থির পূর্ণসংখ্যা rpmi_cppc_read(unsigned long reg, u64 *val)
{
	u64 curr_freq, curr_time_csr, time_csr_delta, freq;
	পূর্ণসংখ্যা rc = SBI_SUCCESS;
	কাঠামো rpmi_cppc_read_reg_req req;
	কাঠামো rpmi_cppc_read_reg_resp resp;
	কাঠামো rpmi_cppc *cppc;

	req.hart_id = current_hartid();
	req.reg_id = reg;
	cppc = rpmi_cppc_get_pointer(req.hart_id);

	যদি ((reg == SBI_CPPC_DELIVERED_CTR) && cppc->fc_supported) {
#যদি __riscv_xlen != 32
		curr_freq = readq((শূন্য *)cppc->fc_perf_feedback_addr);
#অন্যথা
		curr_freq = readl((শূন্য *)cppc->fc_perf_feedback_addr + 4);
		curr_freq <<= 32;
		curr_freq |= readl((শূন্য *)cppc->fc_perf_feedback_addr);
#endif
		curr_time_csr = sbi_timer_মান();
		time_csr_delta = curr_time_csr - cppc->পূর্ব_time_csr;
		cppc->পূর্ব_time_csr = curr_time_csr;

		freq = sbi_timer_frequency();
		যদি (freq)
			cppc->পূর্ব_delivered_গণক +=
				curr_freq * (time_csr_delta / freq) +
				(curr_freq * (time_csr_delta % freq)) / freq;

		resp.তথ্য_lo = cpu_to_le32(cppc->পূর্ব_delivered_গণক & UINT32_MAX);
		resp.তথ্য_hi = cpu_to_le32(cppc->পূর্ব_delivered_গণক >> 32);
	} অন্যথা যদি (reg == SBI_CPPC_REFERENCE_CTR) {
		curr_time_csr = sbi_timer_মান();
		resp.তথ্য_lo = cpu_to_le32(curr_time_csr & UINT32_MAX);
		resp.তথ্য_hi = cpu_to_le32(curr_time_csr >> 32);
	} অন্যথা {
		rc = rpmi_normal_request_with_status(
				cppc->chan, RPMI_CPPC_SRV_READ_REG,
				&req, rpmi_u32_গুনতি(req), rpmi_u32_গুনতি(req),
				&resp, rpmi_u32_গুনতি(resp), rpmi_u32_গুনতি(resp));
		যদি (rc)
			ফেরত rc;
	}

#যদি __riscv_xlen == 32
	*val = resp.তথ্য_lo;
#অন্যথা
	*val = (u64)resp.তথ্য_hi << 32 | resp.তথ্য_lo;
#endif
	ফেরত rc;
}

স্থির পূর্ণসংখ্যা rpmi_cppc_write(unsigned long reg, u64 val)
{
	পূর্ণসংখ্যা rc = SBI_SUCCESS;
	u32 hart_id = current_hartid();
	কাঠামো rpmi_cppc_write_reg_req req;
	কাঠামো rpmi_cppc_write_reg_resp resp;
	কাঠামো rpmi_cppc *cppc = rpmi_cppc_get_pointer(hart_id);

	যদি (reg != SBI_CPPC_DESIRED_PERF || !cppc->fc_supported) {
		req.hart_id = hart_id;
		req.reg_id = reg;
		req.তথ্য_lo = val & 0xFFFFFFFF;
		req.তথ্য_hi = val >> 32;

		rc = rpmi_normal_request_with_status(
			cppc->chan, RPMI_CPPC_SRV_WRITE_REG,
			&req, rpmi_u32_গুনতি(req), rpmi_u32_গুনতি(req),
			&resp, rpmi_u32_গুনতি(resp), rpmi_u32_গুনতি(resp));
	} অন্যথা {
		/* use fast path writes for desired_perf in passive mode */
		writel((u32)val, (শূন্য *)cppc->fc_perf_request_addr);

		যদি (cppc->fc_db_supported)
			rpmi_cppc_fc_db_trigger(cppc);
	}

	ফেরত rc;
}

স্থির পূর্ণসংখ্যা rpmi_cppc_probe(unsigned long reg)
{
	পূর্ণসংখ্যা rc;
	কাঠামো rpmi_cppc *cppc;
	কাঠামো rpmi_cppc_probe_resp resp;
	কাঠামো rpmi_cppc_probe_req req;

	req.hart_id = current_hartid();
	req.reg_id = reg;

	cppc = rpmi_cppc_get_pointer(req.hart_id);
	যদি (!cppc)
		ফেরত SBI_ENOSYS;

	rc = rpmi_normal_request_with_status(
			cppc->chan, RPMI_CPPC_SRV_PROBE_REG,
			&req, rpmi_u32_গুনতি(req), rpmi_u32_গুনতি(req),
			&resp, rpmi_u32_গুনতি(resp), rpmi_u32_গুনতি(resp));
	যদি (rc)
		ফেরত rc;

	ফেরত resp.reg_len;
}

স্থির কাঠামো sbi_cppc_device sbi_rpmi_cppc = {
	.name		= "rpmi-cppc",
	.cppc_read	= rpmi_cppc_read,
	.cppc_write	= rpmi_cppc_write,
	.cppc_probe	= rpmi_cppc_probe,
};

স্থির পূর্ণসংখ্যা rpmi_cppc_update_hart_scratch(কাঠামো mbox_chan *chan)
{
	পূর্ণসংখ্যা rc, i;
	bool fc_supported = false;
	bool fc_db_supported = false;
	কাঠামো rpmi_cppc_hart_list_req req;
	কাঠামো rpmi_cppc_hart_list_resp resp;
	কাঠামো rpmi_cppc_get_fastchan_offset_req hfreq;
	কাঠামো rpmi_cppc_get_fastchan_offset_resp hfresp;
	কাঠামো rpmi_cppc_get_fastchan_region_resp fresp;
	গণনা rpmi_cppc_fast_channel_db_width fc_db_width = 0;
	গণনা rpmi_cppc_fast_channel_cppc_mode cppc_mode = 0;
	কাঠামো rpmi_cppc *cppc;
	unsigned long fc_region_addr = 0;
	unsigned long fc_region_আকার = 0;
	unsigned long fc_db_addr = 0;
	u64 fc_db_setmask = 0;
	u64 fc_db_preservemask = 0;

	rc = rpmi_normal_request_with_status(
		chan, RPMI_CPPC_SRV_GET_FAST_CHANNEL_REGION,
		ফাঁকা, 0, 0,
		&fresp, rpmi_u32_গুনতি(fresp), rpmi_u32_গুনতি(fresp));
	যদি (rc && rc != SBI_ENOTSUPP)
		ফেরত rc;

	/* At this point fast channel availability is confirmed */
	fc_supported = (rc != SBI_ENOTSUPP)? true : false;

	/* If fast channel is supported, add the fast channel
	 * region in root domain as MMIO RW. And, get the doorbell
	 * information from the response */
	যদি (fc_supported) {
#যদি __riscv_xlen == 32
		fc_region_addr = fresp.region_addr_lo;
		fc_region_আকার = fresp.region_আকার_lo;
		fc_db_addr = fresp.db_addr_lo;
#অন্যথা
		fc_region_addr = (ulong)fresp.region_addr_hi << 32 |
							fresp.region_addr_lo;
		fc_region_আকার = (ulong)fresp.region_আকার_hi << 32 |
							fresp.region_আকার_lo;
		fc_db_addr = (ulong)fresp.db_addr_hi << 32 | fresp.db_addr_lo;

#endif
		rc = sbi_domain_মূল_add_memrange(fc_region_addr,
					fc_region_আকার,
					RPMI_CPPC_HART_FASTCHAN_আকার,
					(SBI_DOMAIN_MEMREGION_MMIO |
					SBI_DOMAIN_MEMREGION_M_READABLE |
					SBI_DOMAIN_MEMREGION_M_WRITABLE));
		যদি (rc)
			ফেরত rc;

		cppc_mode = (fresp.flags & RPMI_CPPC_FAST_CHANNEL_CPPC_MODE_MASK) >>
					RPMI_CPPC_FAST_CHANNEL_CPPC_MODE_POS;
		fc_db_supported = fresp.flags &
				RPMI_CPPC_FAST_CHANNEL_FLAGS_DB_SUPPORTED;
		fc_db_width = (fresp.flags &
				RPMI_CPPC_FAST_CHANNEL_FLAGS_DB_WIDTH_MASK) >>
				RPMI_CPPC_FAST_CHANNEL_FLAGS_DB_WIDTH_POS;
		fc_db_setmask = (u64)fresp.db_setmask_hi << 32 |
						fresp.db_setmask_lo;
		fc_db_preservemask = (u64)fresp.db_preservemask_hi << 32 |
						fresp.db_preservemask_lo;
	}

	/* Get the hart list and depending on the fast channel support
	 * initialize the per hart cppc structure */
	req.start_সূচক = 0;
	করো {
		rc = rpmi_normal_request_with_status(
			chan, RPMI_CPPC_SRV_GET_HART_LIST,
			&req, rpmi_u32_গুনতি(req), rpmi_u32_গুনতি(req),
			&resp, rpmi_u32_গুনতি(resp), rpmi_u32_গুনতি(resp));
		যদি (rc)
			ফেরত rc;

		/* For each returned harts, get their fastchan offset and
		 * complete the initialization of per hart cppc structure */
		জন্য (i = 0; i < resp.returned; i++) {
			cppc = rpmi_cppc_get_pointer(resp.hartid[i]);
			যদি (!cppc)
				ফেরত SBI_ENOSYS;

			cppc->পূর্ব_time_csr = 0;
			cppc->পূর্ব_delivered_গণক = 0;
			cppc->chan = chan;
			cppc->mode = cppc_mode;
			cppc->fc_supported = fc_supported;

			যদি (fc_supported) {
				hfreq.hart_id = resp.hartid[i];
				rc = rpmi_normal_request_with_status(
						chan,
						RPMI_CPPC_SRV_GET_FAST_CHANNEL_OFFSET,
						&hfreq,
						rpmi_u32_গুনতি(hfreq),
						rpmi_u32_গুনতি(hfreq),
						&hfresp,
						rpmi_u32_গুনতি(hfresp),
						rpmi_u32_গুনতি(hfresp));
				যদি (rc)
					চালু;

#যদি __riscv_xlen == 32
				cppc->fc_perf_request_addr = fc_region_addr +
							hfresp.fc_perf_request_offset_lo;
				cppc->fc_perf_feedback_addr = fc_region_addr +
							hfresp.fc_perf_feedback_offset_lo;
#অন্যথা
				cppc->fc_perf_request_addr = fc_region_addr +
					((ulong)hfresp.fc_perf_request_offset_hi << 32 |
					hfresp.fc_perf_request_offset_lo);

				cppc->fc_perf_feedback_addr = fc_region_addr +
					((ulong)hfresp.fc_perf_feedback_offset_hi << 32 |
					hfresp.fc_perf_feedback_offset_lo);
#endif
				cppc->fc_db_supported = fc_db_supported;
				cppc->fc_db_addr = fc_db_addr;
				cppc->fc_db_width = fc_db_width;

				cppc->fc_db_setmask = fc_db_setmask;
				cppc->fc_db_preservemask = fc_db_preservemask;
			}
			অন্যথা {
				cppc->fc_perf_request_addr = 0;
				cppc->fc_perf_feedback_addr = 0;
				cppc->fc_db_supported = 0;
				cppc->fc_db_addr = 0;
				cppc->fc_db_width = 0;
				cppc->fc_db_setmask = 0;
				cppc->fc_db_preservemask = 0;
			}
		}
		req.start_সূচক += resp.returned;
	} যখন (resp.remaining);

	ফেরত 0;
}

স্থির পূর্ণসংখ্যা rpmi_cppc_cold_init(ধ্রুবক শূন্য *fdt, পূর্ণসংখ্যা nodeoff,
			       ধ্রুবক কাঠামো fdt_match *match)
{
	পূর্ণসংখ্যা rc;
	কাঠামো mbox_chan *chan;

	যদি (!rpmi_cppc_offset) {
		rpmi_cppc_offset =
			sbi_scratch_alloc_type_offset(কাঠামো rpmi_cppc);
		যদি (!rpmi_cppc_offset)
			ফেরত SBI_ENOMEM;
	}

	/*
	 * If channel request failed then other end does not support
	 * CPPC service group so do nothing.
	 */
	rc = fdt_mailbox_request_chan(fdt, nodeoff, 0, &chan);
	যদি (rc)
		ফেরত SBI_ENODEV;

	/* Update per-HART scratch space */
	rc = rpmi_cppc_update_hart_scratch(chan);
	যদি (rc)
		ফেরত rc;

	sbi_cppc_set_device(&sbi_rpmi_cppc);

	ফেরত 0;
}

স্থির ধ্রুবক কাঠামো fdt_match rpmi_cppc_match[] = {
	{ .compatible = "riscv,rpmi-cppc" },
	{},
};

ধ্রুবক কাঠামো fdt_driver fdt_cppc_rpmi = {
	.match_table = rpmi_cppc_match,
	.init = rpmi_cppc_cold_init,
};
