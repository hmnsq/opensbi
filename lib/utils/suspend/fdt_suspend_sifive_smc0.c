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

#define SIFIVE_SMC_PGPREP_OFF			0x0
#define SIFIVE_SMC_PG_OFF			0x4
#define SIFIVE_SMC_CCTIMER_OFF			0xc
#define SIFIVE_SMC_RESUMEPC_LO_OFF		0x10
#define SIFIVE_SMC_RESUMEPC_HI_OFF		0x14
#define SIFIVE_SMC_SYNC_PMC_OFF			0x24
#define SIFIVE_SMC_CYCLECOUNT_LO_OFF		0x28
#define SIFIVE_SMC_CYCLECOUNT_HI_OFF		0x2c
#define SIFIVE_SMC_WFI_UNCORE_CG_OFF		0x50

#define SIFIVE_SMC_PGPREP_ENA_REQ		BIT(31)
#define SIFIVE_SMC_PGPREP_ENA_ACK		BIT(30)
#define SIFIVE_SMC_PGPREP_DIS_REQ		BIT(29)
#define SIFIVE_SMC_PGPREP_DIS_ACK		BIT(29)
#define SIFIVE_SMC_PGPREP_FRONTNOTQ		BIT(19)
#define SIFIVE_SMC_PGPREP_CLFPNOTQ		BIT(18)
#define SIFIVE_SMC_PGPREP_PMCENAERR		BIT(17)
#define SIFIVE_SMC_PGPREP_WAKE_DETECT		BIT(16)
#define SIFIVE_SMC_PGPREP_BUSERR		BIT(15)
#define SIFIVE_SMC_PGPREP_EARLY_ABORT		BIT(3)
#define SIFIVE_SMC_PGPREP_INTERNAL_ABORT	BIT(2)
#define SIFIVE_SMC_PGPREP_ENARSP		(SIFIVE_SMC_PGPREP_FRONTNOTQ | \
						 SIFIVE_SMC_PGPREP_CLFPNOTQ | \
						 SIFIVE_SMC_PGPREP_PMCENAERR | \
						 SIFIVE_SMC_PGPREP_WAKE_DETECT | \
						 SIFIVE_SMC_PGPREP_BUSERR)

#define SIFIVE_SMC_PGPREP_ABORT			(SIFIVE_SMC_PGPREP_EARLY_ABORT | \
						 SIFIVE_SMC_PGPREP_INTERNAL_ABORT)

#define SIFIVE_SMC_PG_ENA_REQ			BIT(31)
#define SIFIVE_SMC_PG_WARM_RESET		BIT(1)

#define SIFIVE_SMC_SYNCPMC_SYNC_REQ		BIT(31)
#define SIFIVE_SMC_SYNCPMC_SYNC_WREQ		BIT(30)
#define SIFIVE_SMC_SYNCPMC_SYNC_ACK		BIT(29)

ಸ್ಥಿರ ಸಂರಚನಾ aclint_mtimer_data smc_sync_timer;
ಸ್ಥಿರ unsigned long smc0_base;

ಸ್ಥಿರ ಶೂನ್ಯ sifive_smc0_set_pmcsync(ವರ್ಣ regid, bool write_mode)
{
	unsigned long addr = smc0_base + SIFIVE_SMC_SYNC_PMC_OFF;
	u32 v = regid | SIFIVE_SMC_SYNCPMC_SYNC_REQ;

	ಯದಿ (write_mode)
		v |= SIFIVE_SMC_SYNCPMC_SYNC_WREQ;

	writel(v, (ಶೂನ್ಯ *)addr);
	ಯಾವತ್ (!(readl((ಶೂನ್ಯ *)addr) & SIFIVE_SMC_SYNCPMC_SYNC_ACK));
}

ಸ್ಥಿರ u64 sifive_smc0_time_read(ಚಞ್ಚಲ u64 *addr)
{
	u32 lo, hi;

	ಕುರು {
		sifive_smc0_set_pmcsync(SIFIVE_SMC_CYCLECOUNT_LO_OFF, false);
		sifive_smc0_set_pmcsync(SIFIVE_SMC_CYCLECOUNT_HI_OFF, false);
		hi = readl_relaxed((u32 *)addr + 1);
		lo = readl_relaxed((u32 *)addr);
	} ಯಾವತ್ (hi != readl_relaxed((u32 *)addr + 1));

	ಪ್ರತ್ಯಾಗಮ ((u64)hi << 32) | (u64)lo;
}

ಸ್ಥಿರ ಶೂನ್ಯ sifive_smc0_set_resumepc(physical_addr_t raddr)
{
	/* Set resumepc_lo */
	writel((u32)raddr, (ಶೂನ್ಯ *)(smc0_base + SIFIVE_SMC_RESUMEPC_LO_OFF));
	/* copy resumepc_lo from SMC to PMC */
	sifive_smc0_set_pmcsync(SIFIVE_SMC_RESUMEPC_LO_OFF, true);
#ಯದಿ __riscv_xlen > 32
	/* Set resumepc_hi */
	writel((u32)(raddr >> 32), (ಶೂನ್ಯ *)(smc0_base + SIFIVE_SMC_RESUMEPC_HI_OFF));
	/* copy resumepc_hi from SMC to PMC */
	sifive_smc0_set_pmcsync(SIFIVE_SMC_RESUMEPC_HI_OFF, true);
#endif
}

ಸ್ಥಿರ u32 sifive_smc0_get_pgprep_enarsp(ಶೂನ್ಯ)
{
	u32 v = readl((ಶೂನ್ಯ *)(smc0_base + SIFIVE_SMC_PGPREP_OFF));

	ಪ್ರತ್ಯಾಗಮ v & SIFIVE_SMC_PGPREP_ENARSP;
}

ಸ್ಥಿರ ಶೂನ್ಯ sifive_smc0_set_pgprep_disreq(ಶೂನ್ಯ)
{
	unsigned long addr = smc0_base + SIFIVE_SMC_PGPREP_OFF;
	u32 v = readl((ಶೂನ್ಯ *)addr);

	writel(v | SIFIVE_SMC_PGPREP_DIS_REQ, (ಶೂನ್ಯ *)addr);
	ಯಾವತ್ (!(readl((ಶೂನ್ಯ *)addr) & SIFIVE_SMC_PGPREP_DIS_ACK));
}

ಸ್ಥಿರ u32 sifive_smc0_set_pgprep_enareq(ಶೂನ್ಯ)
{
	unsigned long addr = smc0_base + SIFIVE_SMC_PGPREP_OFF;
	u32 v = readl((ಶೂನ್ಯ *)addr);

	writel(v | SIFIVE_SMC_PGPREP_ENA_REQ, (ಶೂನ್ಯ *)addr);
	ಯಾವತ್ (!(readl((ಶೂನ್ಯ *)addr) & SIFIVE_SMC_PGPREP_ENA_ACK));

	v = readl((ಶೂನ್ಯ *)addr);

	ಪ್ರತ್ಯಾಗಮ v & SIFIVE_SMC_PGPREP_ABORT;
}

ಸ್ಥಿರ ಶೂನ್ಯ sifive_smc0_set_pg_enareq(ಶೂನ್ಯ)
{
	unsigned long addr = smc0_base + SIFIVE_SMC_PG_OFF;
	u32 v = readl((ಶೂನ್ಯ *)addr);

	writel(v | SIFIVE_SMC_PG_ENA_REQ, (ಶೂನ್ಯ *)addr);
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ sifive_smc0_set_cg(bool enable)
{
	unsigned long addr = smc0_base + SIFIVE_SMC_WFI_UNCORE_CG_OFF;

	ಯದಿ (enable)
		writel(0, (ಶೂನ್ಯ *)addr);
	ಅನ್ಯಥಾ
		writel(1, (ಶೂನ್ಯ *)addr);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ sifive_smc0_prep(ಶೂನ್ಯ)
{
	ಧ್ರುವ ಸಂರಚನಾ sbi_domain *dom = &ಬೇರು;
	ಸಂರಚನಾ sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	unsigned long i;
	ಪೂರ್ಣಾಙ್ಕ rc;
	u32 target;

	ಯದಿ (!smc0_base)
		ಪ್ರತ್ಯಾಗಮ SBI_ENODEV;

	/* Prevent all secondary tiles from waking up from PG state */
	sbi_hartmask_for_each_hartindex(i, dom->possible_harts) {
		target = sbi_hartindex_to_hartid(i);
		ಯದಿ (target != current_hartid()) {
			rc = sifive_tmc0_set_wakemask_enareq(target);
			ಯದಿ (rc) {
				sbi_printf("Fail to enable wakemask for hart %d\n",
					   target);
				ಗಚ್ಛ fail;
			}
		}
	}

	/* Check if all secondary tiles enter PG state */
	sbi_hartmask_for_each_hartindex(i, dom->possible_harts) {
		target = sbi_hartindex_to_hartid(i);
		ಯದಿ (target != current_hartid() &&
		    !sifive_tmc0_is_pg(target)) {
			sbi_printf("Hart %d not in the PG state\n", target);
			ಗಚ್ಛ fail;
		}
	}

	rc = sifive_smc0_set_pgprep_enareq();
	ಯದಿ (rc) {
		sbi_printf("SMC0 error: abort code: 0x%x\n", rc);
		ಗಚ್ಛ fail;
	}

	rc = sifive_smc0_get_pgprep_enarsp();
	ಯದಿ (rc) {
		sifive_smc0_set_pgprep_disreq();
		sbi_printf("SMC0 error: error response code: 0x%x\n", rc);
		ಗಚ್ಛ fail;
	}

	sifive_smc0_set_resumepc(scratch->warmboot_addr);
	ಪ್ರತ್ಯಾಗಮ SBI_OK;
fail:
	sbi_hartmask_for_each_hartindex(i, dom->possible_harts) {
		target = sbi_hartindex_to_hartid(i);
		ಯದಿ (target != current_hartid())
			sifive_tmc0_set_wakemask_disreq(target);
	}

	ಪ್ರತ್ಯಾಗಮ SBI_EFAIL;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ sifive_smc0_enter(ಶೂನ್ಯ)
{
	ಧ್ರುವ ಸಂರಚನಾ sbi_domain *dom = &ಬೇರು;
	ಸಂರಚನಾ sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	unsigned long i;
	u32 target, rc;

	/* Flush cache and check if there is wake detect or bus error */
	ಯದಿ (fdt_cmo_llc_flush_all() &&
	    sbi_hart_has_extension(scratch, SBI_HART_EXT_XSIFIVE_CFLUSH_D_L1))
		sifive_cflush();

	rc = sifive_smc0_get_pgprep_enarsp();
	ಯದಿ (rc) {
		sbi_printf("SMC0 error: error response code: 0x%x\n", rc);
		rc = SBI_EFAIL;
		ಗಚ್ಛ fail;
	}

	ಯದಿ (sbi_hart_has_extension(scratch, SBI_HART_EXT_XSIFIVE_CEASE)) {
		sifive_smc0_set_pg_enareq();
		ಯಾವತ್ (1)
			sifive_cease();
	}

	rc = SBI_ENOTSUPP;
fail:
	sifive_smc0_set_pgprep_disreq();
	sbi_hartmask_for_each_hartindex(i, dom->possible_harts) {
		target = sbi_hartindex_to_hartid(i);
		ಯದಿ (target != current_hartid())
			sifive_tmc0_set_wakemask_disreq(target);
	}
	ಪ್ರತ್ಯಾಗಮ rc;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ sifive_smc0_pg(ಶೂನ್ಯ)
{
	ಪೂರ್ಣಾಙ್ಕ rc;

	rc = sifive_smc0_prep();
	ಯದಿ (rc)
		ಪ್ರತ್ಯಾಗಮ rc;

	ಪ್ರತ್ಯಾಗಮ sifive_smc0_enter();
}

ಸ್ಥಿರ ಶೂನ್ಯ sifive_smc0_mtime_update(ಶೂನ್ಯ)
{
	ಸಂರಚನಾ aclint_mtimer_data *mt = aclint_get_mtimer_data();

	aclint_mtimer_update(mt, &smc_sync_timer);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ sifive_smc0_system_suspend_check(u32 sleep_type)
{
	ಪ್ರತ್ಯಾಗಮ sleep_type == SBI_SUSP_SLEEP_TYPE_SUSPEND ? SBI_OK : SBI_EINVAL;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ sifive_smc0_system_suspend(u32 sleep_type, unsigned long addr)
{
	/* Disable the timer interrupt */
	sbi_timer_exit(sbi_scratch_thishart_ptr());

	ಪ್ರತ್ಯಾಗಮ sifive_smc0_pg();
}

ಸ್ಥಿರ ಶೂನ್ಯ sifive_smc0_system_resume(ಶೂನ್ಯ)
{
	aplic_reinit_all();
	sifive_smc0_mtime_update();
}

ಸ್ಥಿರ ಸಂರಚನಾ sbi_system_suspend_device smc0_sys_susp = {
	.name = "Sifive SMC0",
	.system_suspend_check = sifive_smc0_system_suspend_check,
	.system_suspend = sifive_smc0_system_suspend,
	.system_resume = sifive_smc0_system_resume,
};

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ sifive_smc0_probe(ಧ್ರುವ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ nodeoff, ಧ್ರುವ ಸಂರಚನಾ fdt_match *match)
{
	ಪೂರ್ಣಾಙ್ಕ rc;
	u64 addr;

	rc = fdt_get_ಗಂಟು_addr_size(fdt, nodeoff, 0, &addr, ರಿಕ್ತ);
	ಯದಿ (rc)
		ಪ್ರತ್ಯಾಗಮ rc;

	smc0_base = (unsigned long)addr;
	smc_sync_timer.time_rd = sifive_smc0_time_read;
	smc_sync_timer.mtime_addr = smc0_base + SIFIVE_SMC_CYCLECOUNT_LO_OFF;

	sbi_system_suspend_set_device(&smc0_sys_susp);
	sifive_smc0_set_cg(true);

	ಪ್ರತ್ಯಾಗಮ SBI_OK;
}

ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ fdt_match sifive_smc0_match[] = {
	{ .compatible = "sifive,smc0" },
	{ },
};

ಧ್ರುವ ಸಂರಚನಾ fdt_driver fdt_suspend_sifive_smc0 = {
	.match_table = sifive_smc0_match,
	.init = sifive_smc0_probe,
};
