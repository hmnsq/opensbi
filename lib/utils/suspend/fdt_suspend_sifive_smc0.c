/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 SiFive
 */

#അന്തര്ഭാവ <libfdt.h>
#അന്തര്ഭാവ <sbi/riscv_asm.h>
#അന്തര്ഭാവ <sbi/riscv_io.h>
#അന്തര്ഭാവ <sbi/sbi_console.h>
#അന്തര്ഭാവ <sbi/sbi_domain.h>
#അന്തര്ഭാവ <sbi/sbi_error.h>
#അന്തര്ഭാവ <sbi/sbi_hart.h>
#അന്തര്ഭാവ <sbi/sbi_hsm.h>
#അന്തര്ഭാവ <sbi/sbi_system.h>
#അന്തര്ഭാവ <sbi/sbi_timer.h>
#അന്തര്ഭാവ <sbi_utils/cache/fdt_cmo_helper.h>
#അന്തര്ഭാവ <sbi_utils/fdt/fdt_driver.h>
#അന്തര്ഭാവ <sbi_utils/fdt/fdt_helper.h>
#അന്തര്ഭാവ <sbi_utils/hsm/fdt_hsm_sifive_inst.h>
#അന്തര്ഭാവ <sbi_utils/hsm/fdt_hsm_sifive_tmc0.h>
#അന്തര്ഭാവ <sbi_utils/irqchip/aplic.h>
#അന്തര്ഭാവ <sbi_utils/timer/aclint_mtimer.h>

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

സ്ഥിതം ഘടന aclint_mtimer_data smc_sync_timer;
സ്ഥിതം unsigned long smc0_base;

സ്ഥിതം ശൂന്യം sifive_smc0_set_pmcsync(അക്ഷരം regid, bool write_mode)
{
	unsigned long addr = smc0_base + SIFIVE_SMC_SYNC_PMC_OFF;
	u32 v = regid | SIFIVE_SMC_SYNCPMC_SYNC_REQ;

	എങ്കിൽ (write_mode)
		v |= SIFIVE_SMC_SYNCPMC_SYNC_WREQ;

	writel(v, (ശൂന്യം *)addr);
	ആകുമ്പോൾ (!(readl((ശൂന്യം *)addr) & SIFIVE_SMC_SYNCPMC_SYNC_ACK));
}

സ്ഥിതം u64 sifive_smc0_time_read(അസ്ഥിര u64 *addr)
{
	u32 lo, hi;

	ചെയ്യുക {
		sifive_smc0_set_pmcsync(SIFIVE_SMC_CYCLECOUNT_LO_OFF, false);
		sifive_smc0_set_pmcsync(SIFIVE_SMC_CYCLECOUNT_HI_OFF, false);
		hi = readl_relaxed((u32 *)addr + 1);
		lo = readl_relaxed((u32 *)addr);
	} ആകുമ്പോൾ (hi != readl_relaxed((u32 *)addr + 1));

	തിരികെ ((u64)hi << 32) | (u64)lo;
}

സ്ഥിതം ശൂന്യം sifive_smc0_set_resumepc(physical_addr_t raddr)
{
	/* Set resumepc_lo */
	writel((u32)raddr, (ശൂന്യം *)(smc0_base + SIFIVE_SMC_RESUMEPC_LO_OFF));
	/* copy resumepc_lo from SMC to PMC */
	sifive_smc0_set_pmcsync(SIFIVE_SMC_RESUMEPC_LO_OFF, true);
#എങ്കിൽ __riscv_xlen > 32
	/* Set resumepc_hi */
	writel((u32)(raddr >> 32), (ശൂന്യം *)(smc0_base + SIFIVE_SMC_RESUMEPC_HI_OFF));
	/* copy resumepc_hi from SMC to PMC */
	sifive_smc0_set_pmcsync(SIFIVE_SMC_RESUMEPC_HI_OFF, true);
#endif
}

സ്ഥിതം u32 sifive_smc0_get_pgprep_enarsp(ശൂന്യം)
{
	u32 v = readl((ശൂന്യം *)(smc0_base + SIFIVE_SMC_PGPREP_OFF));

	തിരികെ v & SIFIVE_SMC_PGPREP_ENARSP;
}

സ്ഥിതം ശൂന്യം sifive_smc0_set_pgprep_disreq(ശൂന്യം)
{
	unsigned long addr = smc0_base + SIFIVE_SMC_PGPREP_OFF;
	u32 v = readl((ശൂന്യം *)addr);

	writel(v | SIFIVE_SMC_PGPREP_DIS_REQ, (ശൂന്യം *)addr);
	ആകുമ്പോൾ (!(readl((ശൂന്യം *)addr) & SIFIVE_SMC_PGPREP_DIS_ACK));
}

സ്ഥിതം u32 sifive_smc0_set_pgprep_enareq(ശൂന്യം)
{
	unsigned long addr = smc0_base + SIFIVE_SMC_PGPREP_OFF;
	u32 v = readl((ശൂന്യം *)addr);

	writel(v | SIFIVE_SMC_PGPREP_ENA_REQ, (ശൂന്യം *)addr);
	ആകുമ്പോൾ (!(readl((ശൂന്യം *)addr) & SIFIVE_SMC_PGPREP_ENA_ACK));

	v = readl((ശൂന്യം *)addr);

	തിരികെ v & SIFIVE_SMC_PGPREP_ABORT;
}

സ്ഥിതം ശൂന്യം sifive_smc0_set_pg_enareq(ശൂന്യം)
{
	unsigned long addr = smc0_base + SIFIVE_SMC_PG_OFF;
	u32 v = readl((ശൂന്യം *)addr);

	writel(v | SIFIVE_SMC_PG_ENA_REQ, (ശൂന്യം *)addr);
}

സ്ഥിതം വരിയിൽ ശൂന്യം sifive_smc0_set_cg(bool enable)
{
	unsigned long addr = smc0_base + SIFIVE_SMC_WFI_UNCORE_CG_OFF;

	എങ്കിൽ (enable)
		writel(0, (ശൂന്യം *)addr);
	അല്ലെങ്കിൽ
		writel(1, (ശൂന്യം *)addr);
}

സ്ഥിതം പൂർണ്ണസംഖ്യ sifive_smc0_prep(ശൂന്യം)
{
	സ്ഥിരം ഘടന sbi_domain *dom = &വേര്;
	ഘടന sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	unsigned long i;
	പൂർണ്ണസംഖ്യ rc;
	u32 target;

	എങ്കിൽ (!smc0_base)
		തിരികെ SBI_ENODEV;

	/* Prevent all secondary tiles from waking up from PG state */
	sbi_hartmask_for_each_hartindex(i, dom->possible_harts) {
		target = sbi_hartindex_to_hartid(i);
		എങ്കിൽ (target != current_hartid()) {
			rc = sifive_tmc0_set_wakemask_enareq(target);
			എങ്കിൽ (rc) {
				sbi_printf("Fail to enable wakemask for hart %d\n",
					   target);
				പോകുക fail;
			}
		}
	}

	/* Check if all secondary tiles enter PG state */
	sbi_hartmask_for_each_hartindex(i, dom->possible_harts) {
		target = sbi_hartindex_to_hartid(i);
		എങ്കിൽ (target != current_hartid() &&
		    !sifive_tmc0_is_pg(target)) {
			sbi_printf("Hart %d not in the PG state\n", target);
			പോകുക fail;
		}
	}

	rc = sifive_smc0_set_pgprep_enareq();
	എങ്കിൽ (rc) {
		sbi_printf("SMC0 error: abort code: 0x%x\n", rc);
		പോകുക fail;
	}

	rc = sifive_smc0_get_pgprep_enarsp();
	എങ്കിൽ (rc) {
		sifive_smc0_set_pgprep_disreq();
		sbi_printf("SMC0 error: error response code: 0x%x\n", rc);
		പോകുക fail;
	}

	sifive_smc0_set_resumepc(scratch->warmboot_addr);
	തിരികെ SBI_OK;
fail:
	sbi_hartmask_for_each_hartindex(i, dom->possible_harts) {
		target = sbi_hartindex_to_hartid(i);
		എങ്കിൽ (target != current_hartid())
			sifive_tmc0_set_wakemask_disreq(target);
	}

	തിരികെ SBI_EFAIL;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ sifive_smc0_enter(ശൂന്യം)
{
	സ്ഥിരം ഘടന sbi_domain *dom = &വേര്;
	ഘടന sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	unsigned long i;
	u32 target, rc;

	/* Flush cache and check if there is wake detect or bus error */
	എങ്കിൽ (fdt_cmo_llc_flush_all() &&
	    sbi_hart_has_extension(scratch, SBI_HART_EXT_XSIFIVE_CFLUSH_D_L1))
		sifive_cflush();

	rc = sifive_smc0_get_pgprep_enarsp();
	എങ്കിൽ (rc) {
		sbi_printf("SMC0 error: error response code: 0x%x\n", rc);
		rc = SBI_EFAIL;
		പോകുക fail;
	}

	എങ്കിൽ (sbi_hart_has_extension(scratch, SBI_HART_EXT_XSIFIVE_CEASE)) {
		sifive_smc0_set_pg_enareq();
		ആകുമ്പോൾ (1)
			sifive_cease();
	}

	rc = SBI_ENOTSUPP;
fail:
	sifive_smc0_set_pgprep_disreq();
	sbi_hartmask_for_each_hartindex(i, dom->possible_harts) {
		target = sbi_hartindex_to_hartid(i);
		എങ്കിൽ (target != current_hartid())
			sifive_tmc0_set_wakemask_disreq(target);
	}
	തിരികെ rc;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ sifive_smc0_pg(ശൂന്യം)
{
	പൂർണ്ണസംഖ്യ rc;

	rc = sifive_smc0_prep();
	എങ്കിൽ (rc)
		തിരികെ rc;

	തിരികെ sifive_smc0_enter();
}

സ്ഥിതം ശൂന്യം sifive_smc0_mtime_update(ശൂന്യം)
{
	ഘടന aclint_mtimer_data *mt = aclint_get_mtimer_data();

	aclint_mtimer_update(mt, &smc_sync_timer);
}

സ്ഥിതം പൂർണ്ണസംഖ്യ sifive_smc0_system_suspend_check(u32 sleep_type)
{
	തിരികെ sleep_type == SBI_SUSP_SLEEP_TYPE_SUSPEND ? SBI_OK : SBI_EINVAL;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ sifive_smc0_system_suspend(u32 sleep_type, unsigned long addr)
{
	/* Disable the timer interrupt */
	sbi_timer_exit(sbi_scratch_thishart_ptr());

	തിരികെ sifive_smc0_pg();
}

സ്ഥിതം ശൂന്യം sifive_smc0_system_resume(ശൂന്യം)
{
	aplic_reinit_all();
	sifive_smc0_mtime_update();
}

സ്ഥിതം ഘടന sbi_system_suspend_device smc0_sys_susp = {
	.name = "Sifive SMC0",
	.system_suspend_check = sifive_smc0_system_suspend_check,
	.system_suspend = sifive_smc0_system_suspend,
	.system_resume = sifive_smc0_system_resume,
};

സ്ഥിതം പൂർണ്ണസംഖ്യ sifive_smc0_probe(സ്ഥിരം ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ nodeoff, സ്ഥിരം ഘടന fdt_match *match)
{
	പൂർണ്ണസംഖ്യ rc;
	u64 addr;

	rc = fdt_get_കണ്ണി_addr_size(fdt, nodeoff, 0, &addr, ശൂന്യ);
	എങ്കിൽ (rc)
		തിരികെ rc;

	smc0_base = (unsigned long)addr;
	smc_sync_timer.time_rd = sifive_smc0_time_read;
	smc_sync_timer.mtime_addr = smc0_base + SIFIVE_SMC_CYCLECOUNT_LO_OFF;

	sbi_system_suspend_set_device(&smc0_sys_susp);
	sifive_smc0_set_cg(true);

	തിരികെ SBI_OK;
}

സ്ഥിതം സ്ഥിരം ഘടന fdt_match sifive_smc0_match[] = {
	{ .compatible = "sifive,smc0" },
	{ },
};

സ്ഥിരം ഘടന fdt_driver fdt_suspend_sifive_smc0 = {
	.match_table = sifive_smc0_match,
	.init = sifive_smc0_probe,
};
