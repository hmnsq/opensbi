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
#وصف_ڪريو RPMI_CPPC_HART_FASTCHAN_سائيز		0x8

بنية rpmi_cppc {
	بنية mbox_chan *chan;
	bool fc_supported;
	bool fc_db_supported;
	تعداد rpmi_cppc_fast_channel_db_ويڪر fc_db_ويڪر;
	تعداد rpmi_cppc_fast_channel_cppc_گهڻائي گهڻائي;
	ulong fc_perf_request_addr;
	ulong fc_perf_feedback_addr;
	ulong fc_db_addr;
	u64 fc_db_setmask;
	u64 fc_db_preservemask;
	u64 prev_وقت_csr;
	u64 prev_delivered_counter;
};

ساكن بنانشان ڊگهو rpmi_cppc_وٿي;

ساكن بنية rpmi_cppc *rpmi_cppc_get_اشارو(u32 hartid)
{
	بنية sbi_scratch *scratch;

	scratch = sbi_hartid_منزل_scratch(hartid);
	إذا (!scratch || !rpmi_cppc_وٿي)
		أرجع عدم;

	أرجع sbi_scratch_وٿي_ptr(scratch, rpmi_cppc_وٿي);
}

ساكن فراغ rpmi_cppc_fc_db_trigger(بنية rpmi_cppc *cppc)
{
	u8 db_val_u8 = 0;
	u16 db_val_u16 = 0;
	u32 db_val_u32 = 0;
	u64 db_val_u64 = 0;

	بدل (cppc->fc_db_ويڪر) {
	حالة RPMI_CPPC_FAST_CHANNEL_DB_ويڪر_8:
		db_val_u8 = readb((فراغ *)cppc->fc_db_addr);
		db_val_u8 = (u8)cppc->fc_db_setmask |
				(db_val_u8 & (u8)cppc->fc_db_preservemask);

		writeb(db_val_u8, (فراغ *)cppc->fc_db_addr);
		اكسر;
	حالة RPMI_CPPC_FAST_CHANNEL_DB_ويڪر_16:
		db_val_u16 = readw((فراغ *)cppc->fc_db_addr);
		db_val_u16 = (u16)cppc->fc_db_setmask |
				(db_val_u16 & (u16)cppc->fc_db_preservemask);

		writew(db_val_u16, (فراغ *)cppc->fc_db_addr);
		اكسر;
	حالة RPMI_CPPC_FAST_CHANNEL_DB_ويڪر_32:
		db_val_u32 = readl((فراغ *)cppc->fc_db_addr);
		db_val_u32 = (u32)cppc->fc_db_setmask |
				(db_val_u32 & (u32)cppc->fc_db_preservemask);

		writel(db_val_u32, (فراغ *)cppc->fc_db_addr);
		اكسر;
	حالة RPMI_CPPC_FAST_CHANNEL_DB_ويڪر_64:
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
#جيڪڏهن_پورو
		اكسر;
	افتراضي:
		اكسر;
	}
}

ساكن صحيح rpmi_cppc_read(بنانشان ڊگهو reg, u64 *val)
{
	u64 curr_freq, curr_وقت_csr, وقت_csr_delta, freq;
	صحيح rc = SBI_SUCCESS;
	بنية rpmi_cppc_read_reg_req req;
	بنية rpmi_cppc_read_reg_resp resp;
	بنية rpmi_cppc *cppc;

	req.hart_id = current_hartid();
	req.reg_id = reg;
	cppc = rpmi_cppc_get_اشارو(req.hart_id);

	إذا ((reg == SBI_CPPC_DELIVERED_CTR) && cppc->fc_supported) {
#إذا __riscv_xlen != 32
		curr_freq = readq((فراغ *)cppc->fc_perf_feedback_addr);
#وإلا
		curr_freq = readl((فراغ *)cppc->fc_perf_feedback_addr + 4);
		curr_freq <<= 32;
		curr_freq |= readl((فراغ *)cppc->fc_perf_feedback_addr);
#جيڪڏهن_پورو
		curr_وقت_csr = sbi_timer_قدر();
		وقت_csr_delta = curr_وقت_csr - cppc->prev_وقت_csr;
		cppc->prev_وقت_csr = curr_وقت_csr;

		freq = sbi_timer_frequency();
		إذا (freq)
			cppc->prev_delivered_counter +=
				curr_freq * (وقت_csr_delta / freq) +
				(curr_freq * (وقت_csr_delta % freq)) / freq;

		resp.معلومات_lo = cpu_منزل_le32(cppc->prev_delivered_counter & UINT32_MAX);
		resp.معلومات_hi = cpu_منزل_le32(cppc->prev_delivered_counter >> 32);
	} وإلا إذا (reg == SBI_CPPC_REFERENCE_CTR) {
		curr_وقت_csr = sbi_timer_قدر();
		resp.معلومات_lo = cpu_منزل_le32(curr_وقت_csr & UINT32_MAX);
		resp.معلومات_hi = cpu_منزل_le32(curr_وقت_csr >> 32);
	} وإلا {
		rc = rpmi_عام_request_with_حالت(
				cppc->chan, RPMI_CPPC_SRV_READ_REG,
				&req, rpmi_u32_ڳڻپ(req), rpmi_u32_ڳڻپ(req),
				&resp, rpmi_u32_ڳڻپ(resp), rpmi_u32_ڳڻپ(resp));
		إذا (rc)
			أرجع rc;
	}

#إذا __riscv_xlen == 32
	*val = resp.معلومات_lo;
#وإلا
	*val = (u64)resp.معلومات_hi << 32 | resp.معلومات_lo;
#جيڪڏهن_پورو
	أرجع rc;
}

ساكن صحيح rpmi_cppc_write(بنانشان ڊگهو reg, u64 val)
{
	صحيح rc = SBI_SUCCESS;
	u32 hart_id = current_hartid();
	بنية rpmi_cppc_write_reg_req req;
	بنية rpmi_cppc_write_reg_resp resp;
	بنية rpmi_cppc *cppc = rpmi_cppc_get_اشارو(hart_id);

	إذا (reg != SBI_CPPC_DESIRED_PERF || !cppc->fc_supported) {
		req.hart_id = hart_id;
		req.reg_id = reg;
		req.معلومات_lo = val & 0xFFFFFFFF;
		req.معلومات_hi = val >> 32;

		rc = rpmi_عام_request_with_حالت(
			cppc->chan, RPMI_CPPC_SRV_WRITE_REG,
			&req, rpmi_u32_ڳڻپ(req), rpmi_u32_ڳڻپ(req),
			&resp, rpmi_u32_ڳڻپ(resp), rpmi_u32_ڳڻپ(resp));
	} وإلا {
		/* use fast path writes for desired_perf in passive mode */
		writel((u32)val, (فراغ *)cppc->fc_perf_request_addr);

		إذا (cppc->fc_db_supported)
			rpmi_cppc_fc_db_trigger(cppc);
	}

	أرجع rc;
}

ساكن صحيح rpmi_cppc_probe(بنانشان ڊگهو reg)
{
	صحيح rc;
	بنية rpmi_cppc *cppc;
	بنية rpmi_cppc_probe_resp resp;
	بنية rpmi_cppc_probe_req req;

	req.hart_id = current_hartid();
	req.reg_id = reg;

	cppc = rpmi_cppc_get_اشارو(req.hart_id);
	إذا (!cppc)
		أرجع SBI_ENOSYS;

	rc = rpmi_عام_request_with_حالت(
			cppc->chan, RPMI_CPPC_SRV_PROBE_REG,
			&req, rpmi_u32_ڳڻپ(req), rpmi_u32_ڳڻپ(req),
			&resp, rpmi_u32_ڳڻپ(resp), rpmi_u32_ڳڻپ(resp));
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
	بنية rpmi_cppc_get_fastchan_وٿي_req hfreq;
	بنية rpmi_cppc_get_fastchan_وٿي_resp hfresp;
	بنية rpmi_cppc_get_fastchan_region_resp fresp;
	تعداد rpmi_cppc_fast_channel_db_ويڪر fc_db_ويڪر = 0;
	تعداد rpmi_cppc_fast_channel_cppc_گهڻائي cppc_گهڻائي = 0;
	بنية rpmi_cppc *cppc;
	بنانشان ڊگهو fc_region_addr = 0;
	بنانشان ڊگهو fc_region_سائيز = 0;
	بنانشان ڊگهو fc_db_addr = 0;
	u64 fc_db_setmask = 0;
	u64 fc_db_preservemask = 0;

	rc = rpmi_عام_request_with_حالت(
		chan, RPMI_CPPC_SRV_GET_FAST_CHANNEL_REGION,
		عدم, 0, 0,
		&fresp, rpmi_u32_ڳڻپ(fresp), rpmi_u32_ڳڻپ(fresp));
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
		fc_region_سائيز = fresp.region_سائيز_lo;
		fc_db_addr = fresp.db_addr_lo;
#وإلا
		fc_region_addr = (ulong)fresp.region_addr_hi << 32 |
							fresp.region_addr_lo;
		fc_region_سائيز = (ulong)fresp.region_سائيز_hi << 32 |
							fresp.region_سائيز_lo;
		fc_db_addr = (ulong)fresp.db_addr_hi << 32 | fresp.db_addr_lo;

#جيڪڏهن_پورو
		rc = sbi_domain_پاڙ_جوڙيو_memrange(fc_region_addr,
					fc_region_سائيز,
					RPMI_CPPC_HART_FASTCHAN_سائيز,
					(SBI_DOMAIN_MEMREGION_MMIO |
					SBI_DOMAIN_MEMREGION_M_READABLE |
					SBI_DOMAIN_MEMREGION_M_WRITABLE));
		إذا (rc)
			أرجع rc;

		cppc_گهڻائي = (fresp.flags & RPMI_CPPC_FAST_CHANNEL_CPPC_گهڻائي_نقاب) >>
					RPMI_CPPC_FAST_CHANNEL_CPPC_گهڻائي_POS;
		fc_db_supported = fresp.flags &
				RPMI_CPPC_FAST_CHANNEL_FLAGS_DB_SUPPORTED;
		fc_db_ويڪر = (fresp.flags &
				RPMI_CPPC_FAST_CHANNEL_FLAGS_DB_ويڪر_نقاب) >>
				RPMI_CPPC_FAST_CHANNEL_FLAGS_DB_ويڪر_POS;
		fc_db_setmask = (u64)fresp.db_setmask_hi << 32 |
						fresp.db_setmask_lo;
		fc_db_preservemask = (u64)fresp.db_preservemask_hi << 32 |
						fresp.db_preservemask_lo;
	}

	/* Get the hart list and depending on the fast channel support
	 * initialize the per hart cppc structure */
	req.شروعات_اشاريو = 0;
	افعل {
		rc = rpmi_عام_request_with_حالت(
			chan, RPMI_CPPC_SRV_GET_HART_LIST,
			&req, rpmi_u32_ڳڻپ(req), rpmi_u32_ڳڻپ(req),
			&resp, rpmi_u32_ڳڻپ(resp), rpmi_u32_ڳڻپ(resp));
		إذا (rc)
			أرجع rc;

		/* For each returned harts, get their fastchan offset and
		 * complete the initialization of per hart cppc structure */
		لكل (i = 0; i < resp.returned; i++) {
			cppc = rpmi_cppc_get_اشارو(resp.hartid[i]);
			إذا (!cppc)
				أرجع SBI_ENOSYS;

			cppc->prev_وقت_csr = 0;
			cppc->prev_delivered_counter = 0;
			cppc->chan = chan;
			cppc->گهڻائي = cppc_گهڻائي;
			cppc->fc_supported = fc_supported;

			إذا (fc_supported) {
				hfreq.hart_id = resp.hartid[i];
				rc = rpmi_عام_request_with_حالت(
						chan,
						RPMI_CPPC_SRV_GET_FAST_CHANNEL_وٿي,
						&hfreq,
						rpmi_u32_ڳڻپ(hfreq),
						rpmi_u32_ڳڻپ(hfreq),
						&hfresp,
						rpmi_u32_ڳڻپ(hfresp),
						rpmi_u32_ڳڻپ(hfresp));
				إذا (rc)
					استمر;

#إذا __riscv_xlen == 32
				cppc->fc_perf_request_addr = fc_region_addr +
							hfresp.fc_perf_request_وٿي_lo;
				cppc->fc_perf_feedback_addr = fc_region_addr +
							hfresp.fc_perf_feedback_وٿي_lo;
#وإلا
				cppc->fc_perf_request_addr = fc_region_addr +
					((ulong)hfresp.fc_perf_request_وٿي_hi << 32 |
					hfresp.fc_perf_request_وٿي_lo);

				cppc->fc_perf_feedback_addr = fc_region_addr +
					((ulong)hfresp.fc_perf_feedback_وٿي_hi << 32 |
					hfresp.fc_perf_feedback_وٿي_lo);
#جيڪڏهن_پورو
				cppc->fc_db_supported = fc_db_supported;
				cppc->fc_db_addr = fc_db_addr;
				cppc->fc_db_ويڪر = fc_db_ويڪر;

				cppc->fc_db_setmask = fc_db_setmask;
				cppc->fc_db_preservemask = fc_db_preservemask;
			}
			وإلا {
				cppc->fc_perf_request_addr = 0;
				cppc->fc_perf_feedback_addr = 0;
				cppc->fc_db_supported = 0;
				cppc->fc_db_addr = 0;
				cppc->fc_db_ويڪر = 0;
				cppc->fc_db_setmask = 0;
				cppc->fc_db_preservemask = 0;
			}
		}
		req.شروعات_اشاريو += resp.returned;
	} طالما (resp.بچيل);

	أرجع 0;
}

ساكن صحيح rpmi_cppc_cold_init(ثابت فراغ *fdt, صحيح nodeoff,
			       ثابت بنية fdt_ميلاپ *ميلاپ)
{
	صحيح rc;
	بنية mbox_chan *chan;

	إذا (!rpmi_cppc_وٿي) {
		rpmi_cppc_وٿي =
			sbi_scratch_alloc_type_وٿي(بنية rpmi_cppc);
		إذا (!rpmi_cppc_وٿي)
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

	sbi_cppc_سيٽ_device(&sbi_rpmi_cppc);

	أرجع 0;
}

ساكن ثابت بنية fdt_ميلاپ rpmi_cppc_ميلاپ[] = {
	{ .compatible = "riscv,rpmi-cppc" },
	{},
};

ثابت بنية fdt_driver fdt_cppc_rpmi = {
	.ميلاپ_جدول = rpmi_cppc_ميلاپ,
	.init = rpmi_cppc_cold_init,
};
