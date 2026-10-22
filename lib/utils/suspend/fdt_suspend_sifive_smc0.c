/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 SiFive
 */

#ضمن <libfdt.h>
#ضمن <sbi/riscv_asm.h>
#ضمن <sbi/riscv_io.h>
#ضمن <sbi/sbi_console.h>
#ضمن <sbi/sbi_domain.h>
#ضمن <sbi/sbi_error.h>
#ضمن <sbi/sbi_hart.h>
#ضمن <sbi/sbi_hsm.h>
#ضمن <sbi/sbi_system.h>
#ضمن <sbi/sbi_timer.h>
#ضمن <sbi_utils/cache/fdt_cmo_helper.h>
#ضمن <sbi_utils/fdt/fdt_driver.h>
#ضمن <sbi_utils/fdt/fdt_helper.h>
#ضمن <sbi_utils/hsm/fdt_hsm_sifive_inst.h>
#ضمن <sbi_utils/hsm/fdt_hsm_sifive_tmc0.h>
#ضمن <sbi_utils/irqchip/aplic.h>
#ضمن <sbi_utils/timer/aclint_mtimer.h>

#تعریف_کړه SIFIVE_SMC_PGPREP_OFF			0x0
#تعریف_کړه SIFIVE_SMC_PG_OFF			0x4
#تعریف_کړه SIFIVE_SMC_CCTIMER_OFF			0xc
#تعریف_کړه SIFIVE_SMC_RESUMEPC_LO_OFF		0x10
#تعریف_کړه SIFIVE_SMC_RESUMEPC_HI_OFF		0x14
#تعریف_کړه SIFIVE_SMC_SYNC_PMC_OFF			0x24
#تعریف_کړه SIFIVE_SMC_CYCLECOUNT_LO_OFF		0x28
#تعریف_کړه SIFIVE_SMC_CYCLECOUNT_HI_OFF		0x2c
#تعریف_کړه SIFIVE_SMC_WFI_UNCORE_CG_OFF		0x50

#تعریف_کړه SIFIVE_SMC_PGPREP_ENA_REQ		بټ(31)
#تعریف_کړه SIFIVE_SMC_PGPREP_ENA_ACK		بټ(30)
#تعریف_کړه SIFIVE_SMC_PGPREP_DIS_REQ		بټ(29)
#تعریف_کړه SIFIVE_SMC_PGPREP_DIS_ACK		بټ(29)
#تعریف_کړه SIFIVE_SMC_PGPREP_FRONTNOTQ		بټ(19)
#تعریف_کړه SIFIVE_SMC_PGPREP_CLFPNOTQ		بټ(18)
#تعریف_کړه SIFIVE_SMC_PGPREP_PMCENAERR		بټ(17)
#تعریف_کړه SIFIVE_SMC_PGPREP_WAKE_DETECT		بټ(16)
#تعریف_کړه SIFIVE_SMC_PGPREP_BUSERR		بټ(15)
#تعریف_کړه SIFIVE_SMC_PGPREP_EARLY_ABORT		بټ(3)
#تعریف_کړه SIFIVE_SMC_PGPREP_دننی_ABORT	بټ(2)
#تعریف_کړه SIFIVE_SMC_PGPREP_ENARSP		(SIFIVE_SMC_PGPREP_FRONTNOTQ | \
						 SIFIVE_SMC_PGPREP_CLFPNOTQ | \
						 SIFIVE_SMC_PGPREP_PMCENAERR | \
						 SIFIVE_SMC_PGPREP_WAKE_DETECT | \
						 SIFIVE_SMC_PGPREP_BUSERR)

#تعریف_کړه SIFIVE_SMC_PGPREP_ABORT			(SIFIVE_SMC_PGPREP_EARLY_ABORT | \
						 SIFIVE_SMC_PGPREP_دننی_ABORT)

#تعریف_کړه SIFIVE_SMC_PG_ENA_REQ			بټ(31)
#تعریف_کړه SIFIVE_SMC_PG_WARM_RESET		بټ(1)

#تعریف_کړه SIFIVE_SMC_SYNCPMC_SYNC_REQ		بټ(31)
#تعریف_کړه SIFIVE_SMC_SYNCPMC_SYNC_WREQ		بټ(30)
#تعریف_کړه SIFIVE_SMC_SYNCPMC_SYNC_ACK		بټ(29)

ساكن بنية aclint_mtimer_معلومات smc_sync_timer;
ساكن بینښه اوږد smc0_بنسټ;

ساكن فراغ sifive_smc0_سېټ_pmcsync(حرف regid, bool write_ډېرپېښ)
{
	بینښه اوږد addr = smc0_بنسټ + SIFIVE_SMC_SYNC_PMC_OFF;
	u32 v = regid | SIFIVE_SMC_SYNCPMC_SYNC_REQ;

	إذا (write_ډېرپېښ)
		v |= SIFIVE_SMC_SYNCPMC_SYNC_WREQ;

	writel(v, (فراغ *)addr);
	طالما (!(readl((فراغ *)addr) & SIFIVE_SMC_SYNCPMC_SYNC_ACK));
}

ساكن u64 sifive_smc0_وخت_read(متطاير u64 *addr)
{
	u32 lo, hi;

	افعل {
		sifive_smc0_سېټ_pmcsync(SIFIVE_SMC_CYCLECOUNT_LO_OFF, false);
		sifive_smc0_سېټ_pmcsync(SIFIVE_SMC_CYCLECOUNT_HI_OFF, false);
		hi = readl_relaxed((u32 *)addr + 1);
		lo = readl_relaxed((u32 *)addr);
	} طالما (hi != readl_relaxed((u32 *)addr + 1));

	أرجع ((u64)hi << 32) | (u64)lo;
}

ساكن فراغ sifive_smc0_سېټ_resumepc(physical_addr_t raddr)
{
	/* Set resumepc_lo */
	writel((u32)raddr, (فراغ *)(smc0_بنسټ + SIFIVE_SMC_RESUMEPC_LO_OFF));
	/* copy resumepc_lo from SMC to PMC */
	sifive_smc0_سېټ_pmcsync(SIFIVE_SMC_RESUMEPC_LO_OFF, true);
#إذا __riscv_xlen > 32
	/* Set resumepc_hi */
	writel((u32)(raddr >> 32), (فراغ *)(smc0_بنسټ + SIFIVE_SMC_RESUMEPC_HI_OFF));
	/* copy resumepc_hi from SMC to PMC */
	sifive_smc0_سېټ_pmcsync(SIFIVE_SMC_RESUMEPC_HI_OFF, true);
#د_که_پای
}

ساكن u32 sifive_smc0_get_pgprep_enarsp(فراغ)
{
	u32 v = readl((فراغ *)(smc0_بنسټ + SIFIVE_SMC_PGPREP_OFF));

	أرجع v & SIFIVE_SMC_PGPREP_ENARSP;
}

ساكن فراغ sifive_smc0_سېټ_pgprep_disreq(فراغ)
{
	بینښه اوږد addr = smc0_بنسټ + SIFIVE_SMC_PGPREP_OFF;
	u32 v = readl((فراغ *)addr);

	writel(v | SIFIVE_SMC_PGPREP_DIS_REQ, (فراغ *)addr);
	طالما (!(readl((فراغ *)addr) & SIFIVE_SMC_PGPREP_DIS_ACK));
}

ساكن u32 sifive_smc0_سېټ_pgprep_enareq(فراغ)
{
	بینښه اوږد addr = smc0_بنسټ + SIFIVE_SMC_PGPREP_OFF;
	u32 v = readl((فراغ *)addr);

	writel(v | SIFIVE_SMC_PGPREP_ENA_REQ, (فراغ *)addr);
	طالما (!(readl((فراغ *)addr) & SIFIVE_SMC_PGPREP_ENA_ACK));

	v = readl((فراغ *)addr);

	أرجع v & SIFIVE_SMC_PGPREP_ABORT;
}

ساكن فراغ sifive_smc0_سېټ_pg_enareq(فراغ)
{
	بینښه اوږد addr = smc0_بنسټ + SIFIVE_SMC_PG_OFF;
	u32 v = readl((فراغ *)addr);

	writel(v | SIFIVE_SMC_PG_ENA_REQ, (فراغ *)addr);
}

ساكن مضمن فراغ sifive_smc0_سېټ_cg(bool enable)
{
	بینښه اوږد addr = smc0_بنسټ + SIFIVE_SMC_WFI_UNCORE_CG_OFF;

	إذا (enable)
		writel(0, (فراغ *)addr);
	وإلا
		writel(1, (فراغ *)addr);
}

ساكن صحيح sifive_smc0_prep(فراغ)
{
	ثابت بنية sbi_domain *dom = &ریښه;
	بنية sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	بینښه اوږد i;
	صحيح rc;
	u32 موخه;

	إذا (!smc0_بنسټ)
		أرجع SBI_ENODEV;

	/* Prevent all secondary tiles from waking up from PG state */
	sbi_hartmask_for_each_hartindex(i, dom->possible_harts) {
		موخه = sbi_hartindex_منزل_hartid(i);
		إذا (موخه != current_hartid()) {
			rc = sifive_tmc0_سېټ_wakemask_enareq(موخه);
			إذا (rc) {
				sbi_printf("Fail to enable wakemask for hart %d\n",
					   موخه);
				اذهب fail;
			}
		}
	}

	/* Check if all secondary tiles enter PG state */
	sbi_hartmask_for_each_hartindex(i, dom->possible_harts) {
		موخه = sbi_hartindex_منزل_hartid(i);
		إذا (موخه != current_hartid() &&
		    !sifive_tmc0_is_pg(موخه)) {
			sbi_printf("Hart %d not in the PG state\n", موخه);
			اذهب fail;
		}
	}

	rc = sifive_smc0_سېټ_pgprep_enareq();
	إذا (rc) {
		sbi_printf("SMC0 error: abort code: 0x%x\n", rc);
		اذهب fail;
	}

	rc = sifive_smc0_get_pgprep_enarsp();
	إذا (rc) {
		sifive_smc0_سېټ_pgprep_disreq();
		sbi_printf("SMC0 error: error response code: 0x%x\n", rc);
		اذهب fail;
	}

	sifive_smc0_سېټ_resumepc(scratch->warmboot_addr);
	أرجع SBI_OK;
fail:
	sbi_hartmask_for_each_hartindex(i, dom->possible_harts) {
		موخه = sbi_hartindex_منزل_hartid(i);
		إذا (موخه != current_hartid())
			sifive_tmc0_سېټ_wakemask_disreq(موخه);
	}

	أرجع SBI_EFAIL;
}

ساكن صحيح sifive_smc0_enter(فراغ)
{
	ثابت بنية sbi_domain *dom = &ریښه;
	بنية sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	بینښه اوږد i;
	u32 موخه, rc;

	/* Flush cache and check if there is wake detect or bus error */
	إذا (fdt_cmo_llc_flush_all() &&
	    sbi_hart_has_extension(scratch, SBI_HART_EXT_XSIFIVE_CFLUSH_D_L1))
		sifive_cflush();

	rc = sifive_smc0_get_pgprep_enarsp();
	إذا (rc) {
		sbi_printf("SMC0 error: error response code: 0x%x\n", rc);
		rc = SBI_EFAIL;
		اذهب fail;
	}

	إذا (sbi_hart_has_extension(scratch, SBI_HART_EXT_XSIFIVE_CEASE)) {
		sifive_smc0_سېټ_pg_enareq();
		طالما (1)
			sifive_cease();
	}

	rc = SBI_ENOTSUPP;
fail:
	sifive_smc0_سېټ_pgprep_disreq();
	sbi_hartmask_for_each_hartindex(i, dom->possible_harts) {
		موخه = sbi_hartindex_منزل_hartid(i);
		إذا (موخه != current_hartid())
			sifive_tmc0_سېټ_wakemask_disreq(موخه);
	}
	أرجع rc;
}

ساكن صحيح sifive_smc0_pg(فراغ)
{
	صحيح rc;

	rc = sifive_smc0_prep();
	إذا (rc)
		أرجع rc;

	أرجع sifive_smc0_enter();
}

ساكن فراغ sifive_smc0_mtime_update(فراغ)
{
	بنية aclint_mtimer_معلومات *mt = aclint_get_mtimer_معلومات();

	aclint_mtimer_update(mt, &smc_sync_timer);
}

ساكن صحيح sifive_smc0_system_suspend_check(u32 sleep_type)
{
	أرجع sleep_type == SBI_SUSP_SLEEP_TYPE_SUSPEND ? SBI_OK : SBI_EINVAL;
}

ساكن صحيح sifive_smc0_system_suspend(u32 sleep_type, بینښه اوږد addr)
{
	/* Disable the timer interrupt */
	sbi_timer_exit(sbi_scratch_thishart_ptr());

	أرجع sifive_smc0_pg();
}

ساكن فراغ sifive_smc0_system_resume(فراغ)
{
	aplic_reinit_all();
	sifive_smc0_mtime_update();
}

ساكن بنية sbi_system_suspend_device smc0_sys_susp = {
	.name = "Sifive SMC0",
	.system_suspend_check = sifive_smc0_system_suspend_check,
	.system_suspend = sifive_smc0_system_suspend,
	.system_resume = sifive_smc0_system_resume,
};

ساكن صحيح sifive_smc0_probe(ثابت فراغ *fdt, صحيح nodeoff, ثابت بنية fdt_سمون *سمون)
{
	صحيح rc;
	u64 addr;

	rc = fdt_get_غوټه_addr_اندازه(fdt, nodeoff, 0, &addr, عدم);
	إذا (rc)
		أرجع rc;

	smc0_بنسټ = (بینښه اوږد)addr;
	smc_sync_timer.وخت_rd = sifive_smc0_وخت_read;
	smc_sync_timer.mtime_addr = smc0_بنسټ + SIFIVE_SMC_CYCLECOUNT_LO_OFF;

	sbi_system_suspend_سېټ_device(&smc0_sys_susp);
	sifive_smc0_سېټ_cg(true);

	أرجع SBI_OK;
}

ساكن ثابت بنية fdt_سمون sifive_smc0_سمون[] = {
	{ .compatible = "sifive,smc0" },
	{ },
};

ثابت بنية fdt_driver fdt_suspend_sifive_smc0 = {
	.سمون_جدول = sifive_smc0_سمون,
	.init = sifive_smc0_probe,
};
