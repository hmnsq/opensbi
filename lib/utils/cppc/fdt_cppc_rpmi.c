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
#تعریف_کړه RPMI_CPPC_HART_FASTCHAN_اندازه		0x8

بنية rpmi_cppc {
	بنية mbox_chan *chan;
	bool fc_supported;
	bool fc_db_supported;
	تعداد rpmi_cppc_fast_channel_db_پلنوالی fc_db_پلنوالی;
	تعداد rpmi_cppc_fast_channel_cppc_ډېرپېښ ډېرپېښ;
	ulong fc_perf_request_addr;
	ulong fc_perf_feedback_addr;
	ulong fc_db_addr;
	u64 fc_db_setmask;
	u64 fc_db_preservemask;
	u64 prev_وخت_csr;
	u64 prev_delivered_counter;
};

ساكن بینښه اوږد rpmi_cppc_دننه_واټن;

ساكن بنية rpmi_cppc *rpmi_cppc_get_اشاره_ګر(u32 hartid)
{
	بنية sbi_scratch *scratch;

	scratch = sbi_hartid_منزل_scratch(hartid);
	إذا (!scratch || !rpmi_cppc_دننه_واټن)
		أرجع عدم;

	أرجع sbi_scratch_دننه_واټن_ptr(scratch, rpmi_cppc_دننه_واټن);
}

ساكن فراغ rpmi_cppc_fc_db_trigger(بنية rpmi_cppc *cppc)
{
	u8 db_val_u8 = 0;
	u16 db_val_u16 = 0;
	u32 db_val_u32 = 0;
	u64 db_val_u64 = 0;

	بدل (cppc->fc_db_پلنوالی) {
	حالة RPMI_CPPC_FAST_CHANNEL_DB_پلنوالی_8:
		db_val_u8 = readb((فراغ *)cppc->fc_db_addr);
		db_val_u8 = (u8)cppc->fc_db_setmask |
				(db_val_u8 & (u8)cppc->fc_db_preservemask);

		writeb(db_val_u8, (فراغ *)cppc->fc_db_addr);
		اكسر;
	حالة RPMI_CPPC_FAST_CHANNEL_DB_پلنوالی_16:
		db_val_u16 = readw((فراغ *)cppc->fc_db_addr);
		db_val_u16 = (u16)cppc->fc_db_setmask |
				(db_val_u16 & (u16)cppc->fc_db_preservemask);

		writew(db_val_u16, (فراغ *)cppc->fc_db_addr);
		اكسر;
	حالة RPMI_CPPC_FAST_CHANNEL_DB_پلنوالی_32:
		db_val_u32 = readl((فراغ *)cppc->fc_db_addr);
		db_val_u32 = (u32)cppc->fc_db_setmask |
				(db_val_u32 & (u32)cppc->fc_db_preservemask);

		writel(db_val_u32, (فراغ *)cppc->fc_db_addr);
		اكسر;
	حالة RPMI_CPPC_FAST_CHANNEL_DB_پلنوالی_64:
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
#د_که_پای
		اكسر;
	افتراضي:
		اكسر;
	}
}

ساكن صحيح rpmi_cppc_read(بینښه اوږد reg, u64 *val)
{
	u64 curr_freq, curr_وخت_csr, وخت_csr_delta, freq;
	صحيح rc = SBI_SUCCESS;
	بنية rpmi_cppc_read_reg_req req;
	بنية rpmi_cppc_read_reg_resp resp;
	بنية rpmi_cppc *cppc;

	req.hart_id = current_hartid();
	req.reg_id = reg;
	cppc = rpmi_cppc_get_اشاره_ګر(req.hart_id);

	إذا ((reg == SBI_CPPC_DELIVERED_CTR) && cppc->fc_supported) {
#إذا __riscv_xlen != 32
		curr_freq = readq((فراغ *)cppc->fc_perf_feedback_addr);
#وإلا
		curr_freq = readl((فراغ *)cppc->fc_perf_feedback_addr + 4);
		curr_freq <<= 32;
		curr_freq |= readl((فراغ *)cppc->fc_perf_feedback_addr);
#د_که_پای
		curr_وخت_csr = sbi_timer_ارزښت();
		وخت_csr_delta = curr_وخت_csr - cppc->prev_وخت_csr;
		cppc->prev_وخت_csr = curr_وخت_csr;

		freq = sbi_timer_frequency();
		إذا (freq)
			cppc->prev_delivered_counter +=
				curr_freq * (وخت_csr_delta / freq) +
				(curr_freq * (وخت_csr_delta % freq)) / freq;

		resp.معلومات_lo = cpu_منزل_le32(cppc->prev_delivered_counter & UINT32_MAX);
		resp.معلومات_hi = cpu_منزل_le32(cppc->prev_delivered_counter >> 32);
	} وإلا إذا (reg == SBI_CPPC_REFERENCE_CTR) {
		curr_وخت_csr = sbi_timer_ارزښت();
		resp.معلومات_lo = cpu_منزل_le32(curr_وخت_csr & UINT32_MAX);
		resp.معلومات_hi = cpu_منزل_le32(curr_وخت_csr >> 32);
	} وإلا {
		rc = rpmi_نورمال_request_with_حالت(
				cppc->chan, RPMI_CPPC_SRV_READ_REG,
				&req, rpmi_u32_شمېر(req), rpmi_u32_شمېر(req),
				&resp, rpmi_u32_شمېر(resp), rpmi_u32_شمېر(resp));
		إذا (rc)
			أرجع rc;
	}

#إذا __riscv_xlen == 32
	*val = resp.معلومات_lo;
#وإلا
	*val = (u64)resp.معلومات_hi << 32 | resp.معلومات_lo;
#د_که_پای
	أرجع rc;
}

ساكن صحيح rpmi_cppc_write(بینښه اوږد reg, u64 val)
{
	صحيح rc = SBI_SUCCESS;
	u32 hart_id = current_hartid();
	بنية rpmi_cppc_write_reg_req req;
	بنية rpmi_cppc_write_reg_resp resp;
	بنية rpmi_cppc *cppc = rpmi_cppc_get_اشاره_ګر(hart_id);

	إذا (reg != SBI_CPPC_DESIRED_PERF || !cppc->fc_supported) {
		req.hart_id = hart_id;
		req.reg_id = reg;
		req.معلومات_lo = val & 0xFFFFFFFF;
		req.معلومات_hi = val >> 32;

		rc = rpmi_نورمال_request_with_حالت(
			cppc->chan, RPMI_CPPC_SRV_WRITE_REG,
			&req, rpmi_u32_شمېر(req), rpmi_u32_شمېر(req),
			&resp, rpmi_u32_شمېر(resp), rpmi_u32_شمېر(resp));
	} وإلا {
		/* use fast path writes for desired_perf in passive mode */
		writel((u32)val, (فراغ *)cppc->fc_perf_request_addr);

		إذا (cppc->fc_db_supported)
			rpmi_cppc_fc_db_trigger(cppc);
	}

	أرجع rc;
}

ساكن صحيح rpmi_cppc_probe(بینښه اوږد reg)
{
	صحيح rc;
	بنية rpmi_cppc *cppc;
	بنية rpmi_cppc_probe_resp resp;
	بنية rpmi_cppc_probe_req req;

	req.hart_id = current_hartid();
	req.reg_id = reg;

	cppc = rpmi_cppc_get_اشاره_ګر(req.hart_id);
	إذا (!cppc)
		أرجع SBI_ENOSYS;

	rc = rpmi_نورمال_request_with_حالت(
			cppc->chan, RPMI_CPPC_SRV_PROBE_REG,
			&req, rpmi_u32_شمېر(req), rpmi_u32_شمېر(req),
			&resp, rpmi_u32_شمېر(resp), rpmi_u32_شمېر(resp));
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
	بنية rpmi_cppc_get_fastchan_دننه_واټن_req hfreq;
	بنية rpmi_cppc_get_fastchan_دننه_واټن_resp hfresp;
	بنية rpmi_cppc_get_fastchan_region_resp fresp;
	تعداد rpmi_cppc_fast_channel_db_پلنوالی fc_db_پلنوالی = 0;
	تعداد rpmi_cppc_fast_channel_cppc_ډېرپېښ cppc_ډېرپېښ = 0;
	بنية rpmi_cppc *cppc;
	بینښه اوږد fc_region_addr = 0;
	بینښه اوږد fc_region_اندازه = 0;
	بینښه اوږد fc_db_addr = 0;
	u64 fc_db_setmask = 0;
	u64 fc_db_preservemask = 0;

	rc = rpmi_نورمال_request_with_حالت(
		chan, RPMI_CPPC_SRV_GET_FAST_CHANNEL_REGION,
		عدم, 0, 0,
		&fresp, rpmi_u32_شمېر(fresp), rpmi_u32_شمېر(fresp));
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

#د_که_پای
		rc = sbi_domain_ریښه_جمع_کړه_memrange(fc_region_addr,
					fc_region_اندازه,
					RPMI_CPPC_HART_FASTCHAN_اندازه,
					(SBI_DOMAIN_MEMREGION_MMIO |
					SBI_DOMAIN_MEMREGION_M_READABLE |
					SBI_DOMAIN_MEMREGION_M_WRITABLE));
		إذا (rc)
			أرجع rc;

		cppc_ډېرپېښ = (fresp.flags & RPMI_CPPC_FAST_CHANNEL_CPPC_ډېرپېښ_ماسک) >>
					RPMI_CPPC_FAST_CHANNEL_CPPC_ډېرپېښ_POS;
		fc_db_supported = fresp.flags &
				RPMI_CPPC_FAST_CHANNEL_FLAGS_DB_SUPPORTED;
		fc_db_پلنوالی = (fresp.flags &
				RPMI_CPPC_FAST_CHANNEL_FLAGS_DB_پلنوالی_ماسک) >>
				RPMI_CPPC_FAST_CHANNEL_FLAGS_DB_پلنوالی_POS;
		fc_db_setmask = (u64)fresp.db_setmask_hi << 32 |
						fresp.db_setmask_lo;
		fc_db_preservemask = (u64)fresp.db_preservemask_hi << 32 |
						fresp.db_preservemask_lo;
	}

	/* Get the hart list and depending on the fast channel support
	 * initialize the per hart cppc structure */
	req.پیل_شاخص = 0;
	افعل {
		rc = rpmi_نورمال_request_with_حالت(
			chan, RPMI_CPPC_SRV_GET_HART_LIST,
			&req, rpmi_u32_شمېر(req), rpmi_u32_شمېر(req),
			&resp, rpmi_u32_شمېر(resp), rpmi_u32_شمېر(resp));
		إذا (rc)
			أرجع rc;

		/* For each returned harts, get their fastchan offset and
		 * complete the initialization of per hart cppc structure */
		لكل (i = 0; i < resp.returned; i++) {
			cppc = rpmi_cppc_get_اشاره_ګر(resp.hartid[i]);
			إذا (!cppc)
				أرجع SBI_ENOSYS;

			cppc->prev_وخت_csr = 0;
			cppc->prev_delivered_counter = 0;
			cppc->chan = chan;
			cppc->ډېرپېښ = cppc_ډېرپېښ;
			cppc->fc_supported = fc_supported;

			إذا (fc_supported) {
				hfreq.hart_id = resp.hartid[i];
				rc = rpmi_نورمال_request_with_حالت(
						chan,
						RPMI_CPPC_SRV_GET_FAST_CHANNEL_دننه_واټن,
						&hfreq,
						rpmi_u32_شمېر(hfreq),
						rpmi_u32_شمېر(hfreq),
						&hfresp,
						rpmi_u32_شمېر(hfresp),
						rpmi_u32_شمېر(hfresp));
				إذا (rc)
					استمر;

#إذا __riscv_xlen == 32
				cppc->fc_perf_request_addr = fc_region_addr +
							hfresp.fc_perf_request_دننه_واټن_lo;
				cppc->fc_perf_feedback_addr = fc_region_addr +
							hfresp.fc_perf_feedback_دننه_واټن_lo;
#وإلا
				cppc->fc_perf_request_addr = fc_region_addr +
					((ulong)hfresp.fc_perf_request_دننه_واټن_hi << 32 |
					hfresp.fc_perf_request_دننه_واټن_lo);

				cppc->fc_perf_feedback_addr = fc_region_addr +
					((ulong)hfresp.fc_perf_feedback_دننه_واټن_hi << 32 |
					hfresp.fc_perf_feedback_دننه_واټن_lo);
#د_که_پای
				cppc->fc_db_supported = fc_db_supported;
				cppc->fc_db_addr = fc_db_addr;
				cppc->fc_db_پلنوالی = fc_db_پلنوالی;

				cppc->fc_db_setmask = fc_db_setmask;
				cppc->fc_db_preservemask = fc_db_preservemask;
			}
			وإلا {
				cppc->fc_perf_request_addr = 0;
				cppc->fc_perf_feedback_addr = 0;
				cppc->fc_db_supported = 0;
				cppc->fc_db_addr = 0;
				cppc->fc_db_پلنوالی = 0;
				cppc->fc_db_setmask = 0;
				cppc->fc_db_preservemask = 0;
			}
		}
		req.پیل_شاخص += resp.returned;
	} طالما (resp.پاتې_شوی);

	أرجع 0;
}

ساكن صحيح rpmi_cppc_cold_init(ثابت فراغ *fdt, صحيح nodeoff,
			       ثابت بنية fdt_سمون *سمون)
{
	صحيح rc;
	بنية mbox_chan *chan;

	إذا (!rpmi_cppc_دننه_واټن) {
		rpmi_cppc_دننه_واټن =
			sbi_scratch_alloc_type_دننه_واټن(بنية rpmi_cppc);
		إذا (!rpmi_cppc_دننه_واټن)
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

	sbi_cppc_سېټ_device(&sbi_rpmi_cppc);

	أرجع 0;
}

ساكن ثابت بنية fdt_سمون rpmi_cppc_سمون[] = {
	{ .compatible = "riscv,rpmi-cppc" },
	{},
};

ثابت بنية fdt_driver fdt_cppc_rpmi = {
	.سمون_جدول = rpmi_cppc_سمون,
	.init = rpmi_cppc_cold_init,
};
