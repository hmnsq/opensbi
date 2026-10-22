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

ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ atcsmu_ತಳ;

ಶೂನ್ಯ atcsmu_ಗಣ_wakeup_events(u32 events, u32 hartid)
{
	writel_relaxed(events, (ಅಕ್ಷರ *)atcsmu_ತಳ + PCSm_WE_ಸ್ಥಾನಾಂತರ(hartid));
}

bool atcsmu_support_sleep_ಬಹುಲಕ(u32 sleep_type, u32 hartid)
{
	u32 pcs_cfg;
	u32 ಮುಖವಾಡ;
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *sleep_ಬಹುಲಕ;

	pcs_cfg = readl_relaxed((ಅಕ್ಷರ *)atcsmu_ತಳ + PCSm_CFG_ಸ್ಥಾನಾಂತರ(hartid));
	ಆಯ್ಕೆ (sleep_type) {
	ಸ್ಥಿತಿ SBI_SUSP_AE350_LIGHT_SLEEP:
		ಮುಖವಾಡ = PCS_CFG_LIGHT_SLEEP;
		sleep_ಬಹುಲಕ = "light sleep";
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ SBI_SUSP_SLEEP_TYPE_SUSPEND:
		ಮುಖವಾಡ = PCS_CFG_DEEP_SLEEP;
		sleep_ಬಹುಲಕ = "deep sleep";
		ನಿಲ್ಲಿಸು;
	ಪೂರ್ವನಿಯೋಜಿತ:
		ಹಿಂತಿರುಗಿ false;
	}

	ಆದರೆ (!ಹೊರತೆಗೆ_ಕ್ಷೇತ್ರ(pcs_cfg, ಮುಖವಾಡ)) {
		sbi_printf("ATCSMU: hart%d (PCS%d) does not support %s mode\n",
			   hartid, hartid + 3, sleep_ಬಹುಲಕ);
		ಹಿಂತಿರುಗಿ false;
	}

	ಹಿಂತಿರುಗಿ true;
}

ಶೂನ್ಯ atcsmu_ಗಣ_command(u32 pcs_ctl, u32 hartid)
{
	writel(pcs_ctl, (ಅಕ್ಷರ *)atcsmu_ತಳ + PCSm_CTL_ಸ್ಥಾನಾಂತರ(hartid));
}

ಪೂರ್ಣಾಂಕ atcsmu_ಗಣ_reset_ಸದಿಶ(u64 wakeup_addr, u32 hartid)
{
	u32 vec_lo;
	u32 vec_hi;
	u64 reset_ಸದಿಶ;

	writel((u32)wakeup_addr, (ಅಕ್ಷರ *)atcsmu_ತಳ + HARTn_RESET_VEC_LO(hartid));
	writel((u32)(wakeup_addr >> 32), (ಅಕ್ಷರ *)atcsmu_ತಳ + HARTn_RESET_VEC_HI(hartid));
	vec_lo = readl((ಅಕ್ಷರ *)atcsmu_ತಳ + HARTn_RESET_VEC_LO(hartid));
	vec_hi = readl((ಅಕ್ಷರ *)atcsmu_ತಳ + HARTn_RESET_VEC_HI(hartid));
	reset_ಸದಿಶ = (u64)vec_hi << 32 | vec_lo;
	ಆದರೆ (reset_ಸದಿಶ != wakeup_addr) {
		sbi_printf("ATCSMU: hart%d (PCS%d): failed to program the reset vector\n",
			   hartid, hartid + 3);
		ಹಿಂತಿರುಗಿ SBI_EFAIL;
	}

	ಹಿಂತಿರುಗಿ SBI_OK;
}

u32 atcsmu_get_sleep_type(u32 hartid)
{
	ಹಿಂತಿರುಗಿ readl_relaxed((ಅಕ್ಷರ *)atcsmu_ತಳ + PCSm_SCRATCH_ಸ್ಥಾನಾಂತರ(hartid));
}

ಶೂನ್ಯ atcsmu_write_scratch(u32 ಮೌಲ್ಯ)
{
	writel_relaxed(ಮೌಲ್ಯ, (ಅಕ್ಷರ *)atcsmu_ತಳ + SCRATCH_PAD_ಸ್ಥಾನಾಂತರ);
}

u32 atcsmu_read_scratch(ಶೂನ್ಯ)
{
	ಹಿಂತಿರುಗಿ readl_relaxed((ಅಕ್ಷರ *)atcsmu_ತಳ + SCRATCH_PAD_ಸ್ಥಾನಾಂತರ);
}

bool atcsmu_hart_is_sleep(ಶೂನ್ಯ *opaque)
{
	ರಚನೆ atcsmu_sleep_arg *arg = opaque;

	u32 pcs_ಸ್ಥಿತಿ = readl_relaxed((ಅಕ್ಷರ *)atcsmu_ತಳ +
				       PCSm_ಸ್ಥಿತಿ_ಸ್ಥಾನಾಂತರ(arg->hartid));
	u32 pd_ಸ್ಥಿತಿ = arg->deep_sleep ? PD_ಸ್ಥಿತಿ_DEEP_SLEEP :
					  PD_ಸ್ಥಿತಿ_LIGHT_SLEEP;

	ಹಿಂತಿರುಗಿ ಹೊರತೆಗೆ_ಕ್ಷೇತ್ರ(pcs_ಸ್ಥಿತಿ, PD_TYPE_ಮುಖವಾಡ) == PD_TYPE_SLEEP &&
	       ಹೊರತೆಗೆ_ಕ್ಷೇತ್ರ(pcs_ಸ್ಥಿತಿ, PD_ಸ್ಥಿತಿ_ಮುಖವಾಡ) == pd_ಸ್ಥಿತಿ;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ae350_hart_ಆರಂಭ(u32 hartid, ulong saddr)
{
	u32 hartindex = sbi_hartid_ಗಮ್ಯ_hartindex(hartid);

	ಹಿಂತಿರುಗಿ sbi_ipi_raw_send(hartindex, false);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ae350_hart_stop(ಶೂನ್ಯ)
{
	u32 hartid = current_hartid();
	u32 sleep_type = atcsmu_get_sleep_type(hartid);
	ಪೂರ್ಣಾಂಕ rc;

	/*
	 * For Andes AX25MP, the hart0 shares power domain with the last level
	 * cache. Instead of turning it off, it should fall through and jump to
	 * warmboot_addr.
	 */
	ಆದರೆ (is_andes(25) && hartid == 0)
		ಹಿಂತಿರುಗಿ SBI_ENOTSUPP;

	ಆದರೆ (!atcsmu_support_sleep_ಬಹುಲಕ(sleep_type, hartid))
		ಹಿಂತಿರುಗಿ SBI_ENOTSUPP;

	/* Prevent the core leaving the WFI mode unexpectedly */
	csr_write(CSR_MIE, 0);

	atcsmu_ಗಣ_wakeup_events(PCS_WAKEUP_MSIP_ಮುಖವಾಡ | PCS_WAKEUP_MEIP_ಮುಖವಾಡ, hartid);
	ಆದರೆ (sleep_type == SBI_SUSP_AE350_LIGHT_SLEEP) {
		/* Clock-gated only: needs MSI or MEI set to resume past the WFI */
		csr_ಗಣ(CSR_MIE, MIP_MSIP | MIP_MEIP);
		atcsmu_ಗಣ_command(LIGHT_SLEEP_CMD, hartid);
	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (sleep_type == SBI_SUSP_SLEEP_TYPE_SUSPEND) {
		/* Power-gated: SMU wakes it via cold reset, interrupts not needed */
		rc = atcsmu_ಗಣ_reset_ಸದಿಶ((ulong)ae350_enable_coherency_warmboot, hartid);
		ಆದರೆ (rc)
			ಹಿಂತಿರುಗಿ SBI_EFAIL;

		ae350_non_ret_save(sbi_scratch_thishart_ptr());
		atcsmu_ಗಣ_command(DEEP_SLEEP_CMD, hartid);
	}

	ae350_disable_coherency();
	wfi();

	/* Light sleep resumes here */
	ae350_enable_coherency();

	ಹಿಂತಿರುಗಿ SBI_ENOTSUPP;
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ sbi_hsm_device hsm_andes_atcsmu = {
	.name = "andes_atcsmu",
	.hart_ಆರಂಭ = ae350_hart_ಆರಂಭ,
	.hart_stop = ae350_hart_stop,
};

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ hsm_andes_atcsmu_probe(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ nodeoff, ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_ಹೊಂದಿಕೆ *ಹೊಂದಿಕೆ)
{
	ಪೂರ್ಣಾಂಕ poff, rc;
	u64 addr, ಗಾತ್ರ_2;

	/* Need to find the parent for the address property  */
	poff = fdt_ಪೋಷಕ_ಸ್ಥಾನಾಂತರ(fdt, nodeoff);
	ಆದರೆ (poff < 0)
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	rc = fdt_get_ಗಂಟು_addr_ಗಾತ್ರ(fdt, poff, 0, &addr, &ಗಾತ್ರ_2);
	ಆದರೆ (rc < 0 || !addr || !ಗಾತ್ರ_2)
		ಹಿಂತಿರುಗಿ SBI_ENODEV;

	ಆದರೆ (sbi_hart_has_extension(sbi_scratch_thishart_ptr(), SBI_HART_EXT_SMEPMP)) {
		rc = sbi_domain_ಬೇರು_ಕೂಡಿಸು_memrange(
			(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)addr, (ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)ಗಾತ್ರ_2, ಪುಟ_ಗಾತ್ರ,
			SBI_DOMAIN_MEMREGION_MMIO |
			SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW);
		ಆದರೆ (rc)
			ಹಿಂತಿರುಗಿ rc;
	}

	atcsmu_ತಳ = addr;
	sbi_hsm_ಗಣ_device(&hsm_andes_atcsmu);
	ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_ಹೊಂದಿಕೆ hsm_andes_atcsmu_ಹೊಂದಿಕೆ[] = {
	{ .compatible = "andestech,atcsmu-hsm" },
	{ },
};

ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_driver fdt_hsm_andes_atcsmu = {
	.ಹೊಂದಿಕೆ_ಕೋಷ್ಟಕ = hsm_andes_atcsmu_ಹೊಂದಿಕೆ,
	.init = hsm_andes_atcsmu_probe,
};
