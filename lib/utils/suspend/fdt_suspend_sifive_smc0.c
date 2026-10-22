/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 SiFive
 */

#અન્તર્ભાવ <libfdt.h>
#અન્તર્ભાવ <sbi/riscv_asm.h>
#અન્તર્ભાવ <sbi/riscv_io.h>
#અન્તર્ભાવ <sbi/sbi_console.h>
#અન્તર્ભાવ <sbi/sbi_domain.h>
#અન્તર્ભાવ <sbi/sbi_error.h>
#અન્તર્ભાવ <sbi/sbi_hart.h>
#અન્તર્ભાવ <sbi/sbi_hsm.h>
#અન્તર્ભાવ <sbi/sbi_system.h>
#અન્તર્ભાવ <sbi/sbi_timer.h>
#અન્તર્ભાવ <sbi_utils/cache/fdt_cmo_helper.h>
#અન્તર્ભાવ <sbi_utils/fdt/fdt_driver.h>
#અન્તર્ભાવ <sbi_utils/fdt/fdt_helper.h>
#અન્તર્ભાવ <sbi_utils/hsm/fdt_hsm_sifive_inst.h>
#અન્તર્ભાવ <sbi_utils/hsm/fdt_hsm_sifive_tmc0.h>
#અન્તર્ભાવ <sbi_utils/irqchip/aplic.h>
#અન્તર્ભાવ <sbi_utils/timer/aclint_mtimer.h>

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

સ્થિર સંરચના aclint_mtimer_data smc_sync_timer;
સ્થિર unsigned long smc0_base;

સ્થિર શૂન્ય sifive_smc0_set_pmcsync(અક્ષર regid, bool write_mode)
{
	unsigned long addr = smc0_base + SIFIVE_SMC_SYNC_PMC_OFF;
	u32 v = regid | SIFIVE_SMC_SYNCPMC_SYNC_REQ;

	જો (write_mode)
		v |= SIFIVE_SMC_SYNCPMC_SYNC_WREQ;

	writel(v, (શૂન્ય *)addr);
	જ્યારે (!(readl((શૂન્ય *)addr) & SIFIVE_SMC_SYNCPMC_SYNC_ACK));
}

સ્થિર u64 sifive_smc0_time_read(અસ્થિર u64 *addr)
{
	u32 lo, hi;

	કરો {
		sifive_smc0_set_pmcsync(SIFIVE_SMC_CYCLECOUNT_LO_OFF, false);
		sifive_smc0_set_pmcsync(SIFIVE_SMC_CYCLECOUNT_HI_OFF, false);
		hi = readl_relaxed((u32 *)addr + 1);
		lo = readl_relaxed((u32 *)addr);
	} જ્યારે (hi != readl_relaxed((u32 *)addr + 1));

	પાછા ((u64)hi << 32) | (u64)lo;
}

સ્થિર શૂન્ય sifive_smc0_set_resumepc(physical_addr_t raddr)
{
	/* Set resumepc_lo */
	writel((u32)raddr, (શૂન્ય *)(smc0_base + SIFIVE_SMC_RESUMEPC_LO_OFF));
	/* copy resumepc_lo from SMC to PMC */
	sifive_smc0_set_pmcsync(SIFIVE_SMC_RESUMEPC_LO_OFF, true);
#જો __riscv_xlen > 32
	/* Set resumepc_hi */
	writel((u32)(raddr >> 32), (શૂન્ય *)(smc0_base + SIFIVE_SMC_RESUMEPC_HI_OFF));
	/* copy resumepc_hi from SMC to PMC */
	sifive_smc0_set_pmcsync(SIFIVE_SMC_RESUMEPC_HI_OFF, true);
#endif
}

સ્થિર u32 sifive_smc0_get_pgprep_enarsp(શૂન્ય)
{
	u32 v = readl((શૂન્ય *)(smc0_base + SIFIVE_SMC_PGPREP_OFF));

	પાછા v & SIFIVE_SMC_PGPREP_ENARSP;
}

સ્થિર શૂન્ય sifive_smc0_set_pgprep_disreq(શૂન્ય)
{
	unsigned long addr = smc0_base + SIFIVE_SMC_PGPREP_OFF;
	u32 v = readl((શૂન્ય *)addr);

	writel(v | SIFIVE_SMC_PGPREP_DIS_REQ, (શૂન્ય *)addr);
	જ્યારે (!(readl((શૂન્ય *)addr) & SIFIVE_SMC_PGPREP_DIS_ACK));
}

સ્થિર u32 sifive_smc0_set_pgprep_enareq(શૂન્ય)
{
	unsigned long addr = smc0_base + SIFIVE_SMC_PGPREP_OFF;
	u32 v = readl((શૂન્ય *)addr);

	writel(v | SIFIVE_SMC_PGPREP_ENA_REQ, (શૂન્ય *)addr);
	જ્યારે (!(readl((શૂન્ય *)addr) & SIFIVE_SMC_PGPREP_ENA_ACK));

	v = readl((શૂન્ય *)addr);

	પાછા v & SIFIVE_SMC_PGPREP_ABORT;
}

સ્થિર શૂન્ય sifive_smc0_set_pg_enareq(શૂન્ય)
{
	unsigned long addr = smc0_base + SIFIVE_SMC_PG_OFF;
	u32 v = readl((શૂન્ય *)addr);

	writel(v | SIFIVE_SMC_PG_ENA_REQ, (શૂન્ય *)addr);
}

સ્થિર અંતઃરેખ શૂન્ય sifive_smc0_set_cg(bool enable)
{
	unsigned long addr = smc0_base + SIFIVE_SMC_WFI_UNCORE_CG_OFF;

	જો (enable)
		writel(0, (શૂન્ય *)addr);
	નહીંતર
		writel(1, (શૂન્ય *)addr);
}

સ્થિર પૂર્ણાંક sifive_smc0_prep(શૂન્ય)
{
	અચલ સંરચના sbi_domain *dom = &મૂળ;
	સંરચના sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	unsigned long i;
	પૂર્ણાંક rc;
	u32 target;

	જો (!smc0_base)
		પાછા SBI_ENODEV;

	/* Prevent all secondary tiles from waking up from PG state */
	sbi_hartmask_for_each_hartindex(i, dom->possible_harts) {
		target = sbi_hartindex_to_hartid(i);
		જો (target != current_hartid()) {
			rc = sifive_tmc0_set_wakemask_enareq(target);
			જો (rc) {
				sbi_printf("Fail to enable wakemask for hart %d\n",
					   target);
				જાઓ fail;
			}
		}
	}

	/* Check if all secondary tiles enter PG state */
	sbi_hartmask_for_each_hartindex(i, dom->possible_harts) {
		target = sbi_hartindex_to_hartid(i);
		જો (target != current_hartid() &&
		    !sifive_tmc0_is_pg(target)) {
			sbi_printf("Hart %d not in the PG state\n", target);
			જાઓ fail;
		}
	}

	rc = sifive_smc0_set_pgprep_enareq();
	જો (rc) {
		sbi_printf("SMC0 error: abort code: 0x%x\n", rc);
		જાઓ fail;
	}

	rc = sifive_smc0_get_pgprep_enarsp();
	જો (rc) {
		sifive_smc0_set_pgprep_disreq();
		sbi_printf("SMC0 error: error response code: 0x%x\n", rc);
		જાઓ fail;
	}

	sifive_smc0_set_resumepc(scratch->warmboot_addr);
	પાછા SBI_OK;
fail:
	sbi_hartmask_for_each_hartindex(i, dom->possible_harts) {
		target = sbi_hartindex_to_hartid(i);
		જો (target != current_hartid())
			sifive_tmc0_set_wakemask_disreq(target);
	}

	પાછા SBI_EFAIL;
}

સ્થિર પૂર્ણાંક sifive_smc0_enter(શૂન્ય)
{
	અચલ સંરચના sbi_domain *dom = &મૂળ;
	સંરચના sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	unsigned long i;
	u32 target, rc;

	/* Flush cache and check if there is wake detect or bus error */
	જો (fdt_cmo_llc_flush_all() &&
	    sbi_hart_has_extension(scratch, SBI_HART_EXT_XSIFIVE_CFLUSH_D_L1))
		sifive_cflush();

	rc = sifive_smc0_get_pgprep_enarsp();
	જો (rc) {
		sbi_printf("SMC0 error: error response code: 0x%x\n", rc);
		rc = SBI_EFAIL;
		જાઓ fail;
	}

	જો (sbi_hart_has_extension(scratch, SBI_HART_EXT_XSIFIVE_CEASE)) {
		sifive_smc0_set_pg_enareq();
		જ્યારે (1)
			sifive_cease();
	}

	rc = SBI_ENOTSUPP;
fail:
	sifive_smc0_set_pgprep_disreq();
	sbi_hartmask_for_each_hartindex(i, dom->possible_harts) {
		target = sbi_hartindex_to_hartid(i);
		જો (target != current_hartid())
			sifive_tmc0_set_wakemask_disreq(target);
	}
	પાછા rc;
}

સ્થિર પૂર્ણાંક sifive_smc0_pg(શૂન્ય)
{
	પૂર્ણાંક rc;

	rc = sifive_smc0_prep();
	જો (rc)
		પાછા rc;

	પાછા sifive_smc0_enter();
}

સ્થિર શૂન્ય sifive_smc0_mtime_update(શૂન્ય)
{
	સંરચના aclint_mtimer_data *mt = aclint_get_mtimer_data();

	aclint_mtimer_update(mt, &smc_sync_timer);
}

સ્થિર પૂર્ણાંક sifive_smc0_system_suspend_check(u32 sleep_type)
{
	પાછા sleep_type == SBI_SUSP_SLEEP_TYPE_SUSPEND ? SBI_OK : SBI_EINVAL;
}

સ્થિર પૂર્ણાંક sifive_smc0_system_suspend(u32 sleep_type, unsigned long addr)
{
	/* Disable the timer interrupt */
	sbi_timer_exit(sbi_scratch_thishart_ptr());

	પાછા sifive_smc0_pg();
}

સ્થિર શૂન્ય sifive_smc0_system_resume(શૂન્ય)
{
	aplic_reinit_all();
	sifive_smc0_mtime_update();
}

સ્થિર સંરચના sbi_system_suspend_device smc0_sys_susp = {
	.name = "Sifive SMC0",
	.system_suspend_check = sifive_smc0_system_suspend_check,
	.system_suspend = sifive_smc0_system_suspend,
	.system_resume = sifive_smc0_system_resume,
};

સ્થિર પૂર્ણાંક sifive_smc0_probe(અચલ શૂન્ય *fdt, પૂર્ણાંક nodeoff, અચલ સંરચના fdt_match *match)
{
	પૂર્ણાંક rc;
	u64 addr;

	rc = fdt_get_ગાંઠ_addr_size(fdt, nodeoff, 0, &addr, ખાલી);
	જો (rc)
		પાછા rc;

	smc0_base = (unsigned long)addr;
	smc_sync_timer.time_rd = sifive_smc0_time_read;
	smc_sync_timer.mtime_addr = smc0_base + SIFIVE_SMC_CYCLECOUNT_LO_OFF;

	sbi_system_suspend_set_device(&smc0_sys_susp);
	sifive_smc0_set_cg(true);

	પાછા SBI_OK;
}

સ્થિર અચલ સંરચના fdt_match sifive_smc0_match[] = {
	{ .compatible = "sifive,smc0" },
	{ },
};

અચલ સંરચના fdt_driver fdt_suspend_sifive_smc0 = {
	.match_table = sifive_smc0_match,
	.init = sifive_smc0_probe,
};
