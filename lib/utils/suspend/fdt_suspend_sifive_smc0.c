/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 SiFive
 */

#समावेश <libfdt.h>
#समावेश <sbi/riscv_asm.h>
#समावेश <sbi/riscv_io.h>
#समावेश <sbi/sbi_console.h>
#समावेश <sbi/sbi_domain.h>
#समावेश <sbi/sbi_error.h>
#समावेश <sbi/sbi_hart.h>
#समावेश <sbi/sbi_hsm.h>
#समावेश <sbi/sbi_system.h>
#समावेश <sbi/sbi_timer.h>
#समावेश <sbi_utils/cache/fdt_cmo_helper.h>
#समावेश <sbi_utils/fdt/fdt_driver.h>
#समावेश <sbi_utils/fdt/fdt_helper.h>
#समावेश <sbi_utils/hsm/fdt_hsm_sifive_inst.h>
#समावेश <sbi_utils/hsm/fdt_hsm_sifive_tmc0.h>
#समावेश <sbi_utils/irqchip/aplic.h>
#समावेश <sbi_utils/timer/aclint_mtimer.h>

#घोषणा SIFIVE_SMC_PGPREP_OFF			0x0
#घोषणा SIFIVE_SMC_PG_OFF			0x4
#घोषणा SIFIVE_SMC_CCTIMER_OFF			0xc
#घोषणा SIFIVE_SMC_RESUMEPC_LO_OFF		0x10
#घोषणा SIFIVE_SMC_RESUMEPC_HI_OFF		0x14
#घोषणा SIFIVE_SMC_SYNC_PMC_OFF			0x24
#घोषणा SIFIVE_SMC_CYCLECOUNT_LO_OFF		0x28
#घोषणा SIFIVE_SMC_CYCLECOUNT_HI_OFF		0x2c
#घोषणा SIFIVE_SMC_WFI_UNCORE_CG_OFF		0x50

#घोषणा SIFIVE_SMC_PGPREP_ENA_REQ		BIT(31)
#घोषणा SIFIVE_SMC_PGPREP_ENA_ACK		BIT(30)
#घोषणा SIFIVE_SMC_PGPREP_DIS_REQ		BIT(29)
#घोषणा SIFIVE_SMC_PGPREP_DIS_ACK		BIT(29)
#घोषणा SIFIVE_SMC_PGPREP_FRONTNOTQ		BIT(19)
#घोषणा SIFIVE_SMC_PGPREP_CLFPNOTQ		BIT(18)
#घोषणा SIFIVE_SMC_PGPREP_PMCENAERR		BIT(17)
#घोषणा SIFIVE_SMC_PGPREP_WAKE_DETECT		BIT(16)
#घोषणा SIFIVE_SMC_PGPREP_BUSERR		BIT(15)
#घोषणा SIFIVE_SMC_PGPREP_EARLY_ABORT		BIT(3)
#घोषणा SIFIVE_SMC_PGPREP_INTERNAL_ABORT	BIT(2)
#घोषणा SIFIVE_SMC_PGPREP_ENARSP		(SIFIVE_SMC_PGPREP_FRONTNOTQ | \
						 SIFIVE_SMC_PGPREP_CLFPNOTQ | \
						 SIFIVE_SMC_PGPREP_PMCENAERR | \
						 SIFIVE_SMC_PGPREP_WAKE_DETECT | \
						 SIFIVE_SMC_PGPREP_BUSERR)

#घोषणा SIFIVE_SMC_PGPREP_ABORT			(SIFIVE_SMC_PGPREP_EARLY_ABORT | \
						 SIFIVE_SMC_PGPREP_INTERNAL_ABORT)

#घोषणा SIFIVE_SMC_PG_ENA_REQ			BIT(31)
#घोषणा SIFIVE_SMC_PG_WARM_RESET		BIT(1)

#घोषणा SIFIVE_SMC_SYNCPMC_SYNC_REQ		BIT(31)
#घोषणा SIFIVE_SMC_SYNCPMC_SYNC_WREQ		BIT(30)
#घोषणा SIFIVE_SMC_SYNCPMC_SYNC_ACK		BIT(29)

अटल काष्ठा aclint_mtimer_तथ्य smc_sync_timer;
अटल अचिन्हित दीर्घ smc0_base;

अटल व्योम sifive_smc0_set_pmcsync(अक्षर regid, bool write_mode)
{
	अचिन्हित दीर्घ addr = smc0_base + SIFIVE_SMC_SYNC_PMC_OFF;
	u32 v = regid | SIFIVE_SMC_SYNCPMC_SYNC_REQ;

	अगर (write_mode)
		v |= SIFIVE_SMC_SYNCPMC_SYNC_WREQ;

	writel(v, (व्योम *)addr);
	जबतक (!(readl((व्योम *)addr) & SIFIVE_SMC_SYNCPMC_SYNC_ACK));
}

अटल u64 sifive_smc0_time_read(अस्थिर u64 *addr)
{
	u32 lo, hi;

	करो {
		sifive_smc0_set_pmcsync(SIFIVE_SMC_CYCLECOUNT_LO_OFF, false);
		sifive_smc0_set_pmcsync(SIFIVE_SMC_CYCLECOUNT_HI_OFF, false);
		hi = readl_relaxed((u32 *)addr + 1);
		lo = readl_relaxed((u32 *)addr);
	} जबतक (hi != readl_relaxed((u32 *)addr + 1));

	वापस ((u64)hi << 32) | (u64)lo;
}

अटल व्योम sifive_smc0_set_resumepc(physical_addr_t raddr)
{
	/* Set resumepc_lo */
	writel((u32)raddr, (व्योम *)(smc0_base + SIFIVE_SMC_RESUMEPC_LO_OFF));
	/* copy resumepc_lo from SMC to PMC */
	sifive_smc0_set_pmcsync(SIFIVE_SMC_RESUMEPC_LO_OFF, true);
#अगर __riscv_xlen > 32
	/* Set resumepc_hi */
	writel((u32)(raddr >> 32), (व्योम *)(smc0_base + SIFIVE_SMC_RESUMEPC_HI_OFF));
	/* copy resumepc_hi from SMC to PMC */
	sifive_smc0_set_pmcsync(SIFIVE_SMC_RESUMEPC_HI_OFF, true);
#पूर्ण_अगर
}

अटल u32 sifive_smc0_get_pgprep_enarsp(व्योम)
{
	u32 v = readl((व्योम *)(smc0_base + SIFIVE_SMC_PGPREP_OFF));

	वापस v & SIFIVE_SMC_PGPREP_ENARSP;
}

अटल व्योम sifive_smc0_set_pgprep_disreq(व्योम)
{
	अचिन्हित दीर्घ addr = smc0_base + SIFIVE_SMC_PGPREP_OFF;
	u32 v = readl((व्योम *)addr);

	writel(v | SIFIVE_SMC_PGPREP_DIS_REQ, (व्योम *)addr);
	जबतक (!(readl((व्योम *)addr) & SIFIVE_SMC_PGPREP_DIS_ACK));
}

अटल u32 sifive_smc0_set_pgprep_enareq(व्योम)
{
	अचिन्हित दीर्घ addr = smc0_base + SIFIVE_SMC_PGPREP_OFF;
	u32 v = readl((व्योम *)addr);

	writel(v | SIFIVE_SMC_PGPREP_ENA_REQ, (व्योम *)addr);
	जबतक (!(readl((व्योम *)addr) & SIFIVE_SMC_PGPREP_ENA_ACK));

	v = readl((व्योम *)addr);

	वापस v & SIFIVE_SMC_PGPREP_ABORT;
}

अटल व्योम sifive_smc0_set_pg_enareq(व्योम)
{
	अचिन्हित दीर्घ addr = smc0_base + SIFIVE_SMC_PG_OFF;
	u32 v = readl((व्योम *)addr);

	writel(v | SIFIVE_SMC_PG_ENA_REQ, (व्योम *)addr);
}

अटल अंतरभूत व्योम sifive_smc0_set_cg(bool enable)
{
	अचिन्हित दीर्घ addr = smc0_base + SIFIVE_SMC_WFI_UNCORE_CG_OFF;

	अगर (enable)
		writel(0, (व्योम *)addr);
	अन्यथा
		writel(1, (व्योम *)addr);
}

अटल पूर्णांक sifive_smc0_prep(व्योम)
{
	स्थिर काष्ठा sbi_domain *dom = &मूल;
	काष्ठा sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	अचिन्हित दीर्घ i;
	पूर्णांक rc;
	u32 target;

	अगर (!smc0_base)
		वापस SBI_ENODEV;

	/* Prevent all secondary tiles from waking up from PG state */
	sbi_hartmask_for_each_hartindex(i, dom->possible_harts) {
		target = sbi_hartindex_to_hartid(i);
		अगर (target != current_hartid()) {
			rc = sifive_tmc0_set_wakemask_enareq(target);
			अगर (rc) {
				sbi_printf("Fail to enable wakemask for hart %d\n",
					   target);
				जाओ fail;
			}
		}
	}

	/* Check if all secondary tiles enter PG state */
	sbi_hartmask_for_each_hartindex(i, dom->possible_harts) {
		target = sbi_hartindex_to_hartid(i);
		अगर (target != current_hartid() &&
		    !sifive_tmc0_is_pg(target)) {
			sbi_printf("Hart %d not in the PG state\n", target);
			जाओ fail;
		}
	}

	rc = sifive_smc0_set_pgprep_enareq();
	अगर (rc) {
		sbi_printf("SMC0 error: abort code: 0x%x\n", rc);
		जाओ fail;
	}

	rc = sifive_smc0_get_pgprep_enarsp();
	अगर (rc) {
		sifive_smc0_set_pgprep_disreq();
		sbi_printf("SMC0 error: error response code: 0x%x\n", rc);
		जाओ fail;
	}

	sifive_smc0_set_resumepc(scratch->warmboot_addr);
	वापस SBI_OK;
fail:
	sbi_hartmask_for_each_hartindex(i, dom->possible_harts) {
		target = sbi_hartindex_to_hartid(i);
		अगर (target != current_hartid())
			sifive_tmc0_set_wakemask_disreq(target);
	}

	वापस SBI_EFAIL;
}

अटल पूर्णांक sifive_smc0_enter(व्योम)
{
	स्थिर काष्ठा sbi_domain *dom = &मूल;
	काष्ठा sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	अचिन्हित दीर्घ i;
	u32 target, rc;

	/* Flush cache and check if there is wake detect or bus error */
	अगर (fdt_cmo_llc_flush_all() &&
	    sbi_hart_has_extension(scratch, SBI_HART_EXT_XSIFIVE_CFLUSH_D_L1))
		sifive_cflush();

	rc = sifive_smc0_get_pgprep_enarsp();
	अगर (rc) {
		sbi_printf("SMC0 error: error response code: 0x%x\n", rc);
		rc = SBI_EFAIL;
		जाओ fail;
	}

	अगर (sbi_hart_has_extension(scratch, SBI_HART_EXT_XSIFIVE_CEASE)) {
		sifive_smc0_set_pg_enareq();
		जबतक (1)
			sifive_cease();
	}

	rc = SBI_ENOTSUPP;
fail:
	sifive_smc0_set_pgprep_disreq();
	sbi_hartmask_for_each_hartindex(i, dom->possible_harts) {
		target = sbi_hartindex_to_hartid(i);
		अगर (target != current_hartid())
			sifive_tmc0_set_wakemask_disreq(target);
	}
	वापस rc;
}

अटल पूर्णांक sifive_smc0_pg(व्योम)
{
	पूर्णांक rc;

	rc = sifive_smc0_prep();
	अगर (rc)
		वापस rc;

	वापस sifive_smc0_enter();
}

अटल व्योम sifive_smc0_mtime_update(व्योम)
{
	काष्ठा aclint_mtimer_तथ्य *mt = aclint_get_mtimer_तथ्य();

	aclint_mtimer_update(mt, &smc_sync_timer);
}

अटल पूर्णांक sifive_smc0_system_suspend_check(u32 sleep_type)
{
	वापस sleep_type == SBI_SUSP_SLEEP_TYPE_SUSPEND ? SBI_OK : SBI_EINVAL;
}

अटल पूर्णांक sifive_smc0_system_suspend(u32 sleep_type, अचिन्हित दीर्घ addr)
{
	/* Disable the timer interrupt */
	sbi_timer_exit(sbi_scratch_thishart_ptr());

	वापस sifive_smc0_pg();
}

अटल व्योम sifive_smc0_system_resume(व्योम)
{
	aplic_reinit_all();
	sifive_smc0_mtime_update();
}

अटल काष्ठा sbi_system_suspend_device smc0_sys_susp = {
	.name = "Sifive SMC0",
	.system_suspend_check = sifive_smc0_system_suspend_check,
	.system_suspend = sifive_smc0_system_suspend,
	.system_resume = sifive_smc0_system_resume,
};

अटल पूर्णांक sifive_smc0_probe(स्थिर व्योम *fdt, पूर्णांक nodeoff, स्थिर काष्ठा fdt_match *match)
{
	पूर्णांक rc;
	u64 addr;

	rc = fdt_get_गाँठ_addr_आकार(fdt, nodeoff, 0, &addr, शून्य);
	अगर (rc)
		वापस rc;

	smc0_base = (अचिन्हित दीर्घ)addr;
	smc_sync_timer.time_rd = sifive_smc0_time_read;
	smc_sync_timer.mtime_addr = smc0_base + SIFIVE_SMC_CYCLECOUNT_LO_OFF;

	sbi_system_suspend_set_device(&smc0_sys_susp);
	sifive_smc0_set_cg(true);

	वापस SBI_OK;
}

अटल स्थिर काष्ठा fdt_match sifive_smc0_match[] = {
	{ .compatible = "sifive,smc0" },
	{ },
};

स्थिर काष्ठा fdt_driver fdt_suspend_sifive_smc0 = {
	.match_table = sifive_smc0_match,
	.init = sifive_smc0_probe,
};
