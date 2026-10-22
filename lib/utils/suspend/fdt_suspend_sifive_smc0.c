/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 SiFive
 */

#ಅನ್ತರ್ಭಾವ <libfdt.h>
#ಅನ್ತರ್ಭಾವ <sbi/riscv_asm.h>
#ಅನ್ತರ್ಭಾವ <sbi/riscv_io.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_console.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_domain.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_error.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_hart.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_hsm.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_system.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_timer.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/cache/fdt_cmo_helper.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/fdt/fdt_driver.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/fdt/fdt_helper.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/hsm/fdt_hsm_sifive_inst.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/hsm/fdt_hsm_sifive_tmc0.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/irqchip/aplic.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/timer/aclint_mtimer.h>

#ವ್ಯಾಖ್ಯಾನಿಸು SIFIVE_SMC_PGPREP_OFF			0x0
#ವ್ಯಾಖ್ಯಾನಿಸು SIFIVE_SMC_PG_OFF			0x4
#ವ್ಯಾಖ್ಯಾನಿಸು SIFIVE_SMC_CCTIMER_OFF			0xc
#ವ್ಯಾಖ್ಯಾನಿಸು SIFIVE_SMC_RESUMEPC_LO_OFF		0x10
#ವ್ಯಾಖ್ಯಾನಿಸು SIFIVE_SMC_RESUMEPC_HI_OFF		0x14
#ವ್ಯಾಖ್ಯಾನಿಸು SIFIVE_SMC_SYNC_PMC_OFF			0x24
#ವ್ಯಾಖ್ಯಾನಿಸು SIFIVE_SMC_CYCLECOUNT_LO_OFF		0x28
#ವ್ಯಾಖ್ಯಾನಿಸು SIFIVE_SMC_CYCLECOUNT_HI_OFF		0x2c
#ವ್ಯಾಖ್ಯಾನಿಸು SIFIVE_SMC_WFI_UNCORE_CG_OFF		0x50

#ವ್ಯಾಖ್ಯಾನಿಸು SIFIVE_SMC_PGPREP_ENA_REQ		ಬಿಟ್(31)
#ವ್ಯಾಖ್ಯಾನಿಸು SIFIVE_SMC_PGPREP_ENA_ACK		ಬಿಟ್(30)
#ವ್ಯಾಖ್ಯಾನಿಸು SIFIVE_SMC_PGPREP_DIS_REQ		ಬಿಟ್(29)
#ವ್ಯಾಖ್ಯಾನಿಸು SIFIVE_SMC_PGPREP_DIS_ACK		ಬಿಟ್(29)
#ವ್ಯಾಖ್ಯಾನಿಸು SIFIVE_SMC_PGPREP_FRONTNOTQ		ಬಿಟ್(19)
#ವ್ಯಾಖ್ಯಾನಿಸು SIFIVE_SMC_PGPREP_CLFPNOTQ		ಬಿಟ್(18)
#ವ್ಯಾಖ್ಯಾನಿಸು SIFIVE_SMC_PGPREP_PMCENAERR		ಬಿಟ್(17)
#ವ್ಯಾಖ್ಯಾನಿಸು SIFIVE_SMC_PGPREP_WAKE_DETECT		ಬಿಟ್(16)
#ವ್ಯಾಖ್ಯಾನಿಸು SIFIVE_SMC_PGPREP_BUSERR		ಬಿಟ್(15)
#ವ್ಯಾಖ್ಯಾನಿಸು SIFIVE_SMC_PGPREP_EARLY_ABORT		ಬಿಟ್(3)
#ವ್ಯಾಖ್ಯಾನಿಸು SIFIVE_SMC_PGPREP_ಆಂತರಿಕ_ABORT	ಬಿಟ್(2)
#ವ್ಯಾಖ್ಯಾನಿಸು SIFIVE_SMC_PGPREP_ENARSP		(SIFIVE_SMC_PGPREP_FRONTNOTQ | \
						 SIFIVE_SMC_PGPREP_CLFPNOTQ | \
						 SIFIVE_SMC_PGPREP_PMCENAERR | \
						 SIFIVE_SMC_PGPREP_WAKE_DETECT | \
						 SIFIVE_SMC_PGPREP_BUSERR)

#ವ್ಯಾಖ್ಯಾನಿಸು SIFIVE_SMC_PGPREP_ABORT			(SIFIVE_SMC_PGPREP_EARLY_ABORT | \
						 SIFIVE_SMC_PGPREP_ಆಂತರಿಕ_ABORT)

#ವ್ಯಾಖ್ಯಾನಿಸು SIFIVE_SMC_PG_ENA_REQ			ಬಿಟ್(31)
#ವ್ಯಾಖ್ಯಾನಿಸು SIFIVE_SMC_PG_WARM_RESET		ಬಿಟ್(1)

#ವ್ಯಾಖ್ಯಾನಿಸು SIFIVE_SMC_SYNCPMC_SYNC_REQ		ಬಿಟ್(31)
#ವ್ಯಾಖ್ಯಾನಿಸು SIFIVE_SMC_SYNCPMC_SYNC_WREQ		ಬಿಟ್(30)
#ವ್ಯಾಖ್ಯಾನಿಸು SIFIVE_SMC_SYNCPMC_SYNC_ACK		ಬಿಟ್(29)

ಸ್ಥಿರ ರಚನೆ aclint_mtimer_ದತ್ತಾಂಶ smc_sync_timer;
ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ smc0_ತಳ;

ಸ್ಥಿರ ಶೂನ್ಯ sifive_smc0_ಗಣ_pmcsync(ಅಕ್ಷರ regid, bool write_ಬಹುಲಕ)
{
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ addr = smc0_ತಳ + SIFIVE_SMC_SYNC_PMC_OFF;
	u32 v = regid | SIFIVE_SMC_SYNCPMC_SYNC_REQ;

	ಆದರೆ (write_ಬಹುಲಕ)
		v |= SIFIVE_SMC_SYNCPMC_SYNC_WREQ;

	writel(v, (ಶೂನ್ಯ *)addr);
	ಆಗ (!(readl((ಶೂನ್ಯ *)addr) & SIFIVE_SMC_SYNCPMC_SYNC_ACK));
}

ಸ್ಥಿರ u64 sifive_smc0_ಸಮಯ_read(ಅಸ್ಥಿರ u64 *addr)
{
	u32 lo, hi;

	ಮಾಡು {
		sifive_smc0_ಗಣ_pmcsync(SIFIVE_SMC_CYCLECOUNT_LO_OFF, false);
		sifive_smc0_ಗಣ_pmcsync(SIFIVE_SMC_CYCLECOUNT_HI_OFF, false);
		hi = readl_relaxed((u32 *)addr + 1);
		lo = readl_relaxed((u32 *)addr);
	} ಆಗ (hi != readl_relaxed((u32 *)addr + 1));

	ಹಿಂತಿರುಗಿ ((u64)hi << 32) | (u64)lo;
}

ಸ್ಥಿರ ಶೂನ್ಯ sifive_smc0_ಗಣ_resumepc(physical_addr_t raddr)
{
	/* Set resumepc_lo */
	writel((u32)raddr, (ಶೂನ್ಯ *)(smc0_ತಳ + SIFIVE_SMC_RESUMEPC_LO_OFF));
	/* copy resumepc_lo from SMC to PMC */
	sifive_smc0_ಗಣ_pmcsync(SIFIVE_SMC_RESUMEPC_LO_OFF, true);
#ಆದರೆ __riscv_xlen > 32
	/* Set resumepc_hi */
	writel((u32)(raddr >> 32), (ಶೂನ್ಯ *)(smc0_ತಳ + SIFIVE_SMC_RESUMEPC_HI_OFF));
	/* copy resumepc_hi from SMC to PMC */
	sifive_smc0_ಗಣ_pmcsync(SIFIVE_SMC_RESUMEPC_HI_OFF, true);
#ಆದರೆ_ಅಂತ್ಯ
}

ಸ್ಥಿರ u32 sifive_smc0_get_pgprep_enarsp(ಶೂನ್ಯ)
{
	u32 v = readl((ಶೂನ್ಯ *)(smc0_ತಳ + SIFIVE_SMC_PGPREP_OFF));

	ಹಿಂತಿರುಗಿ v & SIFIVE_SMC_PGPREP_ENARSP;
}

ಸ್ಥಿರ ಶೂನ್ಯ sifive_smc0_ಗಣ_pgprep_disreq(ಶೂನ್ಯ)
{
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ addr = smc0_ತಳ + SIFIVE_SMC_PGPREP_OFF;
	u32 v = readl((ಶೂನ್ಯ *)addr);

	writel(v | SIFIVE_SMC_PGPREP_DIS_REQ, (ಶೂನ್ಯ *)addr);
	ಆಗ (!(readl((ಶೂನ್ಯ *)addr) & SIFIVE_SMC_PGPREP_DIS_ACK));
}

ಸ್ಥಿರ u32 sifive_smc0_ಗಣ_pgprep_enareq(ಶೂನ್ಯ)
{
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ addr = smc0_ತಳ + SIFIVE_SMC_PGPREP_OFF;
	u32 v = readl((ಶೂನ್ಯ *)addr);

	writel(v | SIFIVE_SMC_PGPREP_ENA_REQ, (ಶೂನ್ಯ *)addr);
	ಆಗ (!(readl((ಶೂನ್ಯ *)addr) & SIFIVE_SMC_PGPREP_ENA_ACK));

	v = readl((ಶೂನ್ಯ *)addr);

	ಹಿಂತಿರುಗಿ v & SIFIVE_SMC_PGPREP_ABORT;
}

ಸ್ಥಿರ ಶೂನ್ಯ sifive_smc0_ಗಣ_pg_enareq(ಶೂನ್ಯ)
{
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ addr = smc0_ತಳ + SIFIVE_SMC_PG_OFF;
	u32 v = readl((ಶೂನ್ಯ *)addr);

	writel(v | SIFIVE_SMC_PG_ENA_REQ, (ಶೂನ್ಯ *)addr);
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ sifive_smc0_ಗಣ_cg(bool enable)
{
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ addr = smc0_ತಳ + SIFIVE_SMC_WFI_UNCORE_CG_OFF;

	ಆದರೆ (enable)
		writel(0, (ಶೂನ್ಯ *)addr);
	ಇಲ್ಲದಿದ್ದರೆ
		writel(1, (ಶೂನ್ಯ *)addr);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ sifive_smc0_prep(ಶೂನ್ಯ)
{
	ಸ್ಥಿರಾಂಕ ರಚನೆ sbi_domain *dom = &ಬೇರು;
	ರಚನೆ sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ i;
	ಪೂರ್ಣಾಂಕ rc;
	u32 ಗುರಿ;

	ಆದರೆ (!smc0_ತಳ)
		ಹಿಂತಿರುಗಿ SBI_ENODEV;

	/* Prevent all secondary tiles from waking up from PG state */
	sbi_hartmask_for_each_hartindex(i, dom->possible_harts) {
		ಗುರಿ = sbi_hartindex_ಗಮ್ಯ_hartid(i);
		ಆದರೆ (ಗುರಿ != current_hartid()) {
			rc = sifive_tmc0_ಗಣ_wakemask_enareq(ಗುರಿ);
			ಆದರೆ (rc) {
				sbi_printf("Fail to enable wakemask for hart %d\n",
					   ಗುರಿ);
				ಹೋಗು fail;
			}
		}
	}

	/* Check if all secondary tiles enter PG state */
	sbi_hartmask_for_each_hartindex(i, dom->possible_harts) {
		ಗುರಿ = sbi_hartindex_ಗಮ್ಯ_hartid(i);
		ಆದರೆ (ಗುರಿ != current_hartid() &&
		    !sifive_tmc0_is_pg(ಗುರಿ)) {
			sbi_printf("Hart %d not in the PG state\n", ಗುರಿ);
			ಹೋಗು fail;
		}
	}

	rc = sifive_smc0_ಗಣ_pgprep_enareq();
	ಆದರೆ (rc) {
		sbi_printf("SMC0 error: abort code: 0x%x\n", rc);
		ಹೋಗು fail;
	}

	rc = sifive_smc0_get_pgprep_enarsp();
	ಆದರೆ (rc) {
		sifive_smc0_ಗಣ_pgprep_disreq();
		sbi_printf("SMC0 error: error response code: 0x%x\n", rc);
		ಹೋಗು fail;
	}

	sifive_smc0_ಗಣ_resumepc(scratch->warmboot_addr);
	ಹಿಂತಿರುಗಿ SBI_OK;
fail:
	sbi_hartmask_for_each_hartindex(i, dom->possible_harts) {
		ಗುರಿ = sbi_hartindex_ಗಮ್ಯ_hartid(i);
		ಆದರೆ (ಗುರಿ != current_hartid())
			sifive_tmc0_ಗಣ_wakemask_disreq(ಗುರಿ);
	}

	ಹಿಂತಿರುಗಿ SBI_EFAIL;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ sifive_smc0_enter(ಶೂನ್ಯ)
{
	ಸ್ಥಿರಾಂಕ ರಚನೆ sbi_domain *dom = &ಬೇರು;
	ರಚನೆ sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ i;
	u32 ಗುರಿ, rc;

	/* Flush cache and check if there is wake detect or bus error */
	ಆದರೆ (fdt_cmo_llc_flush_all() &&
	    sbi_hart_has_extension(scratch, SBI_HART_EXT_XSIFIVE_CFLUSH_D_L1))
		sifive_cflush();

	rc = sifive_smc0_get_pgprep_enarsp();
	ಆದರೆ (rc) {
		sbi_printf("SMC0 error: error response code: 0x%x\n", rc);
		rc = SBI_EFAIL;
		ಹೋಗು fail;
	}

	ಆದರೆ (sbi_hart_has_extension(scratch, SBI_HART_EXT_XSIFIVE_CEASE)) {
		sifive_smc0_ಗಣ_pg_enareq();
		ಆಗ (1)
			sifive_cease();
	}

	rc = SBI_ENOTSUPP;
fail:
	sifive_smc0_ಗಣ_pgprep_disreq();
	sbi_hartmask_for_each_hartindex(i, dom->possible_harts) {
		ಗುರಿ = sbi_hartindex_ಗಮ್ಯ_hartid(i);
		ಆದರೆ (ಗುರಿ != current_hartid())
			sifive_tmc0_ಗಣ_wakemask_disreq(ಗುರಿ);
	}
	ಹಿಂತಿರುಗಿ rc;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ sifive_smc0_pg(ಶೂನ್ಯ)
{
	ಪೂರ್ಣಾಂಕ rc;

	rc = sifive_smc0_prep();
	ಆದರೆ (rc)
		ಹಿಂತಿರುಗಿ rc;

	ಹಿಂತಿರುಗಿ sifive_smc0_enter();
}

ಸ್ಥಿರ ಶೂನ್ಯ sifive_smc0_mtime_update(ಶೂನ್ಯ)
{
	ರಚನೆ aclint_mtimer_ದತ್ತಾಂಶ *mt = aclint_get_mtimer_ದತ್ತಾಂಶ();

	aclint_mtimer_update(mt, &smc_sync_timer);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ sifive_smc0_system_suspend_check(u32 sleep_type)
{
	ಹಿಂತಿರುಗಿ sleep_type == SBI_SUSP_SLEEP_TYPE_SUSPEND ? SBI_OK : SBI_EINVAL;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ sifive_smc0_system_suspend(u32 sleep_type, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ addr)
{
	/* Disable the timer interrupt */
	sbi_timer_exit(sbi_scratch_thishart_ptr());

	ಹಿಂತಿರುಗಿ sifive_smc0_pg();
}

ಸ್ಥಿರ ಶೂನ್ಯ sifive_smc0_system_resume(ಶೂನ್ಯ)
{
	aplic_reinit_all();
	sifive_smc0_mtime_update();
}

ಸ್ಥಿರ ರಚನೆ sbi_system_suspend_device smc0_sys_susp = {
	.name = "Sifive SMC0",
	.system_suspend_check = sifive_smc0_system_suspend_check,
	.system_suspend = sifive_smc0_system_suspend,
	.system_resume = sifive_smc0_system_resume,
};

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ sifive_smc0_probe(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ nodeoff, ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_ಹೊಂದಿಕೆ *ಹೊಂದಿಕೆ)
{
	ಪೂರ್ಣಾಂಕ rc;
	u64 addr;

	rc = fdt_get_ಗಂಟು_addr_ಗಾತ್ರ(fdt, nodeoff, 0, &addr, ಖಾಲಿ);
	ಆದರೆ (rc)
		ಹಿಂತಿರುಗಿ rc;

	smc0_ತಳ = (ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)addr;
	smc_sync_timer.ಸಮಯ_rd = sifive_smc0_ಸಮಯ_read;
	smc_sync_timer.mtime_addr = smc0_ತಳ + SIFIVE_SMC_CYCLECOUNT_LO_OFF;

	sbi_system_suspend_ಗಣ_device(&smc0_sys_susp);
	sifive_smc0_ಗಣ_cg(true);

	ಹಿಂತಿರುಗಿ SBI_OK;
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_ಹೊಂದಿಕೆ sifive_smc0_ಹೊಂದಿಕೆ[] = {
	{ .compatible = "sifive,smc0" },
	{ },
};

ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_driver fdt_suspend_sifive_smc0 = {
	.ಹೊಂದಿಕೆ_ಕೋಷ್ಟಕ = sifive_smc0_ಹೊಂದಿಕೆ,
	.init = sifive_smc0_probe,
};
