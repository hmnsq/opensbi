/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Subrahmanya Lingappa <slingappa@ventanamicro.com>
 */

#अन्तर्भाव <libfdt.h>
#अन्तर्भाव <sbi/riscv_io.h>
#अन्तर्भाव <sbi/sbi_cppc.h>
#अन्तर्भाव <sbi/sbi_ecall_interface.h>
#अन्तर्भाव <sbi/sbi_scratch.h>
#अन्तर्भाव <sbi/sbi_timer.h>
#अन्तर्भाव <sbi_utils/fdt/fdt_driver.h>
#अन्तर्भाव <sbi_utils/fdt/fdt_helper.h>
#अन्तर्भाव <sbi_utils/mailbox/fdt_mailbox.h>
#अन्तर्भाव <sbi_utils/mailbox/rpmi_mailbox.h>

/**
 * Per hart RPMI CPPC fast channel size (bytes)
 * PASSIVE MODE:
 *	0x0: DESIRED_PERFORMANCE (4-byte)
 *	0x4: __RESERVED	(4-byte)
 * ACTIVE MODE: (not supported yet)
 *	0x0: MINIMUM PERFORMANCE (4-byte)
 *	0x4: MAXIMUM PERFORMANCE (4-byte)
 */
#परिभाषय RPMI_CPPC_HART_FASTCHAN_आकार		0x8

संरचना rpmi_cppc {
	संरचना mbox_chan *chan;
	bool fc_supported;
	bool fc_db_supported;
	गणना rpmi_cppc_fast_channel_db_विस्तारः fc_db_विस्तारः;
	गणना rpmi_cppc_fast_channel_cppc_बहुलकम् बहुलकम्;
	ulong fc_perf_request_addr;
	ulong fc_perf_feedback_addr;
	ulong fc_db_addr;
	u64 fc_db_setmask;
	u64 fc_db_preservemask;
	u64 पूर्व_time_csr;
	u64 पूर्व_delivered_गणक;
};

स्थिर अचिह्नित दीर्घ rpmi_cppc_विस्थापनम्;

स्थिर संरचना rpmi_cppc *rpmi_cppc_get_pointer(u32 hartid)
{
	संरचना sbi_scratch *scratch;

	scratch = sbi_hartid_गन्तव्यम्_scratch(hartid);
	यदि (!scratch || !rpmi_cppc_विस्थापनम्)
		प्रत्यागम रिक्त;

	प्रत्यागम sbi_scratch_विस्थापनम्_ptr(scratch, rpmi_cppc_विस्थापनम्);
}

स्थिर शून्य rpmi_cppc_fc_db_trigger(संरचना rpmi_cppc *cppc)
{
	u8 db_val_u8 = 0;
	u16 db_val_u16 = 0;
	u32 db_val_u32 = 0;
	u64 db_val_u64 = 0;

	विकल्प (cppc->fc_db_विस्तारः) {
	पक्ष RPMI_CPPC_FAST_CHANNEL_DB_विस्तारः_8:
		db_val_u8 = readb((शून्य *)cppc->fc_db_addr);
		db_val_u8 = (u8)cppc->fc_db_setmask |
				(db_val_u8 & (u8)cppc->fc_db_preservemask);

		writeb(db_val_u8, (शून्य *)cppc->fc_db_addr);
		भङ्ग;
	पक्ष RPMI_CPPC_FAST_CHANNEL_DB_विस्तारः_16:
		db_val_u16 = readw((शून्य *)cppc->fc_db_addr);
		db_val_u16 = (u16)cppc->fc_db_setmask |
				(db_val_u16 & (u16)cppc->fc_db_preservemask);

		writew(db_val_u16, (शून्य *)cppc->fc_db_addr);
		भङ्ग;
	पक्ष RPMI_CPPC_FAST_CHANNEL_DB_विस्तारः_32:
		db_val_u32 = readl((शून्य *)cppc->fc_db_addr);
		db_val_u32 = (u32)cppc->fc_db_setmask |
				(db_val_u32 & (u32)cppc->fc_db_preservemask);

		writel(db_val_u32, (शून्य *)cppc->fc_db_addr);
		भङ्ग;
	पक्ष RPMI_CPPC_FAST_CHANNEL_DB_विस्तारः_64:
#यदि __riscv_xlen != 32
		db_val_u64 = readq((शून्य *)cppc->fc_db_addr);
		db_val_u64 = cppc->fc_db_setmask |
				(db_val_u64 & cppc->fc_db_preservemask);

		writeq(db_val_u64, (शून्य *)cppc->fc_db_addr);
#अन्यथा
		db_val_u64 = readl((शून्य *)(cppc->fc_db_addr + 4));
		db_val_u64 <<= 32;
		db_val_u64 |= readl((शून्य *)cppc->fc_db_addr);
		db_val_u64 = cppc->fc_db_setmask |
				(db_val_u64 & cppc->fc_db_preservemask);
		writel(db_val_u64, (शून्य *)cppc->fc_db_addr);
		writel(db_val_u64 >> 32, (शून्य *)(cppc->fc_db_addr + 4));
#यदि_अन्तः
		भङ्ग;
	स्वभाव:
		भङ्ग;
	}
}

स्थिर पूर्णाङ्क rpmi_cppc_read(अचिह्नित दीर्घ reg, u64 *val)
{
	u64 curr_freq, curr_time_csr, time_csr_delta, freq;
	पूर्णाङ्क rc = SBI_SUCCESS;
	संरचना rpmi_cppc_read_reg_req req;
	संरचना rpmi_cppc_read_reg_resp resp;
	संरचना rpmi_cppc *cppc;

	req.hart_id = current_hartid();
	req.reg_id = reg;
	cppc = rpmi_cppc_get_pointer(req.hart_id);

	यदि ((reg == SBI_CPPC_DELIVERED_CTR) && cppc->fc_supported) {
#यदि __riscv_xlen != 32
		curr_freq = readq((शून्य *)cppc->fc_perf_feedback_addr);
#अन्यथा
		curr_freq = readl((शून्य *)cppc->fc_perf_feedback_addr + 4);
		curr_freq <<= 32;
		curr_freq |= readl((शून्य *)cppc->fc_perf_feedback_addr);
#यदि_अन्तः
		curr_time_csr = sbi_timer_मानम्();
		time_csr_delta = curr_time_csr - cppc->पूर्व_time_csr;
		cppc->पूर्व_time_csr = curr_time_csr;

		freq = sbi_timer_frequency();
		यदि (freq)
			cppc->पूर्व_delivered_गणक +=
				curr_freq * (time_csr_delta / freq) +
				(curr_freq * (time_csr_delta % freq)) / freq;

		resp.दत्त_lo = cpu_गन्तव्यम्_le32(cppc->पूर्व_delivered_गणक & UINT32_MAX);
		resp.दत्त_hi = cpu_गन्तव्यम्_le32(cppc->पूर्व_delivered_गणक >> 32);
	} अन्यथा यदि (reg == SBI_CPPC_REFERENCE_CTR) {
		curr_time_csr = sbi_timer_मानम्();
		resp.दत्त_lo = cpu_गन्तव्यम्_le32(curr_time_csr & UINT32_MAX);
		resp.दत्त_hi = cpu_गन्तव्यम्_le32(curr_time_csr >> 32);
	} अन्यथा {
		rc = rpmi_प्रसामान्यम्_request_with_स्थितिः(
				cppc->chan, RPMI_CPPC_SRV_READ_REG,
				&req, rpmi_u32_गणनम्(req), rpmi_u32_गणनम्(req),
				&resp, rpmi_u32_गणनम्(resp), rpmi_u32_गणनम्(resp));
		यदि (rc)
			प्रत्यागम rc;
	}

#यदि __riscv_xlen == 32
	*val = resp.दत्त_lo;
#अन्यथा
	*val = (u64)resp.दत्त_hi << 32 | resp.दत्त_lo;
#यदि_अन्तः
	प्रत्यागम rc;
}

स्थिर पूर्णाङ्क rpmi_cppc_write(अचिह्नित दीर्घ reg, u64 val)
{
	पूर्णाङ्क rc = SBI_SUCCESS;
	u32 hart_id = current_hartid();
	संरचना rpmi_cppc_write_reg_req req;
	संरचना rpmi_cppc_write_reg_resp resp;
	संरचना rpmi_cppc *cppc = rpmi_cppc_get_pointer(hart_id);

	यदि (reg != SBI_CPPC_DESIRED_PERF || !cppc->fc_supported) {
		req.hart_id = hart_id;
		req.reg_id = reg;
		req.दत्त_lo = val & 0xFFFFFFFF;
		req.दत्त_hi = val >> 32;

		rc = rpmi_प्रसामान्यम्_request_with_स्थितिः(
			cppc->chan, RPMI_CPPC_SRV_WRITE_REG,
			&req, rpmi_u32_गणनम्(req), rpmi_u32_गणनम्(req),
			&resp, rpmi_u32_गणनम्(resp), rpmi_u32_गणनम्(resp));
	} अन्यथा {
		/* use fast path writes for desired_perf in passive mode */
		writel((u32)val, (शून्य *)cppc->fc_perf_request_addr);

		यदि (cppc->fc_db_supported)
			rpmi_cppc_fc_db_trigger(cppc);
	}

	प्रत्यागम rc;
}

स्थिर पूर्णाङ्क rpmi_cppc_probe(अचिह्नित दीर्घ reg)
{
	पूर्णाङ्क rc;
	संरचना rpmi_cppc *cppc;
	संरचना rpmi_cppc_probe_resp resp;
	संरचना rpmi_cppc_probe_req req;

	req.hart_id = current_hartid();
	req.reg_id = reg;

	cppc = rpmi_cppc_get_pointer(req.hart_id);
	यदि (!cppc)
		प्रत्यागम SBI_ENOSYS;

	rc = rpmi_प्रसामान्यम्_request_with_स्थितिः(
			cppc->chan, RPMI_CPPC_SRV_PROBE_REG,
			&req, rpmi_u32_गणनम्(req), rpmi_u32_गणनम्(req),
			&resp, rpmi_u32_गणनम्(resp), rpmi_u32_गणनम्(resp));
	यदि (rc)
		प्रत्यागम rc;

	प्रत्यागम resp.reg_len;
}

स्थिर संरचना sbi_cppc_device sbi_rpmi_cppc = {
	.name		= "rpmi-cppc",
	.cppc_read	= rpmi_cppc_read,
	.cppc_write	= rpmi_cppc_write,
	.cppc_probe	= rpmi_cppc_probe,
};

स्थिर पूर्णाङ्क rpmi_cppc_update_hart_scratch(संरचना mbox_chan *chan)
{
	पूर्णाङ्क rc, i;
	bool fc_supported = false;
	bool fc_db_supported = false;
	संरचना rpmi_cppc_hart_list_req req;
	संरचना rpmi_cppc_hart_list_resp resp;
	संरचना rpmi_cppc_get_fastchan_विस्थापनम्_req hfreq;
	संरचना rpmi_cppc_get_fastchan_विस्थापनम्_resp hfresp;
	संरचना rpmi_cppc_get_fastchan_region_resp fresp;
	गणना rpmi_cppc_fast_channel_db_विस्तारः fc_db_विस्तारः = 0;
	गणना rpmi_cppc_fast_channel_cppc_बहुलकम् cppc_बहुलकम् = 0;
	संरचना rpmi_cppc *cppc;
	अचिह्नित दीर्घ fc_region_addr = 0;
	अचिह्नित दीर्घ fc_region_आकार = 0;
	अचिह्नित दीर्घ fc_db_addr = 0;
	u64 fc_db_setmask = 0;
	u64 fc_db_preservemask = 0;

	rc = rpmi_प्रसामान्यम्_request_with_स्थितिः(
		chan, RPMI_CPPC_SRV_GET_FAST_CHANNEL_REGION,
		रिक्त, 0, 0,
		&fresp, rpmi_u32_गणनम्(fresp), rpmi_u32_गणनम्(fresp));
	यदि (rc && rc != SBI_ENOTSUPP)
		प्रत्यागम rc;

	/* At this point fast channel availability is confirmed */
	fc_supported = (rc != SBI_ENOTSUPP)? true : false;

	/* If fast channel is supported, add the fast channel
	 * region in root domain as MMIO RW. And, get the doorbell
	 * information from the response */
	यदि (fc_supported) {
#यदि __riscv_xlen == 32
		fc_region_addr = fresp.region_addr_lo;
		fc_region_आकार = fresp.region_आकार_lo;
		fc_db_addr = fresp.db_addr_lo;
#अन्यथा
		fc_region_addr = (ulong)fresp.region_addr_hi << 32 |
							fresp.region_addr_lo;
		fc_region_आकार = (ulong)fresp.region_आकार_hi << 32 |
							fresp.region_आकार_lo;
		fc_db_addr = (ulong)fresp.db_addr_hi << 32 | fresp.db_addr_lo;

#यदि_अन्तः
		rc = sbi_domain_मूलम्_सङ्कलय_memrange(fc_region_addr,
					fc_region_आकार,
					RPMI_CPPC_HART_FASTCHAN_आकार,
					(SBI_DOMAIN_MEMREGION_MMIO |
					SBI_DOMAIN_MEMREGION_M_READABLE |
					SBI_DOMAIN_MEMREGION_M_WRITABLE));
		यदि (rc)
			प्रत्यागम rc;

		cppc_बहुलकम् = (fresp.flags & RPMI_CPPC_FAST_CHANNEL_CPPC_बहुलकम्_आवरणम्) >>
					RPMI_CPPC_FAST_CHANNEL_CPPC_बहुलकम्_POS;
		fc_db_supported = fresp.flags &
				RPMI_CPPC_FAST_CHANNEL_FLAGS_DB_SUPPORTED;
		fc_db_विस्तारः = (fresp.flags &
				RPMI_CPPC_FAST_CHANNEL_FLAGS_DB_विस्तारः_आवरणम्) >>
				RPMI_CPPC_FAST_CHANNEL_FLAGS_DB_विस्तारः_POS;
		fc_db_setmask = (u64)fresp.db_setmask_hi << 32 |
						fresp.db_setmask_lo;
		fc_db_preservemask = (u64)fresp.db_preservemask_hi << 32 |
						fresp.db_preservemask_lo;
	}

	/* Get the hart list and depending on the fast channel support
	 * initialize the per hart cppc structure */
	req.start_सूचकांक = 0;
	कुरु {
		rc = rpmi_प्रसामान्यम्_request_with_स्थितिः(
			chan, RPMI_CPPC_SRV_GET_HART_LIST,
			&req, rpmi_u32_गणनम्(req), rpmi_u32_गणनम्(req),
			&resp, rpmi_u32_गणनम्(resp), rpmi_u32_गणनम्(resp));
		यदि (rc)
			प्रत्यागम rc;

		/* For each returned harts, get their fastchan offset and
		 * complete the initialization of per hart cppc structure */
		प्रति (i = 0; i < resp.returned; i++) {
			cppc = rpmi_cppc_get_pointer(resp.hartid[i]);
			यदि (!cppc)
				प्रत्यागम SBI_ENOSYS;

			cppc->पूर्व_time_csr = 0;
			cppc->पूर्व_delivered_गणक = 0;
			cppc->chan = chan;
			cppc->बहुलकम् = cppc_बहुलकम्;
			cppc->fc_supported = fc_supported;

			यदि (fc_supported) {
				hfreq.hart_id = resp.hartid[i];
				rc = rpmi_प्रसामान्यम्_request_with_स्थितिः(
						chan,
						RPMI_CPPC_SRV_GET_FAST_CHANNEL_विस्थापनम्,
						&hfreq,
						rpmi_u32_गणनम्(hfreq),
						rpmi_u32_गणनम्(hfreq),
						&hfresp,
						rpmi_u32_गणनम्(hfresp),
						rpmi_u32_गणनम्(hfresp));
				यदि (rc)
					अनुवर्त;

#यदि __riscv_xlen == 32
				cppc->fc_perf_request_addr = fc_region_addr +
							hfresp.fc_perf_request_विस्थापनम्_lo;
				cppc->fc_perf_feedback_addr = fc_region_addr +
							hfresp.fc_perf_feedback_विस्थापनम्_lo;
#अन्यथा
				cppc->fc_perf_request_addr = fc_region_addr +
					((ulong)hfresp.fc_perf_request_विस्थापनम्_hi << 32 |
					hfresp.fc_perf_request_विस्थापनम्_lo);

				cppc->fc_perf_feedback_addr = fc_region_addr +
					((ulong)hfresp.fc_perf_feedback_विस्थापनम्_hi << 32 |
					hfresp.fc_perf_feedback_विस्थापनम्_lo);
#यदि_अन्तः
				cppc->fc_db_supported = fc_db_supported;
				cppc->fc_db_addr = fc_db_addr;
				cppc->fc_db_विस्तारः = fc_db_विस्तारः;

				cppc->fc_db_setmask = fc_db_setmask;
				cppc->fc_db_preservemask = fc_db_preservemask;
			}
			अन्यथा {
				cppc->fc_perf_request_addr = 0;
				cppc->fc_perf_feedback_addr = 0;
				cppc->fc_db_supported = 0;
				cppc->fc_db_addr = 0;
				cppc->fc_db_विस्तारः = 0;
				cppc->fc_db_setmask = 0;
				cppc->fc_db_preservemask = 0;
			}
		}
		req.start_सूचकांक += resp.returned;
	} यावत् (resp.remaining);

	प्रत्यागम 0;
}

स्थिर पूर्णाङ्क rpmi_cppc_cold_init(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoff,
			       ध्रुव संरचना fdt_मेलनम् *मेलनम्)
{
	पूर्णाङ्क rc;
	संरचना mbox_chan *chan;

	यदि (!rpmi_cppc_विस्थापनम्) {
		rpmi_cppc_विस्थापनम् =
			sbi_scratch_alloc_type_विस्थापनम्(संरचना rpmi_cppc);
		यदि (!rpmi_cppc_विस्थापनम्)
			प्रत्यागम SBI_ENOMEM;
	}

	/*
	 * If channel request failed then other end does not support
	 * CPPC service group so do nothing.
	 */
	rc = fdt_mailbox_request_chan(fdt, nodeoff, 0, &chan);
	यदि (rc)
		प्रत्यागम SBI_ENODEV;

	/* Update per-HART scratch space */
	rc = rpmi_cppc_update_hart_scratch(chan);
	यदि (rc)
		प्रत्यागम rc;

	sbi_cppc_समुच्चयः_device(&sbi_rpmi_cppc);

	प्रत्यागम 0;
}

स्थिर ध्रुव संरचना fdt_मेलनम् rpmi_cppc_मेलनम्[] = {
	{ .compatible = "riscv,rpmi-cppc" },
	{},
};

ध्रुव संरचना fdt_driver fdt_cppc_rpmi = {
	.मेलनम्_सारणी = rpmi_cppc_मेलनम्,
	.init = rpmi_cppc_cold_init,
};
