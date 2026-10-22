/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Subrahmanya Lingappa <slingappa@ventanamicro.com>
 */

#समावेश <libfdt.h>
#समावेश <sbi/riscv_io.h>
#समावेश <sbi/sbi_cppc.h>
#समावेश <sbi/sbi_ecall_interface.h>
#समावेश <sbi/sbi_scratch.h>
#समावेश <sbi/sbi_timer.h>
#समावेश <sbi_utils/fdt/fdt_driver.h>
#समावेश <sbi_utils/fdt/fdt_helper.h>
#समावेश <sbi_utils/mailbox/fdt_mailbox.h>
#समावेश <sbi_utils/mailbox/rpmi_mailbox.h>

/**
 * Per hart RPMI CPPC fast channel size (bytes)
 * PASSIVE MODE:
 *	0x0: DESIRED_PERFORMANCE (4-byte)
 *	0x4: __RESERVED	(4-byte)
 * ACTIVE MODE: (not supported yet)
 *	0x0: MINIMUM PERFORMANCE (4-byte)
 *	0x4: MAXIMUM PERFORMANCE (4-byte)
 */
#घोषणा RPMI_CPPC_HART_FASTCHAN_आकार		0x8

काष्ठा rpmi_cppc {
	काष्ठा mbox_chan *chan;
	bool fc_supported;
	bool fc_db_supported;
	क्रमागत rpmi_cppc_fast_channel_db_width fc_db_width;
	क्रमागत rpmi_cppc_fast_channel_cppc_mode mode;
	ulong fc_perf_request_addr;
	ulong fc_perf_feedback_addr;
	ulong fc_db_addr;
	u64 fc_db_setmask;
	u64 fc_db_preservemask;
	u64 पूर्व_time_csr;
	u64 पूर्व_delivered_गणक;
};

अटल अचिन्हित दीर्घ rpmi_cppc_offset;

अटल काष्ठा rpmi_cppc *rpmi_cppc_get_pointer(u32 hartid)
{
	काष्ठा sbi_scratch *scratch;

	scratch = sbi_hartid_to_scratch(hartid);
	अगर (!scratch || !rpmi_cppc_offset)
		वापस शून्य;

	वापस sbi_scratch_offset_ptr(scratch, rpmi_cppc_offset);
}

अटल व्योम rpmi_cppc_fc_db_trigger(काष्ठा rpmi_cppc *cppc)
{
	u8 db_val_u8 = 0;
	u16 db_val_u16 = 0;
	u32 db_val_u32 = 0;
	u64 db_val_u64 = 0;

	चयन (cppc->fc_db_width) {
	हाल RPMI_CPPC_FAST_CHANNEL_DB_WIDTH_8:
		db_val_u8 = readb((व्योम *)cppc->fc_db_addr);
		db_val_u8 = (u8)cppc->fc_db_setmask |
				(db_val_u8 & (u8)cppc->fc_db_preservemask);

		writeb(db_val_u8, (व्योम *)cppc->fc_db_addr);
		अवरोध;
	हाल RPMI_CPPC_FAST_CHANNEL_DB_WIDTH_16:
		db_val_u16 = readw((व्योम *)cppc->fc_db_addr);
		db_val_u16 = (u16)cppc->fc_db_setmask |
				(db_val_u16 & (u16)cppc->fc_db_preservemask);

		writew(db_val_u16, (व्योम *)cppc->fc_db_addr);
		अवरोध;
	हाल RPMI_CPPC_FAST_CHANNEL_DB_WIDTH_32:
		db_val_u32 = readl((व्योम *)cppc->fc_db_addr);
		db_val_u32 = (u32)cppc->fc_db_setmask |
				(db_val_u32 & (u32)cppc->fc_db_preservemask);

		writel(db_val_u32, (व्योम *)cppc->fc_db_addr);
		अवरोध;
	हाल RPMI_CPPC_FAST_CHANNEL_DB_WIDTH_64:
#अगर __riscv_xlen != 32
		db_val_u64 = readq((व्योम *)cppc->fc_db_addr);
		db_val_u64 = cppc->fc_db_setmask |
				(db_val_u64 & cppc->fc_db_preservemask);

		writeq(db_val_u64, (व्योम *)cppc->fc_db_addr);
#अन्यथा
		db_val_u64 = readl((व्योम *)(cppc->fc_db_addr + 4));
		db_val_u64 <<= 32;
		db_val_u64 |= readl((व्योम *)cppc->fc_db_addr);
		db_val_u64 = cppc->fc_db_setmask |
				(db_val_u64 & cppc->fc_db_preservemask);
		writel(db_val_u64, (व्योम *)cppc->fc_db_addr);
		writel(db_val_u64 >> 32, (व्योम *)(cppc->fc_db_addr + 4));
#पूर्ण_अगर
		अवरोध;
	शेष:
		अवरोध;
	}
}

अटल पूर्णांक rpmi_cppc_read(अचिन्हित दीर्घ reg, u64 *val)
{
	u64 curr_freq, curr_time_csr, time_csr_delta, freq;
	पूर्णांक rc = SBI_SUCCESS;
	काष्ठा rpmi_cppc_read_reg_req req;
	काष्ठा rpmi_cppc_read_reg_resp resp;
	काष्ठा rpmi_cppc *cppc;

	req.hart_id = current_hartid();
	req.reg_id = reg;
	cppc = rpmi_cppc_get_pointer(req.hart_id);

	अगर ((reg == SBI_CPPC_DELIVERED_CTR) && cppc->fc_supported) {
#अगर __riscv_xlen != 32
		curr_freq = readq((व्योम *)cppc->fc_perf_feedback_addr);
#अन्यथा
		curr_freq = readl((व्योम *)cppc->fc_perf_feedback_addr + 4);
		curr_freq <<= 32;
		curr_freq |= readl((व्योम *)cppc->fc_perf_feedback_addr);
#पूर्ण_अगर
		curr_time_csr = sbi_timer_मूल्य();
		time_csr_delta = curr_time_csr - cppc->पूर्व_time_csr;
		cppc->पूर्व_time_csr = curr_time_csr;

		freq = sbi_timer_frequency();
		अगर (freq)
			cppc->पूर्व_delivered_गणक +=
				curr_freq * (time_csr_delta / freq) +
				(curr_freq * (time_csr_delta % freq)) / freq;

		resp.तथ्य_lo = cpu_to_le32(cppc->पूर्व_delivered_गणक & UINT32_MAX);
		resp.तथ्य_hi = cpu_to_le32(cppc->पूर्व_delivered_गणक >> 32);
	} अन्यथा अगर (reg == SBI_CPPC_REFERENCE_CTR) {
		curr_time_csr = sbi_timer_मूल्य();
		resp.तथ्य_lo = cpu_to_le32(curr_time_csr & UINT32_MAX);
		resp.तथ्य_hi = cpu_to_le32(curr_time_csr >> 32);
	} अन्यथा {
		rc = rpmi_normal_request_with_status(
				cppc->chan, RPMI_CPPC_SRV_READ_REG,
				&req, rpmi_u32_गिनती(req), rpmi_u32_गिनती(req),
				&resp, rpmi_u32_गिनती(resp), rpmi_u32_गिनती(resp));
		अगर (rc)
			वापस rc;
	}

#अगर __riscv_xlen == 32
	*val = resp.तथ्य_lo;
#अन्यथा
	*val = (u64)resp.तथ्य_hi << 32 | resp.तथ्य_lo;
#पूर्ण_अगर
	वापस rc;
}

अटल पूर्णांक rpmi_cppc_write(अचिन्हित दीर्घ reg, u64 val)
{
	पूर्णांक rc = SBI_SUCCESS;
	u32 hart_id = current_hartid();
	काष्ठा rpmi_cppc_write_reg_req req;
	काष्ठा rpmi_cppc_write_reg_resp resp;
	काष्ठा rpmi_cppc *cppc = rpmi_cppc_get_pointer(hart_id);

	अगर (reg != SBI_CPPC_DESIRED_PERF || !cppc->fc_supported) {
		req.hart_id = hart_id;
		req.reg_id = reg;
		req.तथ्य_lo = val & 0xFFFFFFFF;
		req.तथ्य_hi = val >> 32;

		rc = rpmi_normal_request_with_status(
			cppc->chan, RPMI_CPPC_SRV_WRITE_REG,
			&req, rpmi_u32_गिनती(req), rpmi_u32_गिनती(req),
			&resp, rpmi_u32_गिनती(resp), rpmi_u32_गिनती(resp));
	} अन्यथा {
		/* use fast path writes for desired_perf in passive mode */
		writel((u32)val, (व्योम *)cppc->fc_perf_request_addr);

		अगर (cppc->fc_db_supported)
			rpmi_cppc_fc_db_trigger(cppc);
	}

	वापस rc;
}

अटल पूर्णांक rpmi_cppc_probe(अचिन्हित दीर्घ reg)
{
	पूर्णांक rc;
	काष्ठा rpmi_cppc *cppc;
	काष्ठा rpmi_cppc_probe_resp resp;
	काष्ठा rpmi_cppc_probe_req req;

	req.hart_id = current_hartid();
	req.reg_id = reg;

	cppc = rpmi_cppc_get_pointer(req.hart_id);
	अगर (!cppc)
		वापस SBI_ENOSYS;

	rc = rpmi_normal_request_with_status(
			cppc->chan, RPMI_CPPC_SRV_PROBE_REG,
			&req, rpmi_u32_गिनती(req), rpmi_u32_गिनती(req),
			&resp, rpmi_u32_गिनती(resp), rpmi_u32_गिनती(resp));
	अगर (rc)
		वापस rc;

	वापस resp.reg_len;
}

अटल काष्ठा sbi_cppc_device sbi_rpmi_cppc = {
	.name		= "rpmi-cppc",
	.cppc_read	= rpmi_cppc_read,
	.cppc_write	= rpmi_cppc_write,
	.cppc_probe	= rpmi_cppc_probe,
};

अटल पूर्णांक rpmi_cppc_update_hart_scratch(काष्ठा mbox_chan *chan)
{
	पूर्णांक rc, i;
	bool fc_supported = false;
	bool fc_db_supported = false;
	काष्ठा rpmi_cppc_hart_list_req req;
	काष्ठा rpmi_cppc_hart_list_resp resp;
	काष्ठा rpmi_cppc_get_fastchan_offset_req hfreq;
	काष्ठा rpmi_cppc_get_fastchan_offset_resp hfresp;
	काष्ठा rpmi_cppc_get_fastchan_region_resp fresp;
	क्रमागत rpmi_cppc_fast_channel_db_width fc_db_width = 0;
	क्रमागत rpmi_cppc_fast_channel_cppc_mode cppc_mode = 0;
	काष्ठा rpmi_cppc *cppc;
	अचिन्हित दीर्घ fc_region_addr = 0;
	अचिन्हित दीर्घ fc_region_आकार = 0;
	अचिन्हित दीर्घ fc_db_addr = 0;
	u64 fc_db_setmask = 0;
	u64 fc_db_preservemask = 0;

	rc = rpmi_normal_request_with_status(
		chan, RPMI_CPPC_SRV_GET_FAST_CHANNEL_REGION,
		शून्य, 0, 0,
		&fresp, rpmi_u32_गिनती(fresp), rpmi_u32_गिनती(fresp));
	अगर (rc && rc != SBI_ENOTSUPP)
		वापस rc;

	/* At this point fast channel availability is confirmed */
	fc_supported = (rc != SBI_ENOTSUPP)? true : false;

	/* If fast channel is supported, add the fast channel
	 * region in root domain as MMIO RW. And, get the doorbell
	 * information from the response */
	अगर (fc_supported) {
#अगर __riscv_xlen == 32
		fc_region_addr = fresp.region_addr_lo;
		fc_region_आकार = fresp.region_आकार_lo;
		fc_db_addr = fresp.db_addr_lo;
#अन्यथा
		fc_region_addr = (ulong)fresp.region_addr_hi << 32 |
							fresp.region_addr_lo;
		fc_region_आकार = (ulong)fresp.region_आकार_hi << 32 |
							fresp.region_आकार_lo;
		fc_db_addr = (ulong)fresp.db_addr_hi << 32 | fresp.db_addr_lo;

#पूर्ण_अगर
		rc = sbi_domain_मूल_add_memrange(fc_region_addr,
					fc_region_आकार,
					RPMI_CPPC_HART_FASTCHAN_आकार,
					(SBI_DOMAIN_MEMREGION_MMIO |
					SBI_DOMAIN_MEMREGION_M_READABLE |
					SBI_DOMAIN_MEMREGION_M_WRITABLE));
		अगर (rc)
			वापस rc;

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
	req.start_सूचकांक = 0;
	करो {
		rc = rpmi_normal_request_with_status(
			chan, RPMI_CPPC_SRV_GET_HART_LIST,
			&req, rpmi_u32_गिनती(req), rpmi_u32_गिनती(req),
			&resp, rpmi_u32_गिनती(resp), rpmi_u32_गिनती(resp));
		अगर (rc)
			वापस rc;

		/* For each returned harts, get their fastchan offset and
		 * complete the initialization of per hart cppc structure */
		क्रम (i = 0; i < resp.returned; i++) {
			cppc = rpmi_cppc_get_pointer(resp.hartid[i]);
			अगर (!cppc)
				वापस SBI_ENOSYS;

			cppc->पूर्व_time_csr = 0;
			cppc->पूर्व_delivered_गणक = 0;
			cppc->chan = chan;
			cppc->mode = cppc_mode;
			cppc->fc_supported = fc_supported;

			अगर (fc_supported) {
				hfreq.hart_id = resp.hartid[i];
				rc = rpmi_normal_request_with_status(
						chan,
						RPMI_CPPC_SRV_GET_FAST_CHANNEL_OFFSET,
						&hfreq,
						rpmi_u32_गिनती(hfreq),
						rpmi_u32_गिनती(hfreq),
						&hfresp,
						rpmi_u32_गिनती(hfresp),
						rpmi_u32_गिनती(hfresp));
				अगर (rc)
					जारी;

#अगर __riscv_xlen == 32
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
#पूर्ण_अगर
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
		req.start_सूचकांक += resp.returned;
	} जबतक (resp.remaining);

	वापस 0;
}

अटल पूर्णांक rpmi_cppc_cold_init(स्थिर व्योम *fdt, पूर्णांक nodeoff,
			       स्थिर काष्ठा fdt_match *match)
{
	पूर्णांक rc;
	काष्ठा mbox_chan *chan;

	अगर (!rpmi_cppc_offset) {
		rpmi_cppc_offset =
			sbi_scratch_alloc_type_offset(काष्ठा rpmi_cppc);
		अगर (!rpmi_cppc_offset)
			वापस SBI_ENOMEM;
	}

	/*
	 * If channel request failed then other end does not support
	 * CPPC service group so do nothing.
	 */
	rc = fdt_mailbox_request_chan(fdt, nodeoff, 0, &chan);
	अगर (rc)
		वापस SBI_ENODEV;

	/* Update per-HART scratch space */
	rc = rpmi_cppc_update_hart_scratch(chan);
	अगर (rc)
		वापस rc;

	sbi_cppc_set_device(&sbi_rpmi_cppc);

	वापस 0;
}

अटल स्थिर काष्ठा fdt_match rpmi_cppc_match[] = {
	{ .compatible = "riscv,rpmi-cppc" },
	{},
};

स्थिर काष्ठा fdt_driver fdt_cppc_rpmi = {
	.match_table = rpmi_cppc_match,
	.init = rpmi_cppc_cold_init,
};
