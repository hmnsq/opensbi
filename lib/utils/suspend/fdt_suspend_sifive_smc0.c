/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 SiFive
 */

#অন্তর্ভাব <libfdt.h>
#অন্তর্ভাব <sbi/riscv_asm.h>
#অন্তর্ভাব <sbi/riscv_io.h>
#অন্তর্ভাব <sbi/sbi_console.h>
#অন্তর্ভাব <sbi/sbi_domain.h>
#অন্তর্ভাব <sbi/sbi_error.h>
#অন্তর্ভাব <sbi/sbi_hart.h>
#অন্তর্ভাব <sbi/sbi_hsm.h>
#অন্তর্ভাব <sbi/sbi_system.h>
#অন্তর্ভাব <sbi/sbi_timer.h>
#অন্তর্ভাব <sbi_utils/cache/fdt_cmo_helper.h>
#অন্তর্ভাব <sbi_utils/fdt/fdt_driver.h>
#অন্তর্ভাব <sbi_utils/fdt/fdt_helper.h>
#অন্তর্ভাব <sbi_utils/hsm/fdt_hsm_sifive_inst.h>
#অন্তর্ভাব <sbi_utils/hsm/fdt_hsm_sifive_tmc0.h>
#অন্তর্ভাব <sbi_utils/irqchip/aplic.h>
#অন্তর্ভাব <sbi_utils/timer/aclint_mtimer.h>

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

স্থির সংরচনা aclint_mtimer_data smc_sync_timer;
স্থির unsigned long smc0_base;

স্থির শূন্য sifive_smc0_set_pmcsync(বর্ণ regid, bool write_mode)
{
	unsigned long addr = smc0_base + SIFIVE_SMC_SYNC_PMC_OFF;
	u32 v = regid | SIFIVE_SMC_SYNCPMC_SYNC_REQ;

	যদি (write_mode)
		v |= SIFIVE_SMC_SYNCPMC_SYNC_WREQ;

	writel(v, (শূন্য *)addr);
	যাবত্ (!(readl((শূন্য *)addr) & SIFIVE_SMC_SYNCPMC_SYNC_ACK));
}

স্থির u64 sifive_smc0_time_read(চঞ্চল u64 *addr)
{
	u32 lo, hi;

	কুরু {
		sifive_smc0_set_pmcsync(SIFIVE_SMC_CYCLECOUNT_LO_OFF, false);
		sifive_smc0_set_pmcsync(SIFIVE_SMC_CYCLECOUNT_HI_OFF, false);
		hi = readl_relaxed((u32 *)addr + 1);
		lo = readl_relaxed((u32 *)addr);
	} যাবত্ (hi != readl_relaxed((u32 *)addr + 1));

	প্রত্যাগম ((u64)hi << 32) | (u64)lo;
}

স্থির শূন্য sifive_smc0_set_resumepc(physical_addr_t raddr)
{
	/* Set resumepc_lo */
	writel((u32)raddr, (শূন্য *)(smc0_base + SIFIVE_SMC_RESUMEPC_LO_OFF));
	/* copy resumepc_lo from SMC to PMC */
	sifive_smc0_set_pmcsync(SIFIVE_SMC_RESUMEPC_LO_OFF, true);
#যদি __riscv_xlen > 32
	/* Set resumepc_hi */
	writel((u32)(raddr >> 32), (শূন্য *)(smc0_base + SIFIVE_SMC_RESUMEPC_HI_OFF));
	/* copy resumepc_hi from SMC to PMC */
	sifive_smc0_set_pmcsync(SIFIVE_SMC_RESUMEPC_HI_OFF, true);
#endif
}

স্থির u32 sifive_smc0_get_pgprep_enarsp(শূন্য)
{
	u32 v = readl((শূন্য *)(smc0_base + SIFIVE_SMC_PGPREP_OFF));

	প্রত্যাগম v & SIFIVE_SMC_PGPREP_ENARSP;
}

স্থির শূন্য sifive_smc0_set_pgprep_disreq(শূন্য)
{
	unsigned long addr = smc0_base + SIFIVE_SMC_PGPREP_OFF;
	u32 v = readl((শূন্য *)addr);

	writel(v | SIFIVE_SMC_PGPREP_DIS_REQ, (শূন্য *)addr);
	যাবত্ (!(readl((শূন্য *)addr) & SIFIVE_SMC_PGPREP_DIS_ACK));
}

স্থির u32 sifive_smc0_set_pgprep_enareq(শূন্য)
{
	unsigned long addr = smc0_base + SIFIVE_SMC_PGPREP_OFF;
	u32 v = readl((শূন্য *)addr);

	writel(v | SIFIVE_SMC_PGPREP_ENA_REQ, (শূন্য *)addr);
	যাবত্ (!(readl((শূন্য *)addr) & SIFIVE_SMC_PGPREP_ENA_ACK));

	v = readl((শূন্য *)addr);

	প্রত্যাগম v & SIFIVE_SMC_PGPREP_ABORT;
}

স্থির শূন্য sifive_smc0_set_pg_enareq(শূন্য)
{
	unsigned long addr = smc0_base + SIFIVE_SMC_PG_OFF;
	u32 v = readl((শূন্য *)addr);

	writel(v | SIFIVE_SMC_PG_ENA_REQ, (শূন্য *)addr);
}

স্থির অন্তঃস্থ শূন্য sifive_smc0_set_cg(bool enable)
{
	unsigned long addr = smc0_base + SIFIVE_SMC_WFI_UNCORE_CG_OFF;

	যদি (enable)
		writel(0, (শূন্য *)addr);
	অন্যথা
		writel(1, (শূন্য *)addr);
}

স্থির পূর্ণাঙ্ক sifive_smc0_prep(শূন্য)
{
	ধ্রুব সংরচনা sbi_domain *dom = &মূল;
	সংরচনা sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	unsigned long i;
	পূর্ণাঙ্ক rc;
	u32 target;

	যদি (!smc0_base)
		প্রত্যাগম SBI_ENODEV;

	/* Prevent all secondary tiles from waking up from PG state */
	sbi_hartmask_for_each_hartindex(i, dom->possible_harts) {
		target = sbi_hartindex_to_hartid(i);
		যদি (target != current_hartid()) {
			rc = sifive_tmc0_set_wakemask_enareq(target);
			যদি (rc) {
				sbi_printf("Fail to enable wakemask for hart %d\n",
					   target);
				গচ্ছ fail;
			}
		}
	}

	/* Check if all secondary tiles enter PG state */
	sbi_hartmask_for_each_hartindex(i, dom->possible_harts) {
		target = sbi_hartindex_to_hartid(i);
		যদি (target != current_hartid() &&
		    !sifive_tmc0_is_pg(target)) {
			sbi_printf("Hart %d not in the PG state\n", target);
			গচ্ছ fail;
		}
	}

	rc = sifive_smc0_set_pgprep_enareq();
	যদি (rc) {
		sbi_printf("SMC0 error: abort code: 0x%x\n", rc);
		গচ্ছ fail;
	}

	rc = sifive_smc0_get_pgprep_enarsp();
	যদি (rc) {
		sifive_smc0_set_pgprep_disreq();
		sbi_printf("SMC0 error: error response code: 0x%x\n", rc);
		গচ্ছ fail;
	}

	sifive_smc0_set_resumepc(scratch->warmboot_addr);
	প্রত্যাগম SBI_OK;
fail:
	sbi_hartmask_for_each_hartindex(i, dom->possible_harts) {
		target = sbi_hartindex_to_hartid(i);
		যদি (target != current_hartid())
			sifive_tmc0_set_wakemask_disreq(target);
	}

	প্রত্যাগম SBI_EFAIL;
}

স্থির পূর্ণাঙ্ক sifive_smc0_enter(শূন্য)
{
	ধ্রুব সংরচনা sbi_domain *dom = &মূল;
	সংরচনা sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	unsigned long i;
	u32 target, rc;

	/* Flush cache and check if there is wake detect or bus error */
	যদি (fdt_cmo_llc_flush_all() &&
	    sbi_hart_has_extension(scratch, SBI_HART_EXT_XSIFIVE_CFLUSH_D_L1))
		sifive_cflush();

	rc = sifive_smc0_get_pgprep_enarsp();
	যদি (rc) {
		sbi_printf("SMC0 error: error response code: 0x%x\n", rc);
		rc = SBI_EFAIL;
		গচ্ছ fail;
	}

	যদি (sbi_hart_has_extension(scratch, SBI_HART_EXT_XSIFIVE_CEASE)) {
		sifive_smc0_set_pg_enareq();
		যাবত্ (1)
			sifive_cease();
	}

	rc = SBI_ENOTSUPP;
fail:
	sifive_smc0_set_pgprep_disreq();
	sbi_hartmask_for_each_hartindex(i, dom->possible_harts) {
		target = sbi_hartindex_to_hartid(i);
		যদি (target != current_hartid())
			sifive_tmc0_set_wakemask_disreq(target);
	}
	প্রত্যাগম rc;
}

স্থির পূর্ণাঙ্ক sifive_smc0_pg(শূন্য)
{
	পূর্ণাঙ্ক rc;

	rc = sifive_smc0_prep();
	যদি (rc)
		প্রত্যাগম rc;

	প্রত্যাগম sifive_smc0_enter();
}

স্থির শূন্য sifive_smc0_mtime_update(শূন্য)
{
	সংরচনা aclint_mtimer_data *mt = aclint_get_mtimer_data();

	aclint_mtimer_update(mt, &smc_sync_timer);
}

স্থির পূর্ণাঙ্ক sifive_smc0_system_suspend_check(u32 sleep_type)
{
	প্রত্যাগম sleep_type == SBI_SUSP_SLEEP_TYPE_SUSPEND ? SBI_OK : SBI_EINVAL;
}

স্থির পূর্ণাঙ্ক sifive_smc0_system_suspend(u32 sleep_type, unsigned long addr)
{
	/* Disable the timer interrupt */
	sbi_timer_exit(sbi_scratch_thishart_ptr());

	প্রত্যাগম sifive_smc0_pg();
}

স্থির শূন্য sifive_smc0_system_resume(শূন্য)
{
	aplic_reinit_all();
	sifive_smc0_mtime_update();
}

স্থির সংরচনা sbi_system_suspend_device smc0_sys_susp = {
	.name = "Sifive SMC0",
	.system_suspend_check = sifive_smc0_system_suspend_check,
	.system_suspend = sifive_smc0_system_suspend,
	.system_resume = sifive_smc0_system_resume,
};

স্থির পূর্ণাঙ্ক sifive_smc0_probe(ধ্রুব শূন্য *fdt, পূর্ণাঙ্ক nodeoff, ধ্রুব সংরচনা fdt_match *match)
{
	পূর্ণাঙ্ক rc;
	u64 addr;

	rc = fdt_get_গ্রন্থি_addr_size(fdt, nodeoff, 0, &addr, রিক্ত);
	যদি (rc)
		প্রত্যাগম rc;

	smc0_base = (unsigned long)addr;
	smc_sync_timer.time_rd = sifive_smc0_time_read;
	smc_sync_timer.mtime_addr = smc0_base + SIFIVE_SMC_CYCLECOUNT_LO_OFF;

	sbi_system_suspend_set_device(&smc0_sys_susp);
	sifive_smc0_set_cg(true);

	প্রত্যাগম SBI_OK;
}

স্থির ধ্রুব সংরচনা fdt_match sifive_smc0_match[] = {
	{ .compatible = "sifive,smc0" },
	{ },
};

ধ্রুব সংরচনা fdt_driver fdt_suspend_sifive_smc0 = {
	.match_table = sifive_smc0_match,
	.init = sifive_smc0_probe,
};
