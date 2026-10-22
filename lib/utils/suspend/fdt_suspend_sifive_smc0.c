/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 SiFive
 */

#అన్తర్భావ <libfdt.h>
#అన్తర్భావ <sbi/riscv_asm.h>
#అన్తర్భావ <sbi/riscv_io.h>
#అన్తర్భావ <sbi/sbi_console.h>
#అన్తర్భావ <sbi/sbi_domain.h>
#అన్తర్భావ <sbi/sbi_error.h>
#అన్తర్భావ <sbi/sbi_hart.h>
#అన్తర్భావ <sbi/sbi_hsm.h>
#అన్తర్భావ <sbi/sbi_system.h>
#అన్తర్భావ <sbi/sbi_timer.h>
#అన్తర్భావ <sbi_utils/cache/fdt_cmo_helper.h>
#అన్తర్భావ <sbi_utils/fdt/fdt_driver.h>
#అన్తర్భావ <sbi_utils/fdt/fdt_helper.h>
#అన్తర్భావ <sbi_utils/hsm/fdt_hsm_sifive_inst.h>
#అన్తర్భావ <sbi_utils/hsm/fdt_hsm_sifive_tmc0.h>
#అన్తర్భావ <sbi_utils/irqchip/aplic.h>
#అన్తర్భావ <sbi_utils/timer/aclint_mtimer.h>

#నిర్వచించు SIFIVE_SMC_PGPREP_OFF			0x0
#నిర్వచించు SIFIVE_SMC_PG_OFF			0x4
#నిర్వచించు SIFIVE_SMC_CCTIMER_OFF			0xc
#నిర్వచించు SIFIVE_SMC_RESUMEPC_LO_OFF		0x10
#నిర్వచించు SIFIVE_SMC_RESUMEPC_HI_OFF		0x14
#నిర్వచించు SIFIVE_SMC_SYNC_PMC_OFF			0x24
#నిర్వచించు SIFIVE_SMC_CYCLECOUNT_LO_OFF		0x28
#నిర్వచించు SIFIVE_SMC_CYCLECOUNT_HI_OFF		0x2c
#నిర్వచించు SIFIVE_SMC_WFI_UNCORE_CG_OFF		0x50

#నిర్వచించు SIFIVE_SMC_PGPREP_ENA_REQ		బిట్టు(31)
#నిర్వచించు SIFIVE_SMC_PGPREP_ENA_ACK		బిట్టు(30)
#నిర్వచించు SIFIVE_SMC_PGPREP_DIS_REQ		బిట్టు(29)
#నిర్వచించు SIFIVE_SMC_PGPREP_DIS_ACK		బిట్టు(29)
#నిర్వచించు SIFIVE_SMC_PGPREP_FRONTNOTQ		బిట్టు(19)
#నిర్వచించు SIFIVE_SMC_PGPREP_CLFPNOTQ		బిట్టు(18)
#నిర్వచించు SIFIVE_SMC_PGPREP_PMCENAERR		బిట్టు(17)
#నిర్వచించు SIFIVE_SMC_PGPREP_WAKE_DETECT		బిట్టు(16)
#నిర్వచించు SIFIVE_SMC_PGPREP_BUSERR		బిట్టు(15)
#నిర్వచించు SIFIVE_SMC_PGPREP_EARLY_ABORT		బిట్టు(3)
#నిర్వచించు SIFIVE_SMC_PGPREP_అంతర్గత_ABORT	బిట్టు(2)
#నిర్వచించు SIFIVE_SMC_PGPREP_ENARSP		(SIFIVE_SMC_PGPREP_FRONTNOTQ | \
						 SIFIVE_SMC_PGPREP_CLFPNOTQ | \
						 SIFIVE_SMC_PGPREP_PMCENAERR | \
						 SIFIVE_SMC_PGPREP_WAKE_DETECT | \
						 SIFIVE_SMC_PGPREP_BUSERR)

#నిర్వచించు SIFIVE_SMC_PGPREP_ABORT			(SIFIVE_SMC_PGPREP_EARLY_ABORT | \
						 SIFIVE_SMC_PGPREP_అంతర్గత_ABORT)

#నిర్వచించు SIFIVE_SMC_PG_ENA_REQ			బిట్టు(31)
#నిర్వచించు SIFIVE_SMC_PG_WARM_RESET		బిట్టు(1)

#నిర్వచించు SIFIVE_SMC_SYNCPMC_SYNC_REQ		బిట్టు(31)
#నిర్వచించు SIFIVE_SMC_SYNCPMC_SYNC_WREQ		బిట్టు(30)
#నిర్వచించు SIFIVE_SMC_SYNCPMC_SYNC_ACK		బిట్టు(29)

స్థిర నిర్మాణం aclint_mtimer_దత్తాంశం smc_sync_timer;
స్థిర గుర్తులేని దీర్ఘ smc0_పునాది;

స్థిర శూన్య sifive_smc0_సమితి_pmcsync(అక్షరం regid, bool write_బహుళకం)
{
	గుర్తులేని దీర్ఘ addr = smc0_పునాది + SIFIVE_SMC_SYNC_PMC_OFF;
	u32 v = regid | SIFIVE_SMC_SYNCPMC_SYNC_REQ;

	ఒకవేళ (write_బహుళకం)
		v |= SIFIVE_SMC_SYNCPMC_SYNC_WREQ;

	writel(v, (శూన్య *)addr);
	అయితే (!(readl((శూన్య *)addr) & SIFIVE_SMC_SYNCPMC_SYNC_ACK));
}

స్థిర u64 sifive_smc0_సమయం_read(అస్థిర u64 *addr)
{
	u32 lo, hi;

	చేయి {
		sifive_smc0_సమితి_pmcsync(SIFIVE_SMC_CYCLECOUNT_LO_OFF, false);
		sifive_smc0_సమితి_pmcsync(SIFIVE_SMC_CYCLECOUNT_HI_OFF, false);
		hi = readl_relaxed((u32 *)addr + 1);
		lo = readl_relaxed((u32 *)addr);
	} అయితే (hi != readl_relaxed((u32 *)addr + 1));

	తిరిగి ((u64)hi << 32) | (u64)lo;
}

స్థిర శూన్య sifive_smc0_సమితి_resumepc(physical_addr_t raddr)
{
	/* Set resumepc_lo */
	writel((u32)raddr, (శూన్య *)(smc0_పునాది + SIFIVE_SMC_RESUMEPC_LO_OFF));
	/* copy resumepc_lo from SMC to PMC */
	sifive_smc0_సమితి_pmcsync(SIFIVE_SMC_RESUMEPC_LO_OFF, true);
#ఒకవేళ __riscv_xlen > 32
	/* Set resumepc_hi */
	writel((u32)(raddr >> 32), (శూన్య *)(smc0_పునాది + SIFIVE_SMC_RESUMEPC_HI_OFF));
	/* copy resumepc_hi from SMC to PMC */
	sifive_smc0_సమితి_pmcsync(SIFIVE_SMC_RESUMEPC_HI_OFF, true);
#అయితే_ముగింపు
}

స్థిర u32 sifive_smc0_get_pgprep_enarsp(శూన్య)
{
	u32 v = readl((శూన్య *)(smc0_పునాది + SIFIVE_SMC_PGPREP_OFF));

	తిరిగి v & SIFIVE_SMC_PGPREP_ENARSP;
}

స్థిర శూన్య sifive_smc0_సమితి_pgprep_disreq(శూన్య)
{
	గుర్తులేని దీర్ఘ addr = smc0_పునాది + SIFIVE_SMC_PGPREP_OFF;
	u32 v = readl((శూన్య *)addr);

	writel(v | SIFIVE_SMC_PGPREP_DIS_REQ, (శూన్య *)addr);
	అయితే (!(readl((శూన్య *)addr) & SIFIVE_SMC_PGPREP_DIS_ACK));
}

స్థిర u32 sifive_smc0_సమితి_pgprep_enareq(శూన్య)
{
	గుర్తులేని దీర్ఘ addr = smc0_పునాది + SIFIVE_SMC_PGPREP_OFF;
	u32 v = readl((శూన్య *)addr);

	writel(v | SIFIVE_SMC_PGPREP_ENA_REQ, (శూన్య *)addr);
	అయితే (!(readl((శూన్య *)addr) & SIFIVE_SMC_PGPREP_ENA_ACK));

	v = readl((శూన్య *)addr);

	తిరిగి v & SIFIVE_SMC_PGPREP_ABORT;
}

స్థిర శూన్య sifive_smc0_సమితి_pg_enareq(శూన్య)
{
	గుర్తులేని దీర్ఘ addr = smc0_పునాది + SIFIVE_SMC_PG_OFF;
	u32 v = readl((శూన్య *)addr);

	writel(v | SIFIVE_SMC_PG_ENA_REQ, (శూన్య *)addr);
}

స్థిర అంతఃవరుస శూన్య sifive_smc0_సమితి_cg(bool enable)
{
	గుర్తులేని దీర్ఘ addr = smc0_పునాది + SIFIVE_SMC_WFI_UNCORE_CG_OFF;

	ఒకవేళ (enable)
		writel(0, (శూన్య *)addr);
	లేకపోతే
		writel(1, (శూన్య *)addr);
}

స్థిర పూర్ణాంకం sifive_smc0_prep(శూన్య)
{
	స్థిరరాశి నిర్మాణం sbi_domain *dom = &మూలం;
	నిర్మాణం sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	గుర్తులేని దీర్ఘ i;
	పూర్ణాంకం rc;
	u32 లక్ష్యం;

	ఒకవేళ (!smc0_పునాది)
		తిరిగి SBI_ENODEV;

	/* Prevent all secondary tiles from waking up from PG state */
	sbi_hartmask_for_each_hartindex(i, dom->possible_harts) {
		లక్ష్యం = sbi_hartindex_గమ్యం_hartid(i);
		ఒకవేళ (లక్ష్యం != current_hartid()) {
			rc = sifive_tmc0_సమితి_wakemask_enareq(లక్ష్యం);
			ఒకవేళ (rc) {
				sbi_printf("Fail to enable wakemask for hart %d\n",
					   లక్ష్యం);
				వెళ్ళు fail;
			}
		}
	}

	/* Check if all secondary tiles enter PG state */
	sbi_hartmask_for_each_hartindex(i, dom->possible_harts) {
		లక్ష్యం = sbi_hartindex_గమ్యం_hartid(i);
		ఒకవేళ (లక్ష్యం != current_hartid() &&
		    !sifive_tmc0_is_pg(లక్ష్యం)) {
			sbi_printf("Hart %d not in the PG state\n", లక్ష్యం);
			వెళ్ళు fail;
		}
	}

	rc = sifive_smc0_సమితి_pgprep_enareq();
	ఒకవేళ (rc) {
		sbi_printf("SMC0 error: abort code: 0x%x\n", rc);
		వెళ్ళు fail;
	}

	rc = sifive_smc0_get_pgprep_enarsp();
	ఒకవేళ (rc) {
		sifive_smc0_సమితి_pgprep_disreq();
		sbi_printf("SMC0 error: error response code: 0x%x\n", rc);
		వెళ్ళు fail;
	}

	sifive_smc0_సమితి_resumepc(scratch->warmboot_addr);
	తిరిగి SBI_OK;
fail:
	sbi_hartmask_for_each_hartindex(i, dom->possible_harts) {
		లక్ష్యం = sbi_hartindex_గమ్యం_hartid(i);
		ఒకవేళ (లక్ష్యం != current_hartid())
			sifive_tmc0_సమితి_wakemask_disreq(లక్ష్యం);
	}

	తిరిగి SBI_EFAIL;
}

స్థిర పూర్ణాంకం sifive_smc0_enter(శూన్య)
{
	స్థిరరాశి నిర్మాణం sbi_domain *dom = &మూలం;
	నిర్మాణం sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	గుర్తులేని దీర్ఘ i;
	u32 లక్ష్యం, rc;

	/* Flush cache and check if there is wake detect or bus error */
	ఒకవేళ (fdt_cmo_llc_flush_all() &&
	    sbi_hart_has_extension(scratch, SBI_HART_EXT_XSIFIVE_CFLUSH_D_L1))
		sifive_cflush();

	rc = sifive_smc0_get_pgprep_enarsp();
	ఒకవేళ (rc) {
		sbi_printf("SMC0 error: error response code: 0x%x\n", rc);
		rc = SBI_EFAIL;
		వెళ్ళు fail;
	}

	ఒకవేళ (sbi_hart_has_extension(scratch, SBI_HART_EXT_XSIFIVE_CEASE)) {
		sifive_smc0_సమితి_pg_enareq();
		అయితే (1)
			sifive_cease();
	}

	rc = SBI_ENOTSUPP;
fail:
	sifive_smc0_సమితి_pgprep_disreq();
	sbi_hartmask_for_each_hartindex(i, dom->possible_harts) {
		లక్ష్యం = sbi_hartindex_గమ్యం_hartid(i);
		ఒకవేళ (లక్ష్యం != current_hartid())
			sifive_tmc0_సమితి_wakemask_disreq(లక్ష్యం);
	}
	తిరిగి rc;
}

స్థిర పూర్ణాంకం sifive_smc0_pg(శూన్య)
{
	పూర్ణాంకం rc;

	rc = sifive_smc0_prep();
	ఒకవేళ (rc)
		తిరిగి rc;

	తిరిగి sifive_smc0_enter();
}

స్థిర శూన్య sifive_smc0_mtime_update(శూన్య)
{
	నిర్మాణం aclint_mtimer_దత్తాంశం *mt = aclint_get_mtimer_దత్తాంశం();

	aclint_mtimer_update(mt, &smc_sync_timer);
}

స్థిర పూర్ణాంకం sifive_smc0_system_suspend_check(u32 sleep_type)
{
	తిరిగి sleep_type == SBI_SUSP_SLEEP_TYPE_SUSPEND ? SBI_OK : SBI_EINVAL;
}

స్థిర పూర్ణాంకం sifive_smc0_system_suspend(u32 sleep_type, గుర్తులేని దీర్ఘ addr)
{
	/* Disable the timer interrupt */
	sbi_timer_exit(sbi_scratch_thishart_ptr());

	తిరిగి sifive_smc0_pg();
}

స్థిర శూన్య sifive_smc0_system_resume(శూన్య)
{
	aplic_reinit_all();
	sifive_smc0_mtime_update();
}

స్థిర నిర్మాణం sbi_system_suspend_device smc0_sys_susp = {
	.name = "Sifive SMC0",
	.system_suspend_check = sifive_smc0_system_suspend_check,
	.system_suspend = sifive_smc0_system_suspend,
	.system_resume = sifive_smc0_system_resume,
};

స్థిర పూర్ణాంకం sifive_smc0_probe(స్థిరరాశి శూన్య *fdt, పూర్ణాంకం nodeoff, స్థిరరాశి నిర్మాణం fdt_సరిపోలిక *సరిపోలిక)
{
	పూర్ణాంకం rc;
	u64 addr;

	rc = fdt_get_కణుపు_addr_కొలత(fdt, nodeoff, 0, &addr, ఖాళీ);
	ఒకవేళ (rc)
		తిరిగి rc;

	smc0_పునాది = (గుర్తులేని దీర్ఘ)addr;
	smc_sync_timer.సమయం_rd = sifive_smc0_సమయం_read;
	smc_sync_timer.mtime_addr = smc0_పునాది + SIFIVE_SMC_CYCLECOUNT_LO_OFF;

	sbi_system_suspend_సమితి_device(&smc0_sys_susp);
	sifive_smc0_సమితి_cg(true);

	తిరిగి SBI_OK;
}

స్థిర స్థిరరాశి నిర్మాణం fdt_సరిపోలిక sifive_smc0_సరిపోలిక[] = {
	{ .compatible = "sifive,smc0" },
	{ },
};

స్థిరరాశి నిర్మాణం fdt_driver fdt_suspend_sifive_smc0 = {
	.సరిపోలిక_పట్టిక = sifive_smc0_సరిపోలిక,
	.init = sifive_smc0_probe,
};
