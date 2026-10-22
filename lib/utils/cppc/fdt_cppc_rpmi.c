/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Subrahmanya Lingappa <slingappa@ventanamicro.com>
 */

#شامل <libfdt.h>
#شامل <sbi/riscv_io.h>
#شامل <sbi/sbi_cppc.h>
#شامل <sbi/sbi_ecall_interface.h>
#شامل <sbi/sbi_scratch.h>
#شامل <sbi/sbi_timer.h>
#شامل <sbi_utils/fdt/fdt_driver.h>
#شامل <sbi_utils/fdt/fdt_helper.h>
#شامل <sbi_utils/mailbox/fdt_mailbox.h>
#شامل <sbi_utils/mailbox/rpmi_mailbox.h>

/**
 * Per hart RPMI CPPC fast channel size (bytes)
 * PASSIVE MODE:
 *	0x0: DESIRED_PERFORMANCE (4-byte)
 *	0x4: __RESERVED	(4-byte)
 * ACTIVE MODE: (not supported yet)
 *	0x0: MINIMUM PERFORMANCE (4-byte)
 *	0x4: MAXIMUM PERFORMANCE (4-byte)
 */
#تعریف_کرو RPMI_CPPC_HART_FASTCHAN_سائز		0x8

ساخت rpmi_cppc {
	ساخت mbox_chan *chan;
	bool fc_supported;
	bool fc_db_supported;
	شمار rpmi_cppc_fast_channel_db_چوڑائی fc_db_چوڑائی;
	شمار rpmi_cppc_fast_channel_cppc_منوال منوال;
	ulong fc_perf_request_addr;
	ulong fc_perf_feedback_addr;
	ulong fc_db_addr;
	u64 fc_db_setmask;
	u64 fc_db_preservemask;
	u64 پچھلا_وقت_csr;
	u64 پچھلا_delivered_شمار;
};

ساکن غیرعلامتی طویل rpmi_cppc_انحراف;

ساکن ساخت rpmi_cppc *rpmi_cppc_get_اشارہ_گر(u32 hartid)
{
	ساخت sbi_scratch *scratch;

	scratch = sbi_hartid_مقصد_scratch(hartid);
	اگر (!scratch || !rpmi_cppc_انحراف)
		واپس تہی;

	واپس sbi_scratch_انحراف_ptr(scratch, rpmi_cppc_انحراف);
}

ساکن خالی rpmi_cppc_fc_db_trigger(ساخت rpmi_cppc *cppc)
{
	u8 db_val_u8 = 0;
	u16 db_val_u16 = 0;
	u32 db_val_u32 = 0;
	u64 db_val_u64 = 0;

	انتخاب (cppc->fc_db_چوڑائی) {
	حالت RPMI_CPPC_FAST_CHANNEL_DB_چوڑائی_8:
		db_val_u8 = readb((خالی *)cppc->fc_db_addr);
		db_val_u8 = (u8)cppc->fc_db_setmask |
				(db_val_u8 & (u8)cppc->fc_db_preservemask);

		writeb(db_val_u8, (خالی *)cppc->fc_db_addr);
		توڑو;
	حالت RPMI_CPPC_FAST_CHANNEL_DB_چوڑائی_16:
		db_val_u16 = readw((خالی *)cppc->fc_db_addr);
		db_val_u16 = (u16)cppc->fc_db_setmask |
				(db_val_u16 & (u16)cppc->fc_db_preservemask);

		writew(db_val_u16, (خالی *)cppc->fc_db_addr);
		توڑو;
	حالت RPMI_CPPC_FAST_CHANNEL_DB_چوڑائی_32:
		db_val_u32 = readl((خالی *)cppc->fc_db_addr);
		db_val_u32 = (u32)cppc->fc_db_setmask |
				(db_val_u32 & (u32)cppc->fc_db_preservemask);

		writel(db_val_u32, (خالی *)cppc->fc_db_addr);
		توڑو;
	حالت RPMI_CPPC_FAST_CHANNEL_DB_چوڑائی_64:
#اگر __riscv_xlen != 32
		db_val_u64 = readq((خالی *)cppc->fc_db_addr);
		db_val_u64 = cppc->fc_db_setmask |
				(db_val_u64 & cppc->fc_db_preservemask);

		writeq(db_val_u64, (خالی *)cppc->fc_db_addr);
#ورنہ
		db_val_u64 = readl((خالی *)(cppc->fc_db_addr + 4));
		db_val_u64 <<= 32;
		db_val_u64 |= readl((خالی *)cppc->fc_db_addr);
		db_val_u64 = cppc->fc_db_setmask |
				(db_val_u64 & cppc->fc_db_preservemask);
		writel(db_val_u64, (خالی *)cppc->fc_db_addr);
		writel(db_val_u64 >> 32, (خالی *)(cppc->fc_db_addr + 4));
#اگر_ختم
		توڑو;
	طےشدہ:
		توڑو;
	}
}

ساکن صحیح rpmi_cppc_read(غیرعلامتی طویل reg, u64 *val)
{
	u64 curr_freq, curr_وقت_csr, وقت_csr_delta, freq;
	صحیح rc = SBI_SUCCESS;
	ساخت rpmi_cppc_read_reg_req req;
	ساخت rpmi_cppc_read_reg_resp resp;
	ساخت rpmi_cppc *cppc;

	req.hart_id = current_hartid();
	req.reg_id = reg;
	cppc = rpmi_cppc_get_اشارہ_گر(req.hart_id);

	اگر ((reg == SBI_CPPC_DELIVERED_CTR) && cppc->fc_supported) {
#اگر __riscv_xlen != 32
		curr_freq = readq((خالی *)cppc->fc_perf_feedback_addr);
#ورنہ
		curr_freq = readl((خالی *)cppc->fc_perf_feedback_addr + 4);
		curr_freq <<= 32;
		curr_freq |= readl((خالی *)cppc->fc_perf_feedback_addr);
#اگر_ختم
		curr_وقت_csr = sbi_timer_قدر();
		وقت_csr_delta = curr_وقت_csr - cppc->پچھلا_وقت_csr;
		cppc->پچھلا_وقت_csr = curr_وقت_csr;

		freq = sbi_timer_frequency();
		اگر (freq)
			cppc->پچھلا_delivered_شمار +=
				curr_freq * (وقت_csr_delta / freq) +
				(curr_freq * (وقت_csr_delta % freq)) / freq;

		resp.ڈیٹا_lo = cpu_مقصد_le32(cppc->پچھلا_delivered_شمار & UINT32_MAX);
		resp.ڈیٹا_hi = cpu_مقصد_le32(cppc->پچھلا_delivered_شمار >> 32);
	} ورنہ اگر (reg == SBI_CPPC_REFERENCE_CTR) {
		curr_وقت_csr = sbi_timer_قدر();
		resp.ڈیٹا_lo = cpu_مقصد_le32(curr_وقت_csr & UINT32_MAX);
		resp.ڈیٹا_hi = cpu_مقصد_le32(curr_وقت_csr >> 32);
	} ورنہ {
		rc = rpmi_معمول_request_with_حالت(
				cppc->chan, RPMI_CPPC_SRV_READ_REG,
				&req, rpmi_u32_گنتی(req), rpmi_u32_گنتی(req),
				&resp, rpmi_u32_گنتی(resp), rpmi_u32_گنتی(resp));
		اگر (rc)
			واپس rc;
	}

#اگر __riscv_xlen == 32
	*val = resp.ڈیٹا_lo;
#ورنہ
	*val = (u64)resp.ڈیٹا_hi << 32 | resp.ڈیٹا_lo;
#اگر_ختم
	واپس rc;
}

ساکن صحیح rpmi_cppc_write(غیرعلامتی طویل reg, u64 val)
{
	صحیح rc = SBI_SUCCESS;
	u32 hart_id = current_hartid();
	ساخت rpmi_cppc_write_reg_req req;
	ساخت rpmi_cppc_write_reg_resp resp;
	ساخت rpmi_cppc *cppc = rpmi_cppc_get_اشارہ_گر(hart_id);

	اگر (reg != SBI_CPPC_DESIRED_PERF || !cppc->fc_supported) {
		req.hart_id = hart_id;
		req.reg_id = reg;
		req.ڈیٹا_lo = val & 0xFFFFFFFF;
		req.ڈیٹا_hi = val >> 32;

		rc = rpmi_معمول_request_with_حالت(
			cppc->chan, RPMI_CPPC_SRV_WRITE_REG,
			&req, rpmi_u32_گنتی(req), rpmi_u32_گنتی(req),
			&resp, rpmi_u32_گنتی(resp), rpmi_u32_گنتی(resp));
	} ورنہ {
		/* use fast path writes for desired_perf in passive mode */
		writel((u32)val, (خالی *)cppc->fc_perf_request_addr);

		اگر (cppc->fc_db_supported)
			rpmi_cppc_fc_db_trigger(cppc);
	}

	واپس rc;
}

ساکن صحیح rpmi_cppc_probe(غیرعلامتی طویل reg)
{
	صحیح rc;
	ساخت rpmi_cppc *cppc;
	ساخت rpmi_cppc_probe_resp resp;
	ساخت rpmi_cppc_probe_req req;

	req.hart_id = current_hartid();
	req.reg_id = reg;

	cppc = rpmi_cppc_get_اشارہ_گر(req.hart_id);
	اگر (!cppc)
		واپس SBI_ENOSYS;

	rc = rpmi_معمول_request_with_حالت(
			cppc->chan, RPMI_CPPC_SRV_PROBE_REG,
			&req, rpmi_u32_گنتی(req), rpmi_u32_گنتی(req),
			&resp, rpmi_u32_گنتی(resp), rpmi_u32_گنتی(resp));
	اگر (rc)
		واپس rc;

	واپس resp.reg_len;
}

ساکن ساخت sbi_cppc_device sbi_rpmi_cppc = {
	.name		= "rpmi-cppc",
	.cppc_read	= rpmi_cppc_read,
	.cppc_write	= rpmi_cppc_write,
	.cppc_probe	= rpmi_cppc_probe,
};

ساکن صحیح rpmi_cppc_update_hart_scratch(ساخت mbox_chan *chan)
{
	صحیح rc, i;
	bool fc_supported = false;
	bool fc_db_supported = false;
	ساخت rpmi_cppc_hart_list_req req;
	ساخت rpmi_cppc_hart_list_resp resp;
	ساخت rpmi_cppc_get_fastchan_انحراف_req hfreq;
	ساخت rpmi_cppc_get_fastchan_انحراف_resp hfresp;
	ساخت rpmi_cppc_get_fastchan_region_resp fresp;
	شمار rpmi_cppc_fast_channel_db_چوڑائی fc_db_چوڑائی = 0;
	شمار rpmi_cppc_fast_channel_cppc_منوال cppc_منوال = 0;
	ساخت rpmi_cppc *cppc;
	غیرعلامتی طویل fc_region_addr = 0;
	غیرعلامتی طویل fc_region_سائز = 0;
	غیرعلامتی طویل fc_db_addr = 0;
	u64 fc_db_setmask = 0;
	u64 fc_db_preservemask = 0;

	rc = rpmi_معمول_request_with_حالت(
		chan, RPMI_CPPC_SRV_GET_FAST_CHANNEL_REGION,
		تہی, 0, 0,
		&fresp, rpmi_u32_گنتی(fresp), rpmi_u32_گنتی(fresp));
	اگر (rc && rc != SBI_ENOTSUPP)
		واپس rc;

	/* At this point fast channel availability is confirmed */
	fc_supported = (rc != SBI_ENOTSUPP)? true : false;

	/* If fast channel is supported, add the fast channel
	 * region in root domain as MMIO RW. And, get the doorbell
	 * information from the response */
	اگر (fc_supported) {
#اگر __riscv_xlen == 32
		fc_region_addr = fresp.region_addr_lo;
		fc_region_سائز = fresp.region_سائز_lo;
		fc_db_addr = fresp.db_addr_lo;
#ورنہ
		fc_region_addr = (ulong)fresp.region_addr_hi << 32 |
							fresp.region_addr_lo;
		fc_region_سائز = (ulong)fresp.region_سائز_hi << 32 |
							fresp.region_سائز_lo;
		fc_db_addr = (ulong)fresp.db_addr_hi << 32 | fresp.db_addr_lo;

#اگر_ختم
		rc = sbi_domain_جڑ_جوڑو_memrange(fc_region_addr,
					fc_region_سائز,
					RPMI_CPPC_HART_FASTCHAN_سائز,
					(SBI_DOMAIN_MEMREGION_MMIO |
					SBI_DOMAIN_MEMREGION_M_READABLE |
					SBI_DOMAIN_MEMREGION_M_WRITABLE));
		اگر (rc)
			واپس rc;

		cppc_منوال = (fresp.flags & RPMI_CPPC_FAST_CHANNEL_CPPC_منوال_نقاب) >>
					RPMI_CPPC_FAST_CHANNEL_CPPC_منوال_POS;
		fc_db_supported = fresp.flags &
				RPMI_CPPC_FAST_CHANNEL_FLAGS_DB_SUPPORTED;
		fc_db_چوڑائی = (fresp.flags &
				RPMI_CPPC_FAST_CHANNEL_FLAGS_DB_چوڑائی_نقاب) >>
				RPMI_CPPC_FAST_CHANNEL_FLAGS_DB_چوڑائی_POS;
		fc_db_setmask = (u64)fresp.db_setmask_hi << 32 |
						fresp.db_setmask_lo;
		fc_db_preservemask = (u64)fresp.db_preservemask_hi << 32 |
						fresp.db_preservemask_lo;
	}

	/* Get the hart list and depending on the fast channel support
	 * initialize the per hart cppc structure */
	req.آغاز_اشاریہ = 0;
	کرو {
		rc = rpmi_معمول_request_with_حالت(
			chan, RPMI_CPPC_SRV_GET_HART_LIST,
			&req, rpmi_u32_گنتی(req), rpmi_u32_گنتی(req),
			&resp, rpmi_u32_گنتی(resp), rpmi_u32_گنتی(resp));
		اگر (rc)
			واپس rc;

		/* For each returned harts, get their fastchan offset and
		 * complete the initialization of per hart cppc structure */
		برائے (i = 0; i < resp.returned; i++) {
			cppc = rpmi_cppc_get_اشارہ_گر(resp.hartid[i]);
			اگر (!cppc)
				واپس SBI_ENOSYS;

			cppc->پچھلا_وقت_csr = 0;
			cppc->پچھلا_delivered_شمار = 0;
			cppc->chan = chan;
			cppc->منوال = cppc_منوال;
			cppc->fc_supported = fc_supported;

			اگر (fc_supported) {
				hfreq.hart_id = resp.hartid[i];
				rc = rpmi_معمول_request_with_حالت(
						chan,
						RPMI_CPPC_SRV_GET_FAST_CHANNEL_انحراف,
						&hfreq,
						rpmi_u32_گنتی(hfreq),
						rpmi_u32_گنتی(hfreq),
						&hfresp,
						rpmi_u32_گنتی(hfresp),
						rpmi_u32_گنتی(hfresp));
				اگر (rc)
					جاری;

#اگر __riscv_xlen == 32
				cppc->fc_perf_request_addr = fc_region_addr +
							hfresp.fc_perf_request_انحراف_lo;
				cppc->fc_perf_feedback_addr = fc_region_addr +
							hfresp.fc_perf_feedback_انحراف_lo;
#ورنہ
				cppc->fc_perf_request_addr = fc_region_addr +
					((ulong)hfresp.fc_perf_request_انحراف_hi << 32 |
					hfresp.fc_perf_request_انحراف_lo);

				cppc->fc_perf_feedback_addr = fc_region_addr +
					((ulong)hfresp.fc_perf_feedback_انحراف_hi << 32 |
					hfresp.fc_perf_feedback_انحراف_lo);
#اگر_ختم
				cppc->fc_db_supported = fc_db_supported;
				cppc->fc_db_addr = fc_db_addr;
				cppc->fc_db_چوڑائی = fc_db_چوڑائی;

				cppc->fc_db_setmask = fc_db_setmask;
				cppc->fc_db_preservemask = fc_db_preservemask;
			}
			ورنہ {
				cppc->fc_perf_request_addr = 0;
				cppc->fc_perf_feedback_addr = 0;
				cppc->fc_db_supported = 0;
				cppc->fc_db_addr = 0;
				cppc->fc_db_چوڑائی = 0;
				cppc->fc_db_setmask = 0;
				cppc->fc_db_preservemask = 0;
			}
		}
		req.آغاز_اشاریہ += resp.returned;
	} جب (resp.بقایا);

	واپس 0;
}

ساکن صحیح rpmi_cppc_cold_init(ثابت خالی *fdt, صحیح nodeoff,
			       ثابت ساخت fdt_مطابقت *مطابقت)
{
	صحیح rc;
	ساخت mbox_chan *chan;

	اگر (!rpmi_cppc_انحراف) {
		rpmi_cppc_انحراف =
			sbi_scratch_alloc_type_انحراف(ساخت rpmi_cppc);
		اگر (!rpmi_cppc_انحراف)
			واپس SBI_ENOMEM;
	}

	/*
	 * If channel request failed then other end does not support
	 * CPPC service group so do nothing.
	 */
	rc = fdt_mailbox_request_chan(fdt, nodeoff, 0, &chan);
	اگر (rc)
		واپس SBI_ENODEV;

	/* Update per-HART scratch space */
	rc = rpmi_cppc_update_hart_scratch(chan);
	اگر (rc)
		واپس rc;

	sbi_cppc_سیٹ_device(&sbi_rpmi_cppc);

	واپس 0;
}

ساکن ثابت ساخت fdt_مطابقت rpmi_cppc_مطابقت[] = {
	{ .compatible = "riscv,rpmi-cppc" },
	{},
};

ثابت ساخت fdt_driver fdt_cppc_rpmi = {
	.مطابقت_جدول = rpmi_cppc_مطابقت,
	.init = rpmi_cppc_cold_init,
};
