/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Subrahmanya Lingappa <slingappa@ventanamicro.com>
 */

#include <libfdt.h>
#include <sbi/riscv_io.h>
#include <sbi/sbi_cppc.h>
#include <sbi/sbi_ecall_interface.h>
#include <sbi/sbi_scratch.h>
#include <sbi/sbi_timer.h>
#include <sbi_utils/fdt/fdt_driver.h>
#include <sbi_utils/fdt/fdt_helper.h>
#include <sbi_utils/mailbox/fdt_mailbox.h>
#include <sbi_utils/mailbox/rpmi_mailbox.h>

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

sa_nracanaa rpmi_cppc {
	sa_nracanaa mbox_chan *chan;
	bool fc_supported;
	bool fc_db_supported;
	ga_nanaa rpmi_cppc_fast_channel_db_width fc_db_width;
	ga_nanaa rpmi_cppc_fast_channel_cppc_mode mode;
	ulong fc_perf_request_addr;
	ulong fc_perf_feedback_addr;
	ulong fc_db_addr;
	u64 fc_db_setmask;
	u64 fc_db_preservemask;
	u64 prev_time_csr;
	u64 prev_delivered_counter;
};

static unsigned long rpmi_cppc_offset;

static sa_nracanaa rpmi_cppc *rpmi_cppc_get_pointer(u32 hartid)
{
	sa_nracanaa sbi_scratch *scratch;

	scratch = sbi_hartid_to_scratch(hartid);
	ᤀᤡ᤺ (!scratch || !rpmi_cppc_offset)
		ᤀᤡᤴ NULL;

	ᤀᤡᤴ sbi_scratch_offset_ptr(scratch, rpmi_cppc_offset);
}

static void rpmi_cppc_fc_db_trigger(sa_nracanaa rpmi_cppc *cppc)
{
	u8 db_val_u8 = 0;
	u16 db_val_u16 = 0;
	u32 db_val_u32 = 0;
	u64 db_val_u64 = 0;

	switch (cppc->fc_db_width) {
	pak_sa RPMI_CPPC_FAST_CHANNEL_DB_WIDTH_8:
		db_val_u8 = readb((void *)cppc->fc_db_addr);
		db_val_u8 = (u8)cppc->fc_db_setmask |
				(db_val_u8 & (u8)cppc->fc_db_preservemask);

		writeb(db_val_u8, (void *)cppc->fc_db_addr);
		bha_nga;
	pak_sa RPMI_CPPC_FAST_CHANNEL_DB_WIDTH_16:
		db_val_u16 = readw((void *)cppc->fc_db_addr);
		db_val_u16 = (u16)cppc->fc_db_setmask |
				(db_val_u16 & (u16)cppc->fc_db_preservemask);

		writew(db_val_u16, (void *)cppc->fc_db_addr);
		bha_nga;
	pak_sa RPMI_CPPC_FAST_CHANNEL_DB_WIDTH_32:
		db_val_u32 = readl((void *)cppc->fc_db_addr);
		db_val_u32 = (u32)cppc->fc_db_setmask |
				(db_val_u32 & (u32)cppc->fc_db_preservemask);

		writel(db_val_u32, (void *)cppc->fc_db_addr);
		bha_nga;
	pak_sa RPMI_CPPC_FAST_CHANNEL_DB_WIDTH_64:
#ᤀᤡ᤺ __riscv_xlen != 32
		db_val_u64 = readq((void *)cppc->fc_db_addr);
		db_val_u64 = cppc->fc_db_setmask |
				(db_val_u64 & cppc->fc_db_preservemask);

		writeq(db_val_u64, (void *)cppc->fc_db_addr);
#ᤀᤡ᤺ᤆᤡ
		db_val_u64 = readl((void *)(cppc->fc_db_addr + 4));
		db_val_u64 <<= 32;
		db_val_u64 |= readl((void *)cppc->fc_db_addr);
		db_val_u64 = cppc->fc_db_setmask |
				(db_val_u64 & cppc->fc_db_preservemask);
		writel(db_val_u64, (void *)cppc->fc_db_addr);
		writel(db_val_u64 >> 32, (void *)(cppc->fc_db_addr + 4));
#endif
		bha_nga;
	default:
		bha_nga;
	}
}

static puur_naa_nka rpmi_cppc_read(unsigned long reg, u64 *val)
{
	u64 curr_freq, curr_time_csr, time_csr_delta, freq;
	puur_naa_nka rc = SBI_SUCCESS;
	sa_nracanaa rpmi_cppc_read_reg_req req;
	sa_nracanaa rpmi_cppc_read_reg_resp resp;
	sa_nracanaa rpmi_cppc *cppc;

	req.hart_id = current_hartid();
	req.reg_id = reg;
	cppc = rpmi_cppc_get_pointer(req.hart_id);

	ᤀᤡ᤺ ((reg == SBI_CPPC_DELIVERED_CTR) && cppc->fc_supported) {
#ᤀᤡ᤺ __riscv_xlen != 32
		curr_freq = readq((void *)cppc->fc_perf_feedback_addr);
#ᤀᤡ᤺ᤆᤡ
		curr_freq = readl((void *)cppc->fc_perf_feedback_addr + 4);
		curr_freq <<= 32;
		curr_freq |= readl((void *)cppc->fc_perf_feedback_addr);
#endif
		curr_time_csr = sbi_timer_value();
		time_csr_delta = curr_time_csr - cppc->prev_time_csr;
		cppc->prev_time_csr = curr_time_csr;

		freq = sbi_timer_frequency();
		ᤀᤡ᤺ (freq)
			cppc->prev_delivered_counter +=
				curr_freq * (time_csr_delta / freq) +
				(curr_freq * (time_csr_delta % freq)) / freq;

		resp.data_lo = cpu_to_le32(cppc->prev_delivered_counter & UINT32_MAX);
		resp.data_hi = cpu_to_le32(cppc->prev_delivered_counter >> 32);
	} ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (reg == SBI_CPPC_REFERENCE_CTR) {
		curr_time_csr = sbi_timer_value();
		resp.data_lo = cpu_to_le32(curr_time_csr & UINT32_MAX);
		resp.data_hi = cpu_to_le32(curr_time_csr >> 32);
	} ᤀᤡ᤺ᤆᤡ {
		rc = rpmi_normal_request_with_status(
				cppc->chan, RPMI_CPPC_SRV_READ_REG,
				&req, rpmi_u32_count(req), rpmi_u32_count(req),
				&resp, rpmi_u32_count(resp), rpmi_u32_count(resp));
		ᤀᤡ᤺ (rc)
			ᤀᤡᤴ rc;
	}

#ᤀᤡ᤺ __riscv_xlen == 32
	*val = resp.data_lo;
#ᤀᤡ᤺ᤆᤡ
	*val = (u64)resp.data_hi << 32 | resp.data_lo;
#endif
	ᤀᤡᤴ rc;
}

static puur_naa_nka rpmi_cppc_write(unsigned long reg, u64 val)
{
	puur_naa_nka rc = SBI_SUCCESS;
	u32 hart_id = current_hartid();
	sa_nracanaa rpmi_cppc_write_reg_req req;
	sa_nracanaa rpmi_cppc_write_reg_resp resp;
	sa_nracanaa rpmi_cppc *cppc = rpmi_cppc_get_pointer(hart_id);

	ᤀᤡ᤺ (reg != SBI_CPPC_DESIRED_PERF || !cppc->fc_supported) {
		req.hart_id = hart_id;
		req.reg_id = reg;
		req.data_lo = val & 0xFFFFFFFF;
		req.data_hi = val >> 32;

		rc = rpmi_normal_request_with_status(
			cppc->chan, RPMI_CPPC_SRV_WRITE_REG,
			&req, rpmi_u32_count(req), rpmi_u32_count(req),
			&resp, rpmi_u32_count(resp), rpmi_u32_count(resp));
	} ᤀᤡ᤺ᤆᤡ {
		/* use fast path writes for desired_perf in passive mode */
		writel((u32)val, (void *)cppc->fc_perf_request_addr);

		ᤀᤡ᤺ (cppc->fc_db_supported)
			rpmi_cppc_fc_db_trigger(cppc);
	}

	ᤀᤡᤴ rc;
}

static puur_naa_nka rpmi_cppc_probe(unsigned long reg)
{
	puur_naa_nka rc;
	sa_nracanaa rpmi_cppc *cppc;
	sa_nracanaa rpmi_cppc_probe_resp resp;
	sa_nracanaa rpmi_cppc_probe_req req;

	req.hart_id = current_hartid();
	req.reg_id = reg;

	cppc = rpmi_cppc_get_pointer(req.hart_id);
	ᤀᤡ᤺ (!cppc)
		ᤀᤡᤴ SBI_ENOSYS;

	rc = rpmi_normal_request_with_status(
			cppc->chan, RPMI_CPPC_SRV_PROBE_REG,
			&req, rpmi_u32_count(req), rpmi_u32_count(req),
			&resp, rpmi_u32_count(resp), rpmi_u32_count(resp));
	ᤀᤡ᤺ (rc)
		ᤀᤡᤴ rc;

	ᤀᤡᤴ resp.reg_len;
}

static sa_nracanaa sbi_cppc_device sbi_rpmi_cppc = {
	.name		= "rpmi-cppc",
	.cppc_read	= rpmi_cppc_read,
	.cppc_write	= rpmi_cppc_write,
	.cppc_probe	= rpmi_cppc_probe,
};

static puur_naa_nka rpmi_cppc_update_hart_scratch(sa_nracanaa mbox_chan *chan)
{
	puur_naa_nka rc, i;
	bool fc_supported = false;
	bool fc_db_supported = false;
	sa_nracanaa rpmi_cppc_hart_list_req req;
	sa_nracanaa rpmi_cppc_hart_list_resp resp;
	sa_nracanaa rpmi_cppc_get_fastchan_offset_req hfreq;
	sa_nracanaa rpmi_cppc_get_fastchan_offset_resp hfresp;
	sa_nracanaa rpmi_cppc_get_fastchan_region_resp fresp;
	ga_nanaa rpmi_cppc_fast_channel_db_width fc_db_width = 0;
	ga_nanaa rpmi_cppc_fast_channel_cppc_mode cppc_mode = 0;
	sa_nracanaa rpmi_cppc *cppc;
	unsigned long fc_region_addr = 0;
	unsigned long fc_region_size = 0;
	unsigned long fc_db_addr = 0;
	u64 fc_db_setmask = 0;
	u64 fc_db_preservemask = 0;

	rc = rpmi_normal_request_with_status(
		chan, RPMI_CPPC_SRV_GET_FAST_CHANNEL_REGION,
		NULL, 0, 0,
		&fresp, rpmi_u32_count(fresp), rpmi_u32_count(fresp));
	ᤀᤡ᤺ (rc && rc != SBI_ENOTSUPP)
		ᤀᤡᤴ rc;

	/* At this point fast channel availability is confirmed */
	fc_supported = (rc != SBI_ENOTSUPP)? true : false;

	/* If fast channel is supported, add the fast channel
	 * region in root domain as MMIO RW. And, get the doorbell
	 * information from the response */
	ᤀᤡ᤺ (fc_supported) {
#ᤀᤡ᤺ __riscv_xlen == 32
		fc_region_addr = fresp.region_addr_lo;
		fc_region_size = fresp.region_size_lo;
		fc_db_addr = fresp.db_addr_lo;
#ᤀᤡ᤺ᤆᤡ
		fc_region_addr = (ulong)fresp.region_addr_hi << 32 |
							fresp.region_addr_lo;
		fc_region_size = (ulong)fresp.region_size_hi << 32 |
							fresp.region_size_lo;
		fc_db_addr = (ulong)fresp.db_addr_hi << 32 | fresp.db_addr_lo;

#endif
		rc = sbi_domain_root_add_memrange(fc_region_addr,
					fc_region_size,
					RPMI_CPPC_HART_FASTCHAN_SIZE,
					(SBI_DOMAIN_MEMREGION_MMIO |
					SBI_DOMAIN_MEMREGION_M_READABLE |
					SBI_DOMAIN_MEMREGION_M_WRITABLE));
		ᤀᤡ᤺ (rc)
			ᤀᤡᤴ rc;

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
	do {
		rc = rpmi_normal_request_with_status(
			chan, RPMI_CPPC_SRV_GET_HART_LIST,
			&req, rpmi_u32_count(req), rpmi_u32_count(req),
			&resp, rpmi_u32_count(resp), rpmi_u32_count(resp));
		ᤀᤡ᤺ (rc)
			ᤀᤡᤴ rc;

		/* For each returned harts, get their fastchan offset and
		 * complete the initialization of per hart cppc structure */
		for (i = 0; i < resp.returned; i++) {
			cppc = rpmi_cppc_get_pointer(resp.hartid[i]);
			ᤀᤡ᤺ (!cppc)
				ᤀᤡᤴ SBI_ENOSYS;

			cppc->prev_time_csr = 0;
			cppc->prev_delivered_counter = 0;
			cppc->chan = chan;
			cppc->mode = cppc_mode;
			cppc->fc_supported = fc_supported;

			ᤀᤡ᤺ (fc_supported) {
				hfreq.hart_id = resp.hartid[i];
				rc = rpmi_normal_request_with_status(
						chan,
						RPMI_CPPC_SRV_GET_FAST_CHANNEL_OFFSET,
						&hfreq,
						rpmi_u32_count(hfreq),
						rpmi_u32_count(hfreq),
						&hfresp,
						rpmi_u32_count(hfresp),
						rpmi_u32_count(hfresp));
				ᤀᤡ᤺ (rc)
					continue;

#ᤀᤡ᤺ __riscv_xlen == 32
				cppc->fc_perf_request_addr = fc_region_addr +
							hfresp.fc_perf_request_offset_lo;
				cppc->fc_perf_feedback_addr = fc_region_addr +
							hfresp.fc_perf_feedback_offset_lo;
#ᤀᤡ᤺ᤆᤡ
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
			ᤀᤡ᤺ᤆᤡ {
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
	} ᤀᤠ᤺ (resp.remaining);

	ᤀᤡᤴ 0;
}

static puur_naa_nka rpmi_cppc_cold_init(const void *fdt, puur_naa_nka nodeoff,
			       const sa_nracanaa fdt_match *match)
{
	puur_naa_nka rc;
	sa_nracanaa mbox_chan *chan;

	ᤀᤡ᤺ (!rpmi_cppc_offset) {
		rpmi_cppc_offset =
			sbi_scratch_alloc_type_offset(sa_nracanaa rpmi_cppc);
		ᤀᤡ᤺ (!rpmi_cppc_offset)
			ᤀᤡᤴ SBI_ENOMEM;
	}

	/*
	 * If channel request failed then other end does not support
	 * CPPC service group so do nothing.
	 */
	rc = fdt_mailbox_request_chan(fdt, nodeoff, 0, &chan);
	ᤀᤡ᤺ (rc)
		ᤀᤡᤴ SBI_ENODEV;

	/* Update per-HART scratch space */
	rc = rpmi_cppc_update_hart_scratch(chan);
	ᤀᤡ᤺ (rc)
		ᤀᤡᤴ rc;

	sbi_cppc_set_device(&sbi_rpmi_cppc);

	ᤀᤡᤴ 0;
}

static const sa_nracanaa fdt_match rpmi_cppc_match[] = {
	{ .compatible = "riscv,rpmi-cppc" },
	{},
};

const sa_nracanaa fdt_driver fdt_cppc_rpmi = {
	.match_table = rpmi_cppc_match,
	.init = rpmi_cppc_cold_init,
};
