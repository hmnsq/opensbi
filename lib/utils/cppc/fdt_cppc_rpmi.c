/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Subrahmanya Lingappa <slingappa@ventanamicro.com>
 */

#ضمن <libfdt.h>
#ضمن <sbi/riscv_io.h>
#ضمن <sbi/sbi_cppc.h>
#ضمن <sbi/sbi_ecall_interface.h>
#ضمن <sbi/sbi_scratch.h>
#ضمن <sbi/sbi_timer.h>
#ضمن <sbi_utils/fdt/fdt_driver.h>
#ضمن <sbi_utils/fdt/fdt_helper.h>
#ضمن <sbi_utils/mailbox/fdt_mailbox.h>
#ضمن <sbi_utils/mailbox/rpmi_mailbox.h>

/**
 * Per hart RPMI CPPC fast channel size (bytes)
 * PASSIVE MODE:
 *	0x0: DESIRED_PERFORMANCE (4-byte)
 *	0x4: __RESERVED	(4-byte)
 * ACTIVE MODE: (not supported yet)
 *	0x0: MINIMUM PERFORMANCE (4-byte)
 *	0x4: MAXIMUM PERFORMANCE (4-byte)
 */
#تعریف_کن RPMI_CPPC_HART_FASTCHAN_اندازه		0x8

بنية rpmi_cppc {
	بنية mbox_chan *chan;
	bool fc_supported;
	bool fc_db_supported;
	تعداد rpmi_cppc_fast_channel_db_عرض fc_db_عرض;
	تعداد rpmi_cppc_fast_channel_cppc_نما نما;
	ulong fc_perf_request_addr;
	ulong fc_perf_feedback_addr;
	ulong fc_db_addr;
	u64 fc_db_setmask;
	u64 fc_db_preservemask;
	u64 prev_زمان_csr;
	u64 prev_delivered_counter;
};

ساكن بیعلامت دراز rpmi_cppc_جابه_جایی;

ساكن بنية rpmi_cppc *rpmi_cppc_get_اشاره_گر(u32 hartid)
{
	بنية sbi_scratch *scratch;

	scratch = sbi_hartid_مقصد_scratch(hartid);
	إذا (!scratch || !rpmi_cppc_جابه_جایی)
		أرجع عدم;

	أرجع sbi_scratch_جابه_جایی_ptr(scratch, rpmi_cppc_جابه_جایی);
}

ساكن فراغ rpmi_cppc_fc_db_trigger(بنية rpmi_cppc *cppc)
{
	u8 db_val_u8 = 0;
	u16 db_val_u16 = 0;
	u32 db_val_u32 = 0;
	u64 db_val_u64 = 0;

	بدل (cppc->fc_db_عرض) {
	حالة RPMI_CPPC_FAST_CHANNEL_DB_عرض_8:
		db_val_u8 = readb((فراغ *)cppc->fc_db_addr);
		db_val_u8 = (u8)cppc->fc_db_setmask |
				(db_val_u8 & (u8)cppc->fc_db_preservemask);

		writeb(db_val_u8, (فراغ *)cppc->fc_db_addr);
		اكسر;
	حالة RPMI_CPPC_FAST_CHANNEL_DB_عرض_16:
		db_val_u16 = readw((فراغ *)cppc->fc_db_addr);
		db_val_u16 = (u16)cppc->fc_db_setmask |
				(db_val_u16 & (u16)cppc->fc_db_preservemask);

		writew(db_val_u16, (فراغ *)cppc->fc_db_addr);
		اكسر;
	حالة RPMI_CPPC_FAST_CHANNEL_DB_عرض_32:
		db_val_u32 = readl((فراغ *)cppc->fc_db_addr);
		db_val_u32 = (u32)cppc->fc_db_setmask |
				(db_val_u32 & (u32)cppc->fc_db_preservemask);

		writel(db_val_u32, (فراغ *)cppc->fc_db_addr);
		اكسر;
	حالة RPMI_CPPC_FAST_CHANNEL_DB_عرض_64:
#إذا __riscv_xlen != 32
		db_val_u64 = readq((فراغ *)cppc->fc_db_addr);
		db_val_u64 = cppc->fc_db_setmask |
				(db_val_u64 & cppc->fc_db_preservemask);

		writeq(db_val_u64, (فراغ *)cppc->fc_db_addr);
#وإلا
		db_val_u64 = readl((فراغ *)(cppc->fc_db_addr + 4));
		db_val_u64 <<= 32;
		db_val_u64 |= readl((فراغ *)cppc->fc_db_addr);
		db_val_u64 = cppc->fc_db_setmask |
				(db_val_u64 & cppc->fc_db_preservemask);
		writel(db_val_u64, (فراغ *)cppc->fc_db_addr);
		writel(db_val_u64 >> 32, (فراغ *)(cppc->fc_db_addr + 4));
#پایان_اگر
		اكسر;
	افتراضي:
		اكسر;
	}
}

ساكن صحيح rpmi_cppc_read(بیعلامت دراز reg, u64 *val)
{
	u64 curr_freq, curr_زمان_csr, زمان_csr_delta, freq;
	صحيح rc = SBI_SUCCESS;
	بنية rpmi_cppc_read_reg_req req;
	بنية rpmi_cppc_read_reg_resp resp;
	بنية rpmi_cppc *cppc;

	req.hart_id = current_hartid();
	req.reg_id = reg;
	cppc = rpmi_cppc_get_اشاره_گر(req.hart_id);

	إذا ((reg == SBI_CPPC_DELIVERED_CTR) && cppc->fc_supported) {
#إذا __riscv_xlen != 32
		curr_freq = readq((فراغ *)cppc->fc_perf_feedback_addr);
#وإلا
		curr_freq = readl((فراغ *)cppc->fc_perf_feedback_addr + 4);
		curr_freq <<= 32;
		curr_freq |= readl((فراغ *)cppc->fc_perf_feedback_addr);
#پایان_اگر
		curr_زمان_csr = sbi_timer_مقدار();
		زمان_csr_delta = curr_زمان_csr - cppc->prev_زمان_csr;
		cppc->prev_زمان_csr = curr_زمان_csr;

		freq = sbi_timer_frequency();
		إذا (freq)
			cppc->prev_delivered_counter +=
				curr_freq * (زمان_csr_delta / freq) +
				(curr_freq * (زمان_csr_delta % freq)) / freq;

		resp.داده_lo = cpu_مقصد_le32(cppc->prev_delivered_counter & UINT32_MAX);
		resp.داده_hi = cpu_مقصد_le32(cppc->prev_delivered_counter >> 32);
	} وإلا إذا (reg == SBI_CPPC_REFERENCE_CTR) {
		curr_زمان_csr = sbi_timer_مقدار();
		resp.داده_lo = cpu_مقصد_le32(curr_زمان_csr & UINT32_MAX);
		resp.داده_hi = cpu_مقصد_le32(curr_زمان_csr >> 32);
	} وإلا {
		rc = rpmi_نرمال_request_with_وضعیت(
				cppc->chan, RPMI_CPPC_SRV_READ_REG,
				&req, rpmi_u32_شمار(req), rpmi_u32_شمار(req),
				&resp, rpmi_u32_شمار(resp), rpmi_u32_شمار(resp));
		إذا (rc)
			أرجع rc;
	}

#إذا __riscv_xlen == 32
	*val = resp.داده_lo;
#وإلا
	*val = (u64)resp.داده_hi << 32 | resp.داده_lo;
#پایان_اگر
	أرجع rc;
}

ساكن صحيح rpmi_cppc_write(بیعلامت دراز reg, u64 val)
{
	صحيح rc = SBI_SUCCESS;
	u32 hart_id = current_hartid();
	بنية rpmi_cppc_write_reg_req req;
	بنية rpmi_cppc_write_reg_resp resp;
	بنية rpmi_cppc *cppc = rpmi_cppc_get_اشاره_گر(hart_id);

	إذا (reg != SBI_CPPC_DESIRED_PERF || !cppc->fc_supported) {
		req.hart_id = hart_id;
		req.reg_id = reg;
		req.داده_lo = val & 0xFFFFFFFF;
		req.داده_hi = val >> 32;

		rc = rpmi_نرمال_request_with_وضعیت(
			cppc->chan, RPMI_CPPC_SRV_WRITE_REG,
			&req, rpmi_u32_شمار(req), rpmi_u32_شمار(req),
			&resp, rpmi_u32_شمار(resp), rpmi_u32_شمار(resp));
	} وإلا {
		/* use fast path writes for desired_perf in passive mode */
		writel((u32)val, (فراغ *)cppc->fc_perf_request_addr);

		إذا (cppc->fc_db_supported)
			rpmi_cppc_fc_db_trigger(cppc);
	}

	أرجع rc;
}

ساكن صحيح rpmi_cppc_probe(بیعلامت دراز reg)
{
	صحيح rc;
	بنية rpmi_cppc *cppc;
	بنية rpmi_cppc_probe_resp resp;
	بنية rpmi_cppc_probe_req req;

	req.hart_id = current_hartid();
	req.reg_id = reg;

	cppc = rpmi_cppc_get_اشاره_گر(req.hart_id);
	إذا (!cppc)
		أرجع SBI_ENOSYS;

	rc = rpmi_نرمال_request_with_وضعیت(
			cppc->chan, RPMI_CPPC_SRV_PROBE_REG,
			&req, rpmi_u32_شمار(req), rpmi_u32_شمار(req),
			&resp, rpmi_u32_شمار(resp), rpmi_u32_شمار(resp));
	إذا (rc)
		أرجع rc;

	أرجع resp.reg_len;
}

ساكن بنية sbi_cppc_device sbi_rpmi_cppc = {
	.name		= "rpmi-cppc",
	.cppc_read	= rpmi_cppc_read,
	.cppc_write	= rpmi_cppc_write,
	.cppc_probe	= rpmi_cppc_probe,
};

ساكن صحيح rpmi_cppc_update_hart_scratch(بنية mbox_chan *chan)
{
	صحيح rc, i;
	bool fc_supported = false;
	bool fc_db_supported = false;
	بنية rpmi_cppc_hart_list_req req;
	بنية rpmi_cppc_hart_list_resp resp;
	بنية rpmi_cppc_get_fastchan_جابه_جایی_req hfreq;
	بنية rpmi_cppc_get_fastchan_جابه_جایی_resp hfresp;
	بنية rpmi_cppc_get_fastchan_region_resp fresp;
	تعداد rpmi_cppc_fast_channel_db_عرض fc_db_عرض = 0;
	تعداد rpmi_cppc_fast_channel_cppc_نما cppc_نما = 0;
	بنية rpmi_cppc *cppc;
	بیعلامت دراز fc_region_addr = 0;
	بیعلامت دراز fc_region_اندازه = 0;
	بیعلامت دراز fc_db_addr = 0;
	u64 fc_db_setmask = 0;
	u64 fc_db_preservemask = 0;

	rc = rpmi_نرمال_request_with_وضعیت(
		chan, RPMI_CPPC_SRV_GET_FAST_CHANNEL_REGION,
		عدم, 0, 0,
		&fresp, rpmi_u32_شمار(fresp), rpmi_u32_شمار(fresp));
	إذا (rc && rc != SBI_ENOTSUPP)
		أرجع rc;

	/* At this point fast channel availability is confirmed */
	fc_supported = (rc != SBI_ENOTSUPP)? true : false;

	/* If fast channel is supported, add the fast channel
	 * region in root domain as MMIO RW. And, get the doorbell
	 * information from the response */
	إذا (fc_supported) {
#إذا __riscv_xlen == 32
		fc_region_addr = fresp.region_addr_lo;
		fc_region_اندازه = fresp.region_اندازه_lo;
		fc_db_addr = fresp.db_addr_lo;
#وإلا
		fc_region_addr = (ulong)fresp.region_addr_hi << 32 |
							fresp.region_addr_lo;
		fc_region_اندازه = (ulong)fresp.region_اندازه_hi << 32 |
							fresp.region_اندازه_lo;
		fc_db_addr = (ulong)fresp.db_addr_hi << 32 | fresp.db_addr_lo;

#پایان_اگر
		rc = sbi_domain_ریشه_جمع_کن_memrange(fc_region_addr,
					fc_region_اندازه,
					RPMI_CPPC_HART_FASTCHAN_اندازه,
					(SBI_DOMAIN_MEMREGION_MMIO |
					SBI_DOMAIN_MEMREGION_M_READABLE |
					SBI_DOMAIN_MEMREGION_M_WRITABLE));
		إذا (rc)
			أرجع rc;

		cppc_نما = (fresp.flags & RPMI_CPPC_FAST_CHANNEL_CPPC_نما_ماسک) >>
					RPMI_CPPC_FAST_CHANNEL_CPPC_نما_POS;
		fc_db_supported = fresp.flags &
				RPMI_CPPC_FAST_CHANNEL_FLAGS_DB_SUPPORTED;
		fc_db_عرض = (fresp.flags &
				RPMI_CPPC_FAST_CHANNEL_FLAGS_DB_عرض_ماسک) >>
				RPMI_CPPC_FAST_CHANNEL_FLAGS_DB_عرض_POS;
		fc_db_setmask = (u64)fresp.db_setmask_hi << 32 |
						fresp.db_setmask_lo;
		fc_db_preservemask = (u64)fresp.db_preservemask_hi << 32 |
						fresp.db_preservemask_lo;
	}

	/* Get the hart list and depending on the fast channel support
	 * initialize the per hart cppc structure */
	req.شروع_شاخص = 0;
	افعل {
		rc = rpmi_نرمال_request_with_وضعیت(
			chan, RPMI_CPPC_SRV_GET_HART_LIST,
			&req, rpmi_u32_شمار(req), rpmi_u32_شمار(req),
			&resp, rpmi_u32_شمار(resp), rpmi_u32_شمار(resp));
		إذا (rc)
			أرجع rc;

		/* For each returned harts, get their fastchan offset and
		 * complete the initialization of per hart cppc structure */
		لكل (i = 0; i < resp.returned; i++) {
			cppc = rpmi_cppc_get_اشاره_گر(resp.hartid[i]);
			إذا (!cppc)
				أرجع SBI_ENOSYS;

			cppc->prev_زمان_csr = 0;
			cppc->prev_delivered_counter = 0;
			cppc->chan = chan;
			cppc->نما = cppc_نما;
			cppc->fc_supported = fc_supported;

			إذا (fc_supported) {
				hfreq.hart_id = resp.hartid[i];
				rc = rpmi_نرمال_request_with_وضعیت(
						chan,
						RPMI_CPPC_SRV_GET_FAST_CHANNEL_جابه_جایی,
						&hfreq,
						rpmi_u32_شمار(hfreq),
						rpmi_u32_شمار(hfreq),
						&hfresp,
						rpmi_u32_شمار(hfresp),
						rpmi_u32_شمار(hfresp));
				إذا (rc)
					استمر;

#إذا __riscv_xlen == 32
				cppc->fc_perf_request_addr = fc_region_addr +
							hfresp.fc_perf_request_جابه_جایی_lo;
				cppc->fc_perf_feedback_addr = fc_region_addr +
							hfresp.fc_perf_feedback_جابه_جایی_lo;
#وإلا
				cppc->fc_perf_request_addr = fc_region_addr +
					((ulong)hfresp.fc_perf_request_جابه_جایی_hi << 32 |
					hfresp.fc_perf_request_جابه_جایی_lo);

				cppc->fc_perf_feedback_addr = fc_region_addr +
					((ulong)hfresp.fc_perf_feedback_جابه_جایی_hi << 32 |
					hfresp.fc_perf_feedback_جابه_جایی_lo);
#پایان_اگر
				cppc->fc_db_supported = fc_db_supported;
				cppc->fc_db_addr = fc_db_addr;
				cppc->fc_db_عرض = fc_db_عرض;

				cppc->fc_db_setmask = fc_db_setmask;
				cppc->fc_db_preservemask = fc_db_preservemask;
			}
			وإلا {
				cppc->fc_perf_request_addr = 0;
				cppc->fc_perf_feedback_addr = 0;
				cppc->fc_db_supported = 0;
				cppc->fc_db_addr = 0;
				cppc->fc_db_عرض = 0;
				cppc->fc_db_setmask = 0;
				cppc->fc_db_preservemask = 0;
			}
		}
		req.شروع_شاخص += resp.returned;
	} طالما (resp.مانده);

	أرجع 0;
}

ساكن صحيح rpmi_cppc_cold_init(ثابت فراغ *fdt, صحيح nodeoff,
			       ثابت بنية fdt_تطابق *تطابق)
{
	صحيح rc;
	بنية mbox_chan *chan;

	إذا (!rpmi_cppc_جابه_جایی) {
		rpmi_cppc_جابه_جایی =
			sbi_scratch_alloc_type_جابه_جایی(بنية rpmi_cppc);
		إذا (!rpmi_cppc_جابه_جایی)
			أرجع SBI_ENOMEM;
	}

	/*
	 * If channel request failed then other end does not support
	 * CPPC service group so do nothing.
	 */
	rc = fdt_mailbox_request_chan(fdt, nodeoff, 0, &chan);
	إذا (rc)
		أرجع SBI_ENODEV;

	/* Update per-HART scratch space */
	rc = rpmi_cppc_update_hart_scratch(chan);
	إذا (rc)
		أرجع rc;

	sbi_cppc_مجموعه_device(&sbi_rpmi_cppc);

	أرجع 0;
}

ساكن ثابت بنية fdt_تطابق rpmi_cppc_تطابق[] = {
	{ .compatible = "riscv,rpmi-cppc" },
	{},
};

ثابت بنية fdt_driver fdt_cppc_rpmi = {
	.تطابق_جدول = rpmi_cppc_تطابق,
	.init = rpmi_cppc_cold_init,
};
