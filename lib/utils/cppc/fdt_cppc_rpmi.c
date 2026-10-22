/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Subrahmanya Lingappa <slingappa@ventanamicro.com>
 */

#ಅನ್ತರ್ಭಾವ <libfdt.h>
#ಅನ್ತರ್ಭಾವ <sbi/riscv_io.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_cppc.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_ecall_interface.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_scratch.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_timer.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/fdt/fdt_driver.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/fdt/fdt_helper.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/mailbox/fdt_mailbox.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/mailbox/rpmi_mailbox.h>

/**
 * Per hart RPMI CPPC fast channel size (bytes)
 * PASSIVE MODE:
 *	0x0: DESIRED_PERFORMANCE (4-byte)
 *	0x4: __RESERVED	(4-byte)
 * ACTIVE MODE: (not supported yet)
 *	0x0: MINIMUM PERFORMANCE (4-byte)
 *	0x4: MAXIMUM PERFORMANCE (4-byte)
 */
#ವ್ಯಾಖ್ಯಾನಿಸು RPMI_CPPC_HART_FASTCHAN_ಗಾತ್ರ		0x8

ರಚನೆ rpmi_cppc {
	ರಚನೆ mbox_chan *chan;
	bool fc_supported;
	bool fc_db_supported;
	ಎಣಿಕೆ rpmi_cppc_fast_channel_db_ಅಗಲ fc_db_ಅಗಲ;
	ಎಣಿಕೆ rpmi_cppc_fast_channel_cppc_ಬಹುಲಕ ಬಹುಲಕ;
	ulong fc_perf_request_addr;
	ulong fc_perf_feedback_addr;
	ulong fc_db_addr;
	u64 fc_db_setmask;
	u64 fc_db_preservemask;
	u64 prev_ಸಮಯ_csr;
	u64 prev_delivered_counter;
};

ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ rpmi_cppc_ಸ್ಥಾನಾಂತರ;

ಸ್ಥಿರ ರಚನೆ rpmi_cppc *rpmi_cppc_get_ಸೂಚಕ(u32 hartid)
{
	ರಚನೆ sbi_scratch *scratch;

	scratch = sbi_hartid_ಗಮ್ಯ_scratch(hartid);
	ಆದರೆ (!scratch || !rpmi_cppc_ಸ್ಥಾನಾಂತರ)
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;

	ಹಿಂತಿರುಗಿ sbi_scratch_ಸ್ಥಾನಾಂತರ_ptr(scratch, rpmi_cppc_ಸ್ಥಾನಾಂತರ);
}

ಸ್ಥಿರ ಶೂನ್ಯ rpmi_cppc_fc_db_trigger(ರಚನೆ rpmi_cppc *cppc)
{
	u8 db_val_u8 = 0;
	u16 db_val_u16 = 0;
	u32 db_val_u32 = 0;
	u64 db_val_u64 = 0;

	ಆಯ್ಕೆ (cppc->fc_db_ಅಗಲ) {
	ಸ್ಥಿತಿ RPMI_CPPC_FAST_CHANNEL_DB_ಅಗಲ_8:
		db_val_u8 = readb((ಶೂನ್ಯ *)cppc->fc_db_addr);
		db_val_u8 = (u8)cppc->fc_db_setmask |
				(db_val_u8 & (u8)cppc->fc_db_preservemask);

		writeb(db_val_u8, (ಶೂನ್ಯ *)cppc->fc_db_addr);
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ RPMI_CPPC_FAST_CHANNEL_DB_ಅಗಲ_16:
		db_val_u16 = readw((ಶೂನ್ಯ *)cppc->fc_db_addr);
		db_val_u16 = (u16)cppc->fc_db_setmask |
				(db_val_u16 & (u16)cppc->fc_db_preservemask);

		writew(db_val_u16, (ಶೂನ್ಯ *)cppc->fc_db_addr);
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ RPMI_CPPC_FAST_CHANNEL_DB_ಅಗಲ_32:
		db_val_u32 = readl((ಶೂನ್ಯ *)cppc->fc_db_addr);
		db_val_u32 = (u32)cppc->fc_db_setmask |
				(db_val_u32 & (u32)cppc->fc_db_preservemask);

		writel(db_val_u32, (ಶೂನ್ಯ *)cppc->fc_db_addr);
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ RPMI_CPPC_FAST_CHANNEL_DB_ಅಗಲ_64:
#ಆದರೆ __riscv_xlen != 32
		db_val_u64 = readq((ಶೂನ್ಯ *)cppc->fc_db_addr);
		db_val_u64 = cppc->fc_db_setmask |
				(db_val_u64 & cppc->fc_db_preservemask);

		writeq(db_val_u64, (ಶೂನ್ಯ *)cppc->fc_db_addr);
#ಇಲ್ಲದಿದ್ದರೆ
		db_val_u64 = readl((ಶೂನ್ಯ *)(cppc->fc_db_addr + 4));
		db_val_u64 <<= 32;
		db_val_u64 |= readl((ಶೂನ್ಯ *)cppc->fc_db_addr);
		db_val_u64 = cppc->fc_db_setmask |
				(db_val_u64 & cppc->fc_db_preservemask);
		writel(db_val_u64, (ಶೂನ್ಯ *)cppc->fc_db_addr);
		writel(db_val_u64 >> 32, (ಶೂನ್ಯ *)(cppc->fc_db_addr + 4));
#ಆದರೆ_ಅಂತ್ಯ
		ನಿಲ್ಲಿಸು;
	ಪೂರ್ವನಿಯೋಜಿತ:
		ನಿಲ್ಲಿಸು;
	}
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ rpmi_cppc_read(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ reg, u64 *val)
{
	u64 curr_freq, curr_ಸಮಯ_csr, ಸಮಯ_csr_delta, freq;
	ಪೂರ್ಣಾಂಕ rc = SBI_SUCCESS;
	ರಚನೆ rpmi_cppc_read_reg_req req;
	ರಚನೆ rpmi_cppc_read_reg_resp resp;
	ರಚನೆ rpmi_cppc *cppc;

	req.hart_id = current_hartid();
	req.reg_id = reg;
	cppc = rpmi_cppc_get_ಸೂಚಕ(req.hart_id);

	ಆದರೆ ((reg == SBI_CPPC_DELIVERED_CTR) && cppc->fc_supported) {
#ಆದರೆ __riscv_xlen != 32
		curr_freq = readq((ಶೂನ್ಯ *)cppc->fc_perf_feedback_addr);
#ಇಲ್ಲದಿದ್ದರೆ
		curr_freq = readl((ಶೂನ್ಯ *)cppc->fc_perf_feedback_addr + 4);
		curr_freq <<= 32;
		curr_freq |= readl((ಶೂನ್ಯ *)cppc->fc_perf_feedback_addr);
#ಆದರೆ_ಅಂತ್ಯ
		curr_ಸಮಯ_csr = sbi_timer_ಮೌಲ್ಯ();
		ಸಮಯ_csr_delta = curr_ಸಮಯ_csr - cppc->prev_ಸಮಯ_csr;
		cppc->prev_ಸಮಯ_csr = curr_ಸಮಯ_csr;

		freq = sbi_timer_frequency();
		ಆದರೆ (freq)
			cppc->prev_delivered_counter +=
				curr_freq * (ಸಮಯ_csr_delta / freq) +
				(curr_freq * (ಸಮಯ_csr_delta % freq)) / freq;

		resp.ದತ್ತಾಂಶ_lo = cpu_ಗಮ್ಯ_le32(cppc->prev_delivered_counter & UINT32_MAX);
		resp.ದತ್ತಾಂಶ_hi = cpu_ಗಮ್ಯ_le32(cppc->prev_delivered_counter >> 32);
	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (reg == SBI_CPPC_REFERENCE_CTR) {
		curr_ಸಮಯ_csr = sbi_timer_ಮೌಲ್ಯ();
		resp.ದತ್ತಾಂಶ_lo = cpu_ಗಮ್ಯ_le32(curr_ಸಮಯ_csr & UINT32_MAX);
		resp.ದತ್ತಾಂಶ_hi = cpu_ಗಮ್ಯ_le32(curr_ಸಮಯ_csr >> 32);
	} ಇಲ್ಲದಿದ್ದರೆ {
		rc = rpmi_ಸಾಮಾನ್ಯ_request_with_ಸ್ಥಿತಿ(
				cppc->chan, RPMI_CPPC_SRV_READ_REG,
				&req, rpmi_u32_ಎಣಿಕೆ(req), rpmi_u32_ಎಣಿಕೆ(req),
				&resp, rpmi_u32_ಎಣಿಕೆ(resp), rpmi_u32_ಎಣಿಕೆ(resp));
		ಆದರೆ (rc)
			ಹಿಂತಿರುಗಿ rc;
	}

#ಆದರೆ __riscv_xlen == 32
	*val = resp.ದತ್ತಾಂಶ_lo;
#ಇಲ್ಲದಿದ್ದರೆ
	*val = (u64)resp.ದತ್ತಾಂಶ_hi << 32 | resp.ದತ್ತಾಂಶ_lo;
#ಆದರೆ_ಅಂತ್ಯ
	ಹಿಂತಿರುಗಿ rc;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ rpmi_cppc_write(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ reg, u64 val)
{
	ಪೂರ್ಣಾಂಕ rc = SBI_SUCCESS;
	u32 hart_id = current_hartid();
	ರಚನೆ rpmi_cppc_write_reg_req req;
	ರಚನೆ rpmi_cppc_write_reg_resp resp;
	ರಚನೆ rpmi_cppc *cppc = rpmi_cppc_get_ಸೂಚಕ(hart_id);

	ಆದರೆ (reg != SBI_CPPC_DESIRED_PERF || !cppc->fc_supported) {
		req.hart_id = hart_id;
		req.reg_id = reg;
		req.ದತ್ತಾಂಶ_lo = val & 0xFFFFFFFF;
		req.ದತ್ತಾಂಶ_hi = val >> 32;

		rc = rpmi_ಸಾಮಾನ್ಯ_request_with_ಸ್ಥಿತಿ(
			cppc->chan, RPMI_CPPC_SRV_WRITE_REG,
			&req, rpmi_u32_ಎಣಿಕೆ(req), rpmi_u32_ಎಣಿಕೆ(req),
			&resp, rpmi_u32_ಎಣಿಕೆ(resp), rpmi_u32_ಎಣಿಕೆ(resp));
	} ಇಲ್ಲದಿದ್ದರೆ {
		/* use fast path writes for desired_perf in passive mode */
		writel((u32)val, (ಶೂನ್ಯ *)cppc->fc_perf_request_addr);

		ಆದರೆ (cppc->fc_db_supported)
			rpmi_cppc_fc_db_trigger(cppc);
	}

	ಹಿಂತಿರುಗಿ rc;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ rpmi_cppc_probe(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ reg)
{
	ಪೂರ್ಣಾಂಕ rc;
	ರಚನೆ rpmi_cppc *cppc;
	ರಚನೆ rpmi_cppc_probe_resp resp;
	ರಚನೆ rpmi_cppc_probe_req req;

	req.hart_id = current_hartid();
	req.reg_id = reg;

	cppc = rpmi_cppc_get_ಸೂಚಕ(req.hart_id);
	ಆದರೆ (!cppc)
		ಹಿಂತಿರುಗಿ SBI_ENOSYS;

	rc = rpmi_ಸಾಮಾನ್ಯ_request_with_ಸ್ಥಿತಿ(
			cppc->chan, RPMI_CPPC_SRV_PROBE_REG,
			&req, rpmi_u32_ಎಣಿಕೆ(req), rpmi_u32_ಎಣಿಕೆ(req),
			&resp, rpmi_u32_ಎಣಿಕೆ(resp), rpmi_u32_ಎಣಿಕೆ(resp));
	ಆದರೆ (rc)
		ಹಿಂತಿರುಗಿ rc;

	ಹಿಂತಿರುಗಿ resp.reg_len;
}

ಸ್ಥಿರ ರಚನೆ sbi_cppc_device sbi_rpmi_cppc = {
	.name		= "rpmi-cppc",
	.cppc_read	= rpmi_cppc_read,
	.cppc_write	= rpmi_cppc_write,
	.cppc_probe	= rpmi_cppc_probe,
};

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ rpmi_cppc_update_hart_scratch(ರಚನೆ mbox_chan *chan)
{
	ಪೂರ್ಣಾಂಕ rc, i;
	bool fc_supported = false;
	bool fc_db_supported = false;
	ರಚನೆ rpmi_cppc_hart_list_req req;
	ರಚನೆ rpmi_cppc_hart_list_resp resp;
	ರಚನೆ rpmi_cppc_get_fastchan_ಸ್ಥಾನಾಂತರ_req hfreq;
	ರಚನೆ rpmi_cppc_get_fastchan_ಸ್ಥಾನಾಂತರ_resp hfresp;
	ರಚನೆ rpmi_cppc_get_fastchan_region_resp fresp;
	ಎಣಿಕೆ rpmi_cppc_fast_channel_db_ಅಗಲ fc_db_ಅಗಲ = 0;
	ಎಣಿಕೆ rpmi_cppc_fast_channel_cppc_ಬಹುಲಕ cppc_ಬಹುಲಕ = 0;
	ರಚನೆ rpmi_cppc *cppc;
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ fc_region_addr = 0;
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ fc_region_ಗಾತ್ರ = 0;
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ fc_db_addr = 0;
	u64 fc_db_setmask = 0;
	u64 fc_db_preservemask = 0;

	rc = rpmi_ಸಾಮಾನ್ಯ_request_with_ಸ್ಥಿತಿ(
		chan, RPMI_CPPC_SRV_GET_FAST_CHANNEL_REGION,
		ಖಾಲಿ, 0, 0,
		&fresp, rpmi_u32_ಎಣಿಕೆ(fresp), rpmi_u32_ಎಣಿಕೆ(fresp));
	ಆದರೆ (rc && rc != SBI_ENOTSUPP)
		ಹಿಂತಿರುಗಿ rc;

	/* At this point fast channel availability is confirmed */
	fc_supported = (rc != SBI_ENOTSUPP)? true : false;

	/* If fast channel is supported, add the fast channel
	 * region in root domain as MMIO RW. And, get the doorbell
	 * information from the response */
	ಆದರೆ (fc_supported) {
#ಆದರೆ __riscv_xlen == 32
		fc_region_addr = fresp.region_addr_lo;
		fc_region_ಗಾತ್ರ = fresp.region_ಗಾತ್ರ_lo;
		fc_db_addr = fresp.db_addr_lo;
#ಇಲ್ಲದಿದ್ದರೆ
		fc_region_addr = (ulong)fresp.region_addr_hi << 32 |
							fresp.region_addr_lo;
		fc_region_ಗಾತ್ರ = (ulong)fresp.region_ಗಾತ್ರ_hi << 32 |
							fresp.region_ಗಾತ್ರ_lo;
		fc_db_addr = (ulong)fresp.db_addr_hi << 32 | fresp.db_addr_lo;

#ಆದರೆ_ಅಂತ್ಯ
		rc = sbi_domain_ಬೇರು_ಕೂಡಿಸು_memrange(fc_region_addr,
					fc_region_ಗಾತ್ರ,
					RPMI_CPPC_HART_FASTCHAN_ಗಾತ್ರ,
					(SBI_DOMAIN_MEMREGION_MMIO |
					SBI_DOMAIN_MEMREGION_M_READABLE |
					SBI_DOMAIN_MEMREGION_M_WRITABLE));
		ಆದರೆ (rc)
			ಹಿಂತಿರುಗಿ rc;

		cppc_ಬಹುಲಕ = (fresp.flags & RPMI_CPPC_FAST_CHANNEL_CPPC_ಬಹುಲಕ_ಮುಖವಾಡ) >>
					RPMI_CPPC_FAST_CHANNEL_CPPC_ಬಹುಲಕ_POS;
		fc_db_supported = fresp.flags &
				RPMI_CPPC_FAST_CHANNEL_FLAGS_DB_SUPPORTED;
		fc_db_ಅಗಲ = (fresp.flags &
				RPMI_CPPC_FAST_CHANNEL_FLAGS_DB_ಅಗಲ_ಮುಖವಾಡ) >>
				RPMI_CPPC_FAST_CHANNEL_FLAGS_DB_ಅಗಲ_POS;
		fc_db_setmask = (u64)fresp.db_setmask_hi << 32 |
						fresp.db_setmask_lo;
		fc_db_preservemask = (u64)fresp.db_preservemask_hi << 32 |
						fresp.db_preservemask_lo;
	}

	/* Get the hart list and depending on the fast channel support
	 * initialize the per hart cppc structure */
	req.ಆರಂಭ_ಸೂಚ್ಯಂಕ = 0;
	ಮಾಡು {
		rc = rpmi_ಸಾಮಾನ್ಯ_request_with_ಸ್ಥಿತಿ(
			chan, RPMI_CPPC_SRV_GET_HART_LIST,
			&req, rpmi_u32_ಎಣಿಕೆ(req), rpmi_u32_ಎಣಿಕೆ(req),
			&resp, rpmi_u32_ಎಣಿಕೆ(resp), rpmi_u32_ಎಣಿಕೆ(resp));
		ಆದರೆ (rc)
			ಹಿಂತಿರುಗಿ rc;

		/* For each returned harts, get their fastchan offset and
		 * complete the initialization of per hart cppc structure */
		ಗಾಗಿ (i = 0; i < resp.returned; i++) {
			cppc = rpmi_cppc_get_ಸೂಚಕ(resp.hartid[i]);
			ಆದರೆ (!cppc)
				ಹಿಂತಿರುಗಿ SBI_ENOSYS;

			cppc->prev_ಸಮಯ_csr = 0;
			cppc->prev_delivered_counter = 0;
			cppc->chan = chan;
			cppc->ಬಹುಲಕ = cppc_ಬಹುಲಕ;
			cppc->fc_supported = fc_supported;

			ಆದರೆ (fc_supported) {
				hfreq.hart_id = resp.hartid[i];
				rc = rpmi_ಸಾಮಾನ್ಯ_request_with_ಸ್ಥಿತಿ(
						chan,
						RPMI_CPPC_SRV_GET_FAST_CHANNEL_ಸ್ಥಾನಾಂತರ,
						&hfreq,
						rpmi_u32_ಎಣಿಕೆ(hfreq),
						rpmi_u32_ಎಣಿಕೆ(hfreq),
						&hfresp,
						rpmi_u32_ಎಣಿಕೆ(hfresp),
						rpmi_u32_ಎಣಿಕೆ(hfresp));
				ಆದರೆ (rc)
					ಮುಂದುವರಿ;

#ಆದರೆ __riscv_xlen == 32
				cppc->fc_perf_request_addr = fc_region_addr +
							hfresp.fc_perf_request_ಸ್ಥಾನಾಂತರ_lo;
				cppc->fc_perf_feedback_addr = fc_region_addr +
							hfresp.fc_perf_feedback_ಸ್ಥಾನಾಂತರ_lo;
#ಇಲ್ಲದಿದ್ದರೆ
				cppc->fc_perf_request_addr = fc_region_addr +
					((ulong)hfresp.fc_perf_request_ಸ್ಥಾನಾಂತರ_hi << 32 |
					hfresp.fc_perf_request_ಸ್ಥಾನಾಂತರ_lo);

				cppc->fc_perf_feedback_addr = fc_region_addr +
					((ulong)hfresp.fc_perf_feedback_ಸ್ಥಾನಾಂತರ_hi << 32 |
					hfresp.fc_perf_feedback_ಸ್ಥಾನಾಂತರ_lo);
#ಆದರೆ_ಅಂತ್ಯ
				cppc->fc_db_supported = fc_db_supported;
				cppc->fc_db_addr = fc_db_addr;
				cppc->fc_db_ಅಗಲ = fc_db_ಅಗಲ;

				cppc->fc_db_setmask = fc_db_setmask;
				cppc->fc_db_preservemask = fc_db_preservemask;
			}
			ಇಲ್ಲದಿದ್ದರೆ {
				cppc->fc_perf_request_addr = 0;
				cppc->fc_perf_feedback_addr = 0;
				cppc->fc_db_supported = 0;
				cppc->fc_db_addr = 0;
				cppc->fc_db_ಅಗಲ = 0;
				cppc->fc_db_setmask = 0;
				cppc->fc_db_preservemask = 0;
			}
		}
		req.ಆರಂಭ_ಸೂಚ್ಯಂಕ += resp.returned;
	} ಆಗ (resp.ಉಳಿದ);

	ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ rpmi_cppc_cold_init(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ nodeoff,
			       ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_ಹೊಂದಿಕೆ *ಹೊಂದಿಕೆ)
{
	ಪೂರ್ಣಾಂಕ rc;
	ರಚನೆ mbox_chan *chan;

	ಆದರೆ (!rpmi_cppc_ಸ್ಥಾನಾಂತರ) {
		rpmi_cppc_ಸ್ಥಾನಾಂತರ =
			sbi_scratch_alloc_type_ಸ್ಥಾನಾಂತರ(ರಚನೆ rpmi_cppc);
		ಆದರೆ (!rpmi_cppc_ಸ್ಥಾನಾಂತರ)
			ಹಿಂತಿರುಗಿ SBI_ENOMEM;
	}

	/*
	 * If channel request failed then other end does not support
	 * CPPC service group so do nothing.
	 */
	rc = fdt_mailbox_request_chan(fdt, nodeoff, 0, &chan);
	ಆದರೆ (rc)
		ಹಿಂತಿರುಗಿ SBI_ENODEV;

	/* Update per-HART scratch space */
	rc = rpmi_cppc_update_hart_scratch(chan);
	ಆದರೆ (rc)
		ಹಿಂತಿರುಗಿ rc;

	sbi_cppc_ಗಣ_device(&sbi_rpmi_cppc);

	ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_ಹೊಂದಿಕೆ rpmi_cppc_ಹೊಂದಿಕೆ[] = {
	{ .compatible = "riscv,rpmi-cppc" },
	{},
};

ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_driver fdt_cppc_rpmi = {
	.ಹೊಂದಿಕೆ_ಕೋಷ್ಟಕ = rpmi_cppc_ಹೊಂದಿಕೆ,
	.init = rpmi_cppc_cold_init,
};
