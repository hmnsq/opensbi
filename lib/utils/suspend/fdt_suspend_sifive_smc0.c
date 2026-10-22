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
स्थिर unsigned long smc0_base;

स्थिर शून्य sifive_smc0_set_pmcsync(वर्ण regid, bool write_mode)
{
	unsigned long addr = smc0_base + SIFIVE_SMC_SYNC_PMC_OFF;
	u32 v = regid | SIFIVE_SMC_SYNCPMC_SYNC_REQ;

	यदि (write_mode)
		v |= SIFIVE_SMC_SYNCPMC_SYNC_WREQ;

	writel(v, (शून्य *)addr);
	यावत् (!(readl((शून्य *)addr) & SIFIVE_SMC_SYNCPMC_SYNC_ACK));
}

स्थिर u64 sifive_smc0_time_read(चञ्चल u64 *addr)
{
	u32 lo, hi;

	कुरु {
		sifive_smc0_set_pmcsync(SIFIVE_SMC_CYCLECOUNT_LO_OFF, false);
		sifive_smc0_set_pmcsync(SIFIVE_SMC_CYCLECOUNT_HI_OFF, false);
		hi = readl_relaxed((u32 *)addr + 1);
		lo = readl_relaxed((u32 *)addr);
	} यावत् (hi != readl_relaxed((u32 *)addr + 1));

	प्रत्यागम ((u64)hi << 32) | (u64)lo;
}

स्थिर शून्य sifive_smc0_set_resumepc(physical_addr_t raddr)
{
	/* Set resumepc_lo */
	writel((u32)raddr, (शून्य *)(smc0_base + SIFIVE_SMC_RESUMEPC_LO_OFF));
	/* copy resumepc_lo from SMC to PMC */
	sifive_smc0_set_pmcsync(SIFIVE_SMC_RESUMEPC_LO_OFF, true);
#यदि __riscv_xlen > 32
	/* Set resumepc_hi */
	writel((u32)(raddr >> 32), (शून्य *)(smc0_base + SIFIVE_SMC_RESUMEPC_HI_OFF));
	/* copy resumepc_hi from SMC to PMC */
	sifive_smc0_set_pmcsync(SIFIVE_SMC_RESUMEPC_HI_OFF, true);
#endif
}

स्थिर u32 sifive_smc0_get_pgprep_enarsp(शून्य)
{
	u32 v = readl((शून्य *)(smc0_base + SIFIVE_SMC_PGPREP_OFF));

	प्रत्यागम v & SIFIVE_SMC_PGPREP_ENARSP;
}

स्थिर शून्य sifive_smc0_set_pgprep_disreq(शून्य)
{
	unsigned long addr = smc0_base + SIFIVE_SMC_PGPREP_OFF;
	u32 v = readl((शून्य *)addr);

	writel(v | SIFIVE_SMC_PGPREP_DIS_REQ, (शून्य *)addr);
	यावत् (!(readl((शून्य *)addr) & SIFIVE_SMC_PGPREP_DIS_ACK));
}

स्थिर u32 sifive_smc0_set_pgprep_enareq(शून्य)
{
	unsigned long addr = smc0_base + SIFIVE_SMC_PGPREP_OFF;
	u32 v = readl((शून्य *)addr);

	writel(v | SIFIVE_SMC_PGPREP_ENA_REQ, (शून्य *)addr);
	यावत् (!(readl((शून्य *)addr) & SIFIVE_SMC_PGPREP_ENA_ACK));

	v = readl((शून्य *)addr);

	प्रत्यागम v & SIFIVE_SMC_PGPREP_ABORT;
}

स्थिर शून्य sifive_smc0_set_pg_enareq(शून्य)
{
	unsigned long addr = smc0_base + SIFIVE_SMC_PG_OFF;
	u32 v = readl((शून्य *)addr);

	writel(v | SIFIVE_SMC_PG_ENA_REQ, (शून्य *)addr);
}

स्थिर अन्तःस्थ शून्य sifive_smc0_set_cg(bool enable)
{
	unsigned long addr = smc0_base + SIFIVE_SMC_WFI_UNCORE_CG_OFF;

	यदि (enable)
		writel(0, (शून्य *)addr);
	अन्यथा
		writel(1, (शून्य *)addr);
}

स्थिर पूर्णाङ्क sifive_smc0_prep(शून्य)
{
	ध्रुव संरचना sbi_domain *dom = &मूल;
	संरचना sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	unsigned long i;
	पूर्णाङ्क rc;
	u32 target;

	यदि (!smc0_base)
		प्रत्यागम SBI_ENODEV;

	/* Prevent all secondary tiles from waking up from PG state */
	sbi_hartmask_for_each_hartindex(i, dom->possible_harts) {
		target = sbi_hartindex_to_hartid(i);
		यदि (target != current_hartid()) {
			rc = sifive_tmc0_set_wakemask_enareq(target);
			यदि (rc) {
				sbi_printf("Fail to enable wakemask for hart %d\n",
					   target);
				गच्छ fail;
			}
		}
	}

	/* Check if all secondary tiles enter PG state */
	sbi_hartmask_for_each_hartindex(i, dom->possible_harts) {
		target = sbi_hartindex_to_hartid(i);
		यदि (target != current_hartid() &&
		    !sifive_tmc0_is_pg(target)) {
			sbi_printf("Hart %d not in the PG state\n", target);
			गच्छ fail;
		}
	}

	rc = sifive_smc0_set_pgprep_enareq();
	यदि (rc) {
		sbi_printf("SMC0 error: abort code: 0x%x\n", rc);
		गच्छ fail;
	}

	rc = sifive_smc0_get_pgprep_enarsp();
	यदि (rc) {
		sifive_smc0_set_pgprep_disreq();
		sbi_printf("SMC0 error: error response code: 0x%x\n", rc);
		गच्छ fail;
	}

	sifive_smc0_set_resumepc(scratch->warmboot_addr);
	प्रत्यागम SBI_OK;
fail:
	sbi_hartmask_for_each_hartindex(i, dom->possible_harts) {
		target = sbi_hartindex_to_hartid(i);
		यदि (target != current_hartid())
			sifive_tmc0_set_wakemask_disreq(target);
	}

	प्रत्यागम SBI_EFAIL;
}

स्थिर पूर्णाङ्क sifive_smc0_enter(शून्य)
{
	ध्रुव संरचना sbi_domain *dom = &मूल;
	संरचना sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	unsigned long i;
	u32 target, rc;

	/* Flush cache and check if there is wake detect or bus error */
	यदि (fdt_cmo_llc_flush_all() &&
	    sbi_hart_has_extension(scratch, SBI_HART_EXT_XSIFIVE_CFLUSH_D_L1))
		sifive_cflush();

	rc = sifive_smc0_get_pgprep_enarsp();
	यदि (rc) {
		sbi_printf("SMC0 error: error response code: 0x%x\n", rc);
		rc = SBI_EFAIL;
		गच्छ fail;
	}

	यदि (sbi_hart_has_extension(scratch, SBI_HART_EXT_XSIFIVE_CEASE)) {
		sifive_smc0_set_pg_enareq();
		यावत् (1)
			sifive_cease();
	}

	rc = SBI_ENOTSUPP;
fail:
	sifive_smc0_set_pgprep_disreq();
	sbi_hartmask_for_each_hartindex(i, dom->possible_harts) {
		target = sbi_hartindex_to_hartid(i);
		यदि (target != current_hartid())
			sifive_tmc0_set_wakemask_disreq(target);
	}
	प्रत्यागम rc;
}

स्थिर पूर्णाङ्क sifive_smc0_pg(शून्य)
{
	पूर्णाङ्क rc;

	rc = sifive_smc0_prep();
	यदि (rc)
		प्रत्यागम rc;

	प्रत्यागम sifive_smc0_enter();
}

स्थिर शून्य sifive_smc0_mtime_update(शून्य)
{
	संरचना aclint_mtimer_data *mt = aclint_get_mtimer_data();

	aclint_mtimer_update(mt, &smc_sync_timer);
}

स्थिर पूर्णाङ्क sifive_smc0_system_suspend_check(u32 sleep_type)
{
	प्रत्यागम sleep_type == SBI_SUSP_SLEEP_TYPE_SUSPEND ? SBI_OK : SBI_EINVAL;
}

स्थिर पूर्णाङ्क sifive_smc0_system_suspend(u32 sleep_type, unsigned long addr)
{
	/* Disable the timer interrupt */
	sbi_timer_exit(sbi_scratch_thishart_ptr());

	प्रत्यागम sifive_smc0_pg();
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

स्थिर पूर्णाङ्क sifive_smc0_probe(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoff, ध्रुव संरचना fdt_match *match)
{
	पूर्णाङ्क rc;
	u64 addr;

	rc = fdt_get_गाँठ_addr_size(fdt, nodeoff, 0, &addr, रिक्त);
	यदि (rc)
		प्रत्यागम rc;

	smc0_base = (unsigned long)addr;
	smc_sync_timer.time_rd = sifive_smc0_time_read;
	smc_sync_timer.mtime_addr = smc0_base + SIFIVE_SMC_CYCLECOUNT_LO_OFF;

	sbi_system_suspend_set_device(&smc0_sys_susp);
	sifive_smc0_set_cg(true);

	प्रत्यागम SBI_OK;
}

स्थिर ध्रुव संरचना fdt_match sifive_smc0_match[] = {
	{ .compatible = "sifive,smc0" },
	{ },
};

ध्रुव संरचना fdt_driver fdt_suspend_sifive_smc0 = {
	.match_table = sifive_smc0_match,
	.init = sifive_smc0_probe,
};
