/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 Andes Technology Corporation
 */

#ಅನ್ತರ್ಭಾವ <andes/andes.h>
#ಅನ್ತರ್ಭಾವ <libfdt.h>
#ಅನ್ತರ್ಭಾವ <sbi/riscv_io.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_console.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_ecall_interface.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_error.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_hart.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_hsm.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_init.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_ipi.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/fdt/fdt_driver.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/fdt/fdt_helper.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/hsm/fdt_hsm_andes_atcsmu.h>

ಸ್ಥಿರ unsigned long atcsmu_base;

ಶೂನ್ಯ atcsmu_set_wakeup_events(u32 events, u32 hartid)
{
	writel_relaxed(events, (ವರ್ಣ *)atcsmu_base + PCSm_WE_OFFSET(hartid));
}

bool atcsmu_support_sleep_mode(u32 sleep_type, u32 hartid)
{
	u32 pcs_cfg;
	u32 mask;
	ಧ್ರುವ ವರ್ಣ *sleep_mode;

	pcs_cfg = readl_relaxed((ವರ್ಣ *)atcsmu_base + PCSm_CFG_OFFSET(hartid));
	ವಿಕಲ್ಪ (sleep_type) {
	ಪಕ್ಷ SBI_SUSP_AE350_LIGHT_SLEEP:
		mask = PCS_CFG_LIGHT_SLEEP;
		sleep_mode = "light sleep";
		ಭಙ್ಗ;
	ಪಕ್ಷ SBI_SUSP_SLEEP_TYPE_SUSPEND:
		mask = PCS_CFG_DEEP_SLEEP;
		sleep_mode = "deep sleep";
		ಭಙ್ಗ;
	ಸ್ವಭಾವ:
		ಪ್ರತ್ಯಾಗಮ false;
	}

	ಯದಿ (!EXTRACT_FIELD(pcs_cfg, mask)) {
		sbi_printf("ATCSMU: hart%d (PCS%d) does not support %s mode\n",
			   hartid, hartid + 3, sleep_mode);
		ಪ್ರತ್ಯಾಗಮ false;
	}

	ಪ್ರತ್ಯಾಗಮ true;
}

ಶೂನ್ಯ atcsmu_set_command(u32 pcs_ctl, u32 hartid)
{
	writel(pcs_ctl, (ವರ್ಣ *)atcsmu_base + PCSm_CTL_OFFSET(hartid));
}

ಪೂರ್ಣಾಙ್ಕ atcsmu_set_reset_vector(u64 wakeup_addr, u32 hartid)
{
	u32 vec_lo;
	u32 vec_hi;
	u64 reset_vector;

	writel((u32)wakeup_addr, (ವರ್ಣ *)atcsmu_base + HARTn_RESET_VEC_LO(hartid));
	writel((u32)(wakeup_addr >> 32), (ವರ್ಣ *)atcsmu_base + HARTn_RESET_VEC_HI(hartid));
	vec_lo = readl((ವರ್ಣ *)atcsmu_base + HARTn_RESET_VEC_LO(hartid));
	vec_hi = readl((ವರ್ಣ *)atcsmu_base + HARTn_RESET_VEC_HI(hartid));
	reset_vector = (u64)vec_hi << 32 | vec_lo;
	ಯದಿ (reset_vector != wakeup_addr) {
		sbi_printf("ATCSMU: hart%d (PCS%d): failed to program the reset vector\n",
			   hartid, hartid + 3);
		ಪ್ರತ್ಯಾಗಮ SBI_EFAIL;
	}

	ಪ್ರತ್ಯಾಗಮ SBI_OK;
}

u32 atcsmu_get_sleep_type(u32 hartid)
{
	ಪ್ರತ್ಯಾಗಮ readl_relaxed((ವರ್ಣ *)atcsmu_base + PCSm_SCRATCH_OFFSET(hartid));
}

ಶೂನ್ಯ atcsmu_write_scratch(u32 ಮೌಲ್ಯ)
{
	writel_relaxed(ಮೌಲ್ಯ, (ವರ್ಣ *)atcsmu_base + SCRATCH_PAD_OFFSET);
}

u32 atcsmu_read_scratch(ಶೂನ್ಯ)
{
	ಪ್ರತ್ಯಾಗಮ readl_relaxed((ವರ್ಣ *)atcsmu_base + SCRATCH_PAD_OFFSET);
}

bool atcsmu_hart_is_sleep(ಶೂನ್ಯ *opaque)
{
	ಸಂರಚನಾ atcsmu_sleep_arg *arg = opaque;

	u32 pcs_status = readl_relaxed((ವರ್ಣ *)atcsmu_base +
				       PCSm_STATUS_OFFSET(arg->hartid));
	u32 pd_status = arg->deep_sleep ? PD_STATUS_DEEP_SLEEP :
					  PD_STATUS_LIGHT_SLEEP;

	ಪ್ರತ್ಯಾಗಮ EXTRACT_FIELD(pcs_status, PD_TYPE_MASK) == PD_TYPE_SLEEP &&
	       EXTRACT_FIELD(pcs_status, PD_STATUS_MASK) == pd_status;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ ae350_hart_start(u32 hartid, ulong saddr)
{
	u32 hartindex = sbi_hartid_to_hartindex(hartid);

	ಪ್ರತ್ಯಾಗಮ sbi_ipi_raw_send(hartindex, false);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ ae350_hart_stop(ಶೂನ್ಯ)
{
	u32 hartid = current_hartid();
	u32 sleep_type = atcsmu_get_sleep_type(hartid);
	ಪೂರ್ಣಾಙ್ಕ rc;

	/*
	 * For Andes AX25MP, the hart0 shares power domain with the last level
	 * cache. Instead of turning it off, it should fall through and jump to
	 * warmboot_addr.
	 */
	ಯದಿ (is_andes(25) && hartid == 0)
		ಪ್ರತ್ಯಾಗಮ SBI_ENOTSUPP;

	ಯದಿ (!atcsmu_support_sleep_mode(sleep_type, hartid))
		ಪ್ರತ್ಯಾಗಮ SBI_ENOTSUPP;

	/* Prevent the core leaving the WFI mode unexpectedly */
	csr_write(CSR_MIE, 0);

	atcsmu_set_wakeup_events(PCS_WAKEUP_MSIP_MASK | PCS_WAKEUP_MEIP_MASK, hartid);
	ಯದಿ (sleep_type == SBI_SUSP_AE350_LIGHT_SLEEP) {
		/* Clock-gated only: needs MSI or MEI set to resume past the WFI */
		csr_set(CSR_MIE, MIP_MSIP | MIP_MEIP);
		atcsmu_set_command(LIGHT_SLEEP_CMD, hartid);
	} ಅನ್ಯಥಾ ಯದಿ (sleep_type == SBI_SUSP_SLEEP_TYPE_SUSPEND) {
		/* Power-gated: SMU wakes it via cold reset, interrupts not needed */
		rc = atcsmu_set_reset_vector((ulong)ae350_enable_coherency_warmboot, hartid);
		ಯದಿ (rc)
			ಪ್ರತ್ಯಾಗಮ SBI_EFAIL;

		ae350_non_ret_save(sbi_scratch_thishart_ptr());
		atcsmu_set_command(DEEP_SLEEP_CMD, hartid);
	}

	ae350_disable_coherency();
	wfi();

	/* Light sleep resumes here */
	ae350_enable_coherency();

	ಪ್ರತ್ಯಾಗಮ SBI_ENOTSUPP;
}

ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ sbi_hsm_device hsm_andes_atcsmu = {
	.name = "andes_atcsmu",
	.hart_start = ae350_hart_start,
	.hart_stop = ae350_hart_stop,
};

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ hsm_andes_atcsmu_probe(ಧ್ರುವ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ nodeoff, ಧ್ರುವ ಸಂರಚನಾ fdt_match *match)
{
	ಪೂರ್ಣಾಙ್ಕ poff, rc;
	u64 addr, size;

	/* Need to find the parent for the address property  */
	poff = fdt_parent_offset(fdt, nodeoff);
	ಯದಿ (poff < 0)
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;

	rc = fdt_get_ಗಂಟು_addr_size(fdt, poff, 0, &addr, &size);
	ಯದಿ (rc < 0 || !addr || !size)
		ಪ್ರತ್ಯಾಗಮ SBI_ENODEV;

	ಯದಿ (sbi_hart_has_extension(sbi_scratch_thishart_ptr(), SBI_HART_EXT_SMEPMP)) {
		rc = sbi_domain_ಬೇರು_add_memrange(
			(unsigned long)addr, (unsigned long)size, PAGE_SIZE,
			SBI_DOMAIN_MEMREGION_MMIO |
			SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW);
		ಯದಿ (rc)
			ಪ್ರತ್ಯಾಗಮ rc;
	}

	atcsmu_base = addr;
	sbi_hsm_set_device(&hsm_andes_atcsmu);
	ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ fdt_match hsm_andes_atcsmu_match[] = {
	{ .compatible = "andestech,atcsmu-hsm" },
	{ },
};

ಧ್ರುವ ಸಂರಚನಾ fdt_driver fdt_hsm_andes_atcsmu = {
	.match_table = hsm_andes_atcsmu_match,
	.init = hsm_andes_atcsmu_probe,
};
