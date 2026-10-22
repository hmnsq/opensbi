/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 SiFive
 */

#अन्तर्भाव <libfdt.h>
#अन्तर्भाव <sbi/riscv_asm.h>
#अन्तर्भाव <sbi/riscv_io.h>
#अन्तर्भाव <sbi/sbi_console.h>
#अन्तर्भाव <sbi/sbi_domain.h>
#अन्तर्भाव <sbi/sbi_error.h>
#अन्तर्भाव <sbi/sbi_hart.h>
#अन्तर्भाव <sbi/sbi_hsm.h>
#अन्तर्भाव <sbi/sbi_system.h>
#अन्तर्भाव <sbi/sbi_timer.h>
#अन्तर्भाव <sbi_utils/cache/fdt_cmo_helper.h>
#अन्तर्भाव <sbi_utils/fdt/fdt_driver.h>
#अन्तर्भाव <sbi_utils/fdt/fdt_helper.h>
#अन्तर्भाव <sbi_utils/hsm/fdt_hsm_sifive_inst.h>
#अन्तर्भाव <sbi_utils/hsm/fdt_hsm_sifive_tmc0.h>
#अन्तर्भाव <sbi_utils/irqchip/aplic.h>
#अन्तर्भाव <sbi_utils/timer/aclint_mtimer.h>

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

स्थिर संरचना aclint_mtimer_data smc_sync_timer;
स्थिर अचिन्हित दीर्घ smc0_base;

स्थिर शून्य sifive_smc0_set_pmcsync(अक्षर regid, bool write_mode)
{
	अचिन्हित दीर्घ addr = smc0_base + SIFIVE_SMC_SYNC_PMC_OFF;
	u32 v = regid | SIFIVE_SMC_SYNCPMC_SYNC_REQ;

	जर (write_mode)
		v |= SIFIVE_SMC_SYNCPMC_SYNC_WREQ;

	writel(v, (शून्य *)addr);
	पर्यंत (!(readl((शून्य *)addr) & SIFIVE_SMC_SYNCPMC_SYNC_ACK));
}

स्थिर u64 sifive_smc0_time_read(अस्थिर u64 *addr)
{
	u32 lo, hi;

	करा {
		sifive_smc0_set_pmcsync(SIFIVE_SMC_CYCLECOUNT_LO_OFF, false);
		sifive_smc0_set_pmcsync(SIFIVE_SMC_CYCLECOUNT_HI_OFF, false);
		hi = readl_relaxed((u32 *)addr + 1);
		lo = readl_relaxed((u32 *)addr);
	} पर्यंत (hi != readl_relaxed((u32 *)addr + 1));

	परत ((u64)hi << 32) | (u64)lo;
}

स्थिर शून्य sifive_smc0_set_resumepc(physical_addr_t raddr)
{
	/* Set resumepc_lo */
	writel((u32)raddr, (शून्य *)(smc0_base + SIFIVE_SMC_RESUMEPC_LO_OFF));
	/* copy resumepc_lo from SMC to PMC */
	sifive_smc0_set_pmcsync(SIFIVE_SMC_RESUMEPC_LO_OFF, true);
#जर __riscv_xlen > 32
	/* Set resumepc_hi */
	writel((u32)(raddr >> 32), (शून्य *)(smc0_base + SIFIVE_SMC_RESUMEPC_HI_OFF));
	/* copy resumepc_hi from SMC to PMC */
	sifive_smc0_set_pmcsync(SIFIVE_SMC_RESUMEPC_HI_OFF, true);
#endif
}

स्थिर u32 sifive_smc0_get_pgprep_enarsp(शून्य)
{
	u32 v = readl((शून्य *)(smc0_base + SIFIVE_SMC_PGPREP_OFF));

	परत v & SIFIVE_SMC_PGPREP_ENARSP;
}

स्थिर शून्य sifive_smc0_set_pgprep_disreq(शून्य)
{
	अचिन्हित दीर्घ addr = smc0_base + SIFIVE_SMC_PGPREP_OFF;
	u32 v = readl((शून्य *)addr);

	writel(v | SIFIVE_SMC_PGPREP_DIS_REQ, (शून्य *)addr);
	पर्यंत (!(readl((शून्य *)addr) & SIFIVE_SMC_PGPREP_DIS_ACK));
}

स्थिर u32 sifive_smc0_set_pgprep_enareq(शून्य)
{
	अचिन्हित दीर्घ addr = smc0_base + SIFIVE_SMC_PGPREP_OFF;
	u32 v = readl((शून्य *)addr);

	writel(v | SIFIVE_SMC_PGPREP_ENA_REQ, (शून्य *)addr);
	पर्यंत (!(readl((शून्य *)addr) & SIFIVE_SMC_PGPREP_ENA_ACK));

	v = readl((शून्य *)addr);

	परत v & SIFIVE_SMC_PGPREP_ABORT;
}

स्थिर शून्य sifive_smc0_set_pg_enareq(शून्य)
{
	अचिन्हित दीर्घ addr = smc0_base + SIFIVE_SMC_PG_OFF;
	u32 v = readl((शून्य *)addr);

	writel(v | SIFIVE_SMC_PG_ENA_REQ, (शून्य *)addr);
}

स्थिर ओळीत शून्य sifive_smc0_set_cg(bool enable)
{
	अचिन्हित दीर्घ addr = smc0_base + SIFIVE_SMC_WFI_UNCORE_CG_OFF;

	जर (enable)
		writel(0, (शून्य *)addr);
	अन्यथा
		writel(1, (शून्य *)addr);
}

स्थिर पूर्णांक sifive_smc0_prep(शून्य)
{
	स्थिरांक संरचना sbi_domain *dom = &मूळ;
	संरचना sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	अचिन्हित दीर्घ i;
	पूर्णांक rc;
	u32 target;

	जर (!smc0_base)
		परत SBI_ENODEV;

	/* Prevent all secondary tiles from waking up from PG state */
	sbi_hartmask_for_each_hartindex(i, dom->possible_harts) {
		target = sbi_hartindex_to_hartid(i);
		जर (target != current_hartid()) {
			rc = sifive_tmc0_set_wakemask_enareq(target);
			जर (rc) {
				sbi_printf("Fail to enable wakemask for hart %d\n",
					   target);
				जा fail;
			}
		}
	}

	/* Check if all secondary tiles enter PG state */
	sbi_hartmask_for_each_hartindex(i, dom->possible_harts) {
		target = sbi_hartindex_to_hartid(i);
		जर (target != current_hartid() &&
		    !sifive_tmc0_is_pg(target)) {
			sbi_printf("Hart %d not in the PG state\n", target);
			जा fail;
		}
	}

	rc = sifive_smc0_set_pgprep_enareq();
	जर (rc) {
		sbi_printf("SMC0 error: abort code: 0x%x\n", rc);
		जा fail;
	}

	rc = sifive_smc0_get_pgprep_enarsp();
	जर (rc) {
		sifive_smc0_set_pgprep_disreq();
		sbi_printf("SMC0 error: error response code: 0x%x\n", rc);
		जा fail;
	}

	sifive_smc0_set_resumepc(scratch->warmboot_addr);
	परत SBI_OK;
fail:
	sbi_hartmask_for_each_hartindex(i, dom->possible_harts) {
		target = sbi_hartindex_to_hartid(i);
		जर (target != current_hartid())
			sifive_tmc0_set_wakemask_disreq(target);
	}

	परत SBI_EFAIL;
}

स्थिर पूर्णांक sifive_smc0_enter(शून्य)
{
	स्थिरांक संरचना sbi_domain *dom = &मूळ;
	संरचना sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	अचिन्हित दीर्घ i;
	u32 target, rc;

	/* Flush cache and check if there is wake detect or bus error */
	जर (fdt_cmo_llc_flush_all() &&
	    sbi_hart_has_extension(scratch, SBI_HART_EXT_XSIFIVE_CFLUSH_D_L1))
		sifive_cflush();

	rc = sifive_smc0_get_pgprep_enarsp();
	जर (rc) {
		sbi_printf("SMC0 error: error response code: 0x%x\n", rc);
		rc = SBI_EFAIL;
		जा fail;
	}

	जर (sbi_hart_has_extension(scratch, SBI_HART_EXT_XSIFIVE_CEASE)) {
		sifive_smc0_set_pg_enareq();
		पर्यंत (1)
			sifive_cease();
	}

	rc = SBI_ENOTSUPP;
fail:
	sifive_smc0_set_pgprep_disreq();
	sbi_hartmask_for_each_hartindex(i, dom->possible_harts) {
		target = sbi_hartindex_to_hartid(i);
		जर (target != current_hartid())
			sifive_tmc0_set_wakemask_disreq(target);
	}
	परत rc;
}

स्थिर पूर्णांक sifive_smc0_pg(शून्य)
{
	पूर्णांक rc;

	rc = sifive_smc0_prep();
	जर (rc)
		परत rc;

	परत sifive_smc0_enter();
}

स्थिर शून्य sifive_smc0_mtime_update(शून्य)
{
	संरचना aclint_mtimer_data *mt = aclint_get_mtimer_data();

	aclint_mtimer_update(mt, &smc_sync_timer);
}

स्थिर पूर्णांक sifive_smc0_system_suspend_check(u32 sleep_type)
{
	परत sleep_type == SBI_SUSP_SLEEP_TYPE_SUSPEND ? SBI_OK : SBI_EINVAL;
}

स्थिर पूर्णांक sifive_smc0_system_suspend(u32 sleep_type, अचिन्हित दीर्घ addr)
{
	/* Disable the timer interrupt */
	sbi_timer_exit(sbi_scratch_thishart_ptr());

	परत sifive_smc0_pg();
}

स्थिर शून्य sifive_smc0_system_resume(शून्य)
{
	aplic_reinit_all();
	sifive_smc0_mtime_update();
}

स्थिर संरचना sbi_system_suspend_device smc0_sys_susp = {
	.name = "Sifive SMC0",
	.system_suspend_check = sifive_smc0_system_suspend_check,
	.system_suspend = sifive_smc0_system_suspend,
	.system_resume = sifive_smc0_system_resume,
};

स्थिर पूर्णांक sifive_smc0_probe(स्थिरांक शून्य *fdt, पूर्णांक nodeoff, स्थिरांक संरचना fdt_match *match)
{
	पूर्णांक rc;
	u64 addr;

	rc = fdt_get_गाठ_addr_size(fdt, nodeoff, 0, &addr, रिक्त);
	जर (rc)
		परत rc;

	smc0_base = (अचिन्हित दीर्घ)addr;
	smc_sync_timer.time_rd = sifive_smc0_time_read;
	smc_sync_timer.mtime_addr = smc0_base + SIFIVE_SMC_CYCLECOUNT_LO_OFF;

	sbi_system_suspend_set_device(&smc0_sys_susp);
	sifive_smc0_set_cg(true);

	परत SBI_OK;
}

स्थिर स्थिरांक संरचना fdt_match sifive_smc0_match[] = {
	{ .compatible = "sifive,smc0" },
	{ },
};

स्थिरांक संरचना fdt_driver fdt_suspend_sifive_smc0 = {
	.match_table = sifive_smc0_match,
	.init = sifive_smc0_probe,
};
