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
#define RPMI_CPPC_HART_FASTCHAN_SIZE		0x8

संरचना rpmi_cppc {
	संरचना mbox_chan *chan;
	bool fc_supported;
	bool fc_db_supported;
	गणना rpmi_cppc_fast_channel_db_width fc_db_width;
	गणना rpmi_cppc_fast_channel_cppc_mode mode;
	ulong fc_perf_request_addr;
	ulong fc_perf_feedback_addr;
	ulong fc_db_addr;
	u64 fc_db_setmask;
	u64 fc_db_preservemask;
	u64 prev_time_csr;
	u64 prev_delivered_counter;
};

स्थिर अचिन्हित दीर्घ rpmi_cppc_offset;

स्थिर संरचना rpmi_cppc *rpmi_cppc_get_pointer(u32 hartid)
{
	संरचना sbi_scratch *scratch;

	scratch = sbi_hartid_to_scratch(hartid);
	जर (!scratch || !rpmi_cppc_offset)
		परत रिक्त;

	परत sbi_scratch_offset_ptr(scratch, rpmi_cppc_offset);
}

स्थिर शून्य rpmi_cppc_fc_db_trigger(संरचना rpmi_cppc *cppc)
{
	u8 db_val_u8 = 0;
	u16 db_val_u16 = 0;
	u32 db_val_u32 = 0;
	u64 db_val_u64 = 0;

	निवड (cppc->fc_db_width) {
	स्थिती RPMI_CPPC_FAST_CHANNEL_DB_WIDTH_8:
		db_val_u8 = readb((शून्य *)cppc->fc_db_addr);
		db_val_u8 = (u8)cppc->fc_db_setmask |
				(db_val_u8 & (u8)cppc->fc_db_preservemask);

		writeb(db_val_u8, (शून्य *)cppc->fc_db_addr);
		थांबा;
	स्थिती RPMI_CPPC_FAST_CHANNEL_DB_WIDTH_16:
		db_val_u16 = readw((शून्य *)cppc->fc_db_addr);
		db_val_u16 = (u16)cppc->fc_db_setmask |
				(db_val_u16 & (u16)cppc->fc_db_preservemask);

		writew(db_val_u16, (शून्य *)cppc->fc_db_addr);
		थांबा;
	स्थिती RPMI_CPPC_FAST_CHANNEL_DB_WIDTH_32:
		db_val_u32 = readl((शून्य *)cppc->fc_db_addr);
		db_val_u32 = (u32)cppc->fc_db_setmask |
				(db_val_u32 & (u32)cppc->fc_db_preservemask);

		writel(db_val_u32, (शून्य *)cppc->fc_db_addr);
		थांबा;
	स्थिती RPMI_CPPC_FAST_CHANNEL_DB_WIDTH_64:
#जर __riscv_xlen != 32
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
#endif
		थांबा;
	पूर्वनिर्धारित:
		थांबा;
	}
}

स्थिर पूर्णांक rpmi_cppc_read(अचिन्हित दीर्घ reg, u64 *val)
{
	u64 curr_freq, curr_time_csr, time_csr_delta, freq;
	पूर्णांक rc = SBI_SUCCESS;
	संरचना rpmi_cppc_read_reg_req req;
	संरचना rpmi_cppc_read_reg_resp resp;
	संरचना rpmi_cppc *cppc;

	req.hart_id = current_hartid();
	req.reg_id = reg;
	cppc = rpmi_cppc_get_pointer(req.hart_id);

	जर ((reg == SBI_CPPC_DELIVERED_CTR) && cppc->fc_supported) {
#जर __riscv_xlen != 32
		curr_freq = readq((शून्य *)cppc->fc_perf_feedback_addr);
#अन्यथा
		curr_freq = readl((शून्य *)cppc->fc_perf_feedback_addr + 4);
		curr_freq <<= 32;
		curr_freq |= readl((शून्य *)cppc->fc_perf_feedback_addr);
#endif
		curr_time_csr = sbi_timer_मूल्य();
		time_csr_delta = curr_time_csr - cppc->prev_time_csr;
		cppc->prev_time_csr = curr_time_csr;

		freq = sbi_timer_frequency();
		जर (freq)
			cppc->prev_delivered_counter +=
				curr_freq * (time_csr_delta / freq) +
				(curr_freq * (time_csr_delta % freq)) / freq;

		resp.data_lo = cpu_to_le32(cppc->prev_delivered_counter & UINT32_MAX);
		resp.data_hi = cpu_to_le32(cppc->prev_delivered_counter >> 32);
	} अन्यथा जर (reg == SBI_CPPC_REFERENCE_CTR) {
		curr_time_csr = sbi_timer_मूल्य();
		resp.data_lo = cpu_to_le32(curr_time_csr & UINT32_MAX);
		resp.data_hi = cpu_to_le32(curr_time_csr >> 32);
	} अन्यथा {
		rc = rpmi_normal_request_with_status(
				cppc->chan, RPMI_CPPC_SRV_READ_REG,
				&req, rpmi_u32_मोजणी(req), rpmi_u32_मोजणी(req),
				&resp, rpmi_u32_मोजणी(resp), rpmi_u32_मोजणी(resp));
		जर (rc)
			परत rc;
	}

#जर __riscv_xlen == 32
	*val = resp.data_lo;
#अन्यथा
	*val = (u64)resp.data_hi << 32 | resp.data_lo;
#endif
	परत rc;
}

स्थिर पूर्णांक rpmi_cppc_write(अचिन्हित दीर्घ reg, u64 val)
{
	पूर्णांक rc = SBI_SUCCESS;
	u32 hart_id = current_hartid();
	संरचना rpmi_cppc_write_reg_req req;
	संरचना rpmi_cppc_write_reg_resp resp;
	संरचना rpmi_cppc *cppc = rpmi_cppc_get_pointer(hart_id);

	जर (reg != SBI_CPPC_DESIRED_PERF || !cppc->fc_supported) {
		req.hart_id = hart_id;
		req.reg_id = reg;
		req.data_lo = val & 0xFFFFFFFF;
		req.data_hi = val >> 32;

		rc = rpmi_normal_request_with_status(
			cppc->chan, RPMI_CPPC_SRV_WRITE_REG,
			&req, rpmi_u32_मोजणी(req), rpmi_u32_मोजणी(req),
			&resp, rpmi_u32_मोजणी(resp), rpmi_u32_मोजणी(resp));
	} अन्यथा {
		/* use fast path writes for desired_perf in passive mode */
		writel((u32)val, (शून्य *)cppc->fc_perf_request_addr);

		जर (cppc->fc_db_supported)
			rpmi_cppc_fc_db_trigger(cppc);
	}

	परत rc;
}

स्थिर पूर्णांक rpmi_cppc_probe(अचिन्हित दीर्घ reg)
{
	पूर्णांक rc;
	संरचना rpmi_cppc *cppc;
	संरचना rpmi_cppc_probe_resp resp;
	संरचना rpmi_cppc_probe_req req;

	req.hart_id = current_hartid();
	req.reg_id = reg;

	cppc = rpmi_cppc_get_pointer(req.hart_id);
	जर (!cppc)
		परत SBI_ENOSYS;

	rc = rpmi_normal_request_with_status(
			cppc->chan, RPMI_CPPC_SRV_PROBE_REG,
			&req, rpmi_u32_मोजणी(req), rpmi_u32_मोजणी(req),
			&resp, rpmi_u32_मोजणी(resp), rpmi_u32_मोजणी(resp));
	जर (rc)
		परत rc;

	परत resp.reg_len;
}

स्थिर संरचना sbi_cppc_device sbi_rpmi_cppc = {
	.name		= "rpmi-cppc",
	.cppc_read	= rpmi_cppc_read,
	.cppc_write	= rpmi_cppc_write,
	.cppc_probe	= rpmi_cppc_probe,
};

स्थिर पूर्णांक rpmi_cppc_update_hart_scratch(संरचना mbox_chan *chan)
{
	पूर्णांक rc, i;
	bool fc_supported = false;
	bool fc_db_supported = false;
	संरचना rpmi_cppc_hart_list_req req;
	संरचना rpmi_cppc_hart_list_resp resp;
	संरचना rpmi_cppc_get_fastchan_offset_req hfreq;
	संरचना rpmi_cppc_get_fastchan_offset_resp hfresp;
	संरचना rpmi_cppc_get_fastchan_region_resp fresp;
	गणना rpmi_cppc_fast_channel_db_width fc_db_width = 0;
	गणना rpmi_cppc_fast_channel_cppc_mode cppc_mode = 0;
	संरचना rpmi_cppc *cppc;
	अचिन्हित दीर्घ fc_region_addr = 0;
	अचिन्हित दीर्घ fc_region_size = 0;
	अचिन्हित दीर्घ fc_db_addr = 0;
	u64 fc_db_setmask = 0;
	u64 fc_db_preservemask = 0;

	rc = rpmi_normal_request_with_status(
		chan, RPMI_CPPC_SRV_GET_FAST_CHANNEL_REGION,
		रिक्त, 0, 0,
		&fresp, rpmi_u32_मोजणी(fresp), rpmi_u32_मोजणी(fresp));
	जर (rc && rc != SBI_ENOTSUPP)
		परत rc;

	/* At this point fast channel availability is confirmed */
	fc_supported = (rc != SBI_ENOTSUPP)? true : false;

	/* If fast channel is supported, add the fast channel
	 * region in root domain as MMIO RW. And, get the doorbell
	 * information from the response */
	जर (fc_supported) {
#जर __riscv_xlen == 32
		fc_region_addr = fresp.region_addr_lo;
		fc_region_size = fresp.region_size_lo;
		fc_db_addr = fresp.db_addr_lo;
#अन्यथा
		fc_region_addr = (ulong)fresp.region_addr_hi << 32 |
							fresp.region_addr_lo;
		fc_region_size = (ulong)fresp.region_size_hi << 32 |
							fresp.region_size_lo;
		fc_db_addr = (ulong)fresp.db_addr_hi << 32 | fresp.db_addr_lo;

#endif
		rc = sbi_domain_मूळ_add_memrange(fc_region_addr,
					fc_region_size,
					RPMI_CPPC_HART_FASTCHAN_SIZE,
					(SBI_DOMAIN_MEMREGION_MMIO |
					SBI_DOMAIN_MEMREGION_M_READABLE |
					SBI_DOMAIN_MEMREGION_M_WRITABLE));
		जर (rc)
			परत rc;

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
	req.start_index = 0;
	करा {
		rc = rpmi_normal_request_with_status(
			chan, RPMI_CPPC_SRV_GET_HART_LIST,
			&req, rpmi_u32_मोजणी(req), rpmi_u32_मोजणी(req),
			&resp, rpmi_u32_मोजणी(resp), rpmi_u32_मोजणी(resp));
		जर (rc)
			परत rc;

		/* For each returned harts, get their fastchan offset and
		 * complete the initialization of per hart cppc structure */
		साठी (i = 0; i < resp.returned; i++) {
			cppc = rpmi_cppc_get_pointer(resp.hartid[i]);
			जर (!cppc)
				परत SBI_ENOSYS;

			cppc->prev_time_csr = 0;
			cppc->prev_delivered_counter = 0;
			cppc->chan = chan;
			cppc->mode = cppc_mode;
			cppc->fc_supported = fc_supported;

			जर (fc_supported) {
				hfreq.hart_id = resp.hartid[i];
				rc = rpmi_normal_request_with_status(
						chan,
						RPMI_CPPC_SRV_GET_FAST_CHANNEL_OFFSET,
						&hfreq,
						rpmi_u32_मोजणी(hfreq),
						rpmi_u32_मोजणी(hfreq),
						&hfresp,
						rpmi_u32_मोजणी(hfresp),
						rpmi_u32_मोजणी(hfresp));
				जर (rc)
					सुरूठेवा;

#जर __riscv_xlen == 32
				cppc->fc_perf_request_addr = fc_region_addr +
							hfresp.fc_perf_request_offset_lo;
				cppc->fc_perf_feedback_addr = fc_region_addr +
							hfresp.fc_perf_feedback_offset_lo;
#अन्यथा
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
			अन्यथा {
				cppc->fc_perf_request_addr = 0;
				cppc->fc_perf_feedback_addr = 0;
				cppc->fc_db_supported = 0;
				cppc->fc_db_addr = 0;
				cppc->fc_db_width = 0;
				cppc->fc_db_setmask = 0;
				cppc->fc_db_preservemask = 0;
			}
		}
		req.start_index += resp.returned;
	} पर्यंत (resp.remaining);

	परत 0;
}

स्थिर पूर्णांक rpmi_cppc_cold_init(स्थिरांक शून्य *fdt, पूर्णांक nodeoff,
			       स्थिरांक संरचना fdt_match *match)
{
	पूर्णांक rc;
	संरचना mbox_chan *chan;

	जर (!rpmi_cppc_offset) {
		rpmi_cppc_offset =
			sbi_scratch_alloc_type_offset(संरचना rpmi_cppc);
		जर (!rpmi_cppc_offset)
			परत SBI_ENOMEM;
	}

	/*
	 * If channel request failed then other end does not support
	 * CPPC service group so do nothing.
	 */
	rc = fdt_mailbox_request_chan(fdt, nodeoff, 0, &chan);
	जर (rc)
		परत SBI_ENODEV;

	/* Update per-HART scratch space */
	rc = rpmi_cppc_update_hart_scratch(chan);
	जर (rc)
		परत rc;

	sbi_cppc_set_device(&sbi_rpmi_cppc);

	परत 0;
}

स्थिर स्थिरांक संरचना fdt_match rpmi_cppc_match[] = {
	{ .compatible = "riscv,rpmi-cppc" },
	{},
};

स्थिरांक संरचना fdt_driver fdt_cppc_rpmi = {
	.match_table = rpmi_cppc_match,
	.init = rpmi_cppc_cold_init,
};
