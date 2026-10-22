/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 SiFive
 */

#شامل <libfdt.h>
#شامل <sbi/riscv_asm.h>
#شامل <sbi/riscv_io.h>
#شامل <sbi/sbi_console.h>
#شامل <sbi/sbi_domain.h>
#شامل <sbi/sbi_error.h>
#شامل <sbi/sbi_hart.h>
#شامل <sbi/sbi_hsm.h>
#شامل <sbi/sbi_system.h>
#شامل <sbi/sbi_timer.h>
#شامل <sbi_utils/cache/fdt_cmo_helper.h>
#شامل <sbi_utils/fdt/fdt_driver.h>
#شامل <sbi_utils/fdt/fdt_helper.h>
#شامل <sbi_utils/hsm/fdt_hsm_sifive_inst.h>
#شامل <sbi_utils/hsm/fdt_hsm_sifive_tmc0.h>
#شامل <sbi_utils/irqchip/aplic.h>
#شامل <sbi_utils/timer/aclint_mtimer.h>

#تعریف_کرو SIFIVE_SMC_PGPREP_OFF			0x0
#تعریف_کرو SIFIVE_SMC_PG_OFF			0x4
#تعریف_کرو SIFIVE_SMC_CCTIMER_OFF			0xc
#تعریف_کرو SIFIVE_SMC_RESUMEPC_LO_OFF		0x10
#تعریف_کرو SIFIVE_SMC_RESUMEPC_HI_OFF		0x14
#تعریف_کرو SIFIVE_SMC_SYNC_PMC_OFF			0x24
#تعریف_کرو SIFIVE_SMC_CYCLECOUNT_LO_OFF		0x28
#تعریف_کرو SIFIVE_SMC_CYCLECOUNT_HI_OFF		0x2c
#تعریف_کرو SIFIVE_SMC_WFI_UNCORE_CG_OFF		0x50

#تعریف_کرو SIFIVE_SMC_PGPREP_ENA_REQ		ثنائی_ہندسہ(31)
#تعریف_کرو SIFIVE_SMC_PGPREP_ENA_ACK		ثنائی_ہندسہ(30)
#تعریف_کرو SIFIVE_SMC_PGPREP_DIS_REQ		ثنائی_ہندسہ(29)
#تعریف_کرو SIFIVE_SMC_PGPREP_DIS_ACK		ثنائی_ہندسہ(29)
#تعریف_کرو SIFIVE_SMC_PGPREP_FRONTNOTQ		ثنائی_ہندسہ(19)
#تعریف_کرو SIFIVE_SMC_PGPREP_CLFPNOTQ		ثنائی_ہندسہ(18)
#تعریف_کرو SIFIVE_SMC_PGPREP_PMCENAERR		ثنائی_ہندسہ(17)
#تعریف_کرو SIFIVE_SMC_PGPREP_WAKE_DETECT		ثنائی_ہندسہ(16)
#تعریف_کرو SIFIVE_SMC_PGPREP_BUSERR		ثنائی_ہندسہ(15)
#تعریف_کرو SIFIVE_SMC_PGPREP_EARLY_ABORT		ثنائی_ہندسہ(3)
#تعریف_کرو SIFIVE_SMC_PGPREP_اندرونی_ABORT	ثنائی_ہندسہ(2)
#تعریف_کرو SIFIVE_SMC_PGPREP_ENARSP		(SIFIVE_SMC_PGPREP_FRONTNOTQ | \
						 SIFIVE_SMC_PGPREP_CLFPNOTQ | \
						 SIFIVE_SMC_PGPREP_PMCENAERR | \
						 SIFIVE_SMC_PGPREP_WAKE_DETECT | \
						 SIFIVE_SMC_PGPREP_BUSERR)

#تعریف_کرو SIFIVE_SMC_PGPREP_ABORT			(SIFIVE_SMC_PGPREP_EARLY_ABORT | \
						 SIFIVE_SMC_PGPREP_اندرونی_ABORT)

#تعریف_کرو SIFIVE_SMC_PG_ENA_REQ			ثنائی_ہندسہ(31)
#تعریف_کرو SIFIVE_SMC_PG_WARM_RESET		ثنائی_ہندسہ(1)

#تعریف_کرو SIFIVE_SMC_SYNCPMC_SYNC_REQ		ثنائی_ہندسہ(31)
#تعریف_کرو SIFIVE_SMC_SYNCPMC_SYNC_WREQ		ثنائی_ہندسہ(30)
#تعریف_کرو SIFIVE_SMC_SYNCPMC_SYNC_ACK		ثنائی_ہندسہ(29)

ساکن ساخت aclint_mtimer_ڈیٹا smc_sync_timer;
ساکن غیرعلامتی طویل smc0_بنیاد;

ساکن خالی sifive_smc0_سیٹ_pmcsync(حرف regid, bool write_منوال)
{
	غیرعلامتی طویل addr = smc0_بنیاد + SIFIVE_SMC_SYNC_PMC_OFF;
	u32 v = regid | SIFIVE_SMC_SYNCPMC_SYNC_REQ;

	اگر (write_منوال)
		v |= SIFIVE_SMC_SYNCPMC_SYNC_WREQ;

	writel(v, (خالی *)addr);
	جب (!(readl((خالی *)addr) & SIFIVE_SMC_SYNCPMC_SYNC_ACK));
}

ساکن u64 sifive_smc0_وقت_read(متلون u64 *addr)
{
	u32 lo, hi;

	کرو {
		sifive_smc0_سیٹ_pmcsync(SIFIVE_SMC_CYCLECOUNT_LO_OFF, false);
		sifive_smc0_سیٹ_pmcsync(SIFIVE_SMC_CYCLECOUNT_HI_OFF, false);
		hi = readl_relaxed((u32 *)addr + 1);
		lo = readl_relaxed((u32 *)addr);
	} جب (hi != readl_relaxed((u32 *)addr + 1));

	واپس ((u64)hi << 32) | (u64)lo;
}

ساکن خالی sifive_smc0_سیٹ_resumepc(physical_addr_t raddr)
{
	/* Set resumepc_lo */
	writel((u32)raddr, (خالی *)(smc0_بنیاد + SIFIVE_SMC_RESUMEPC_LO_OFF));
	/* copy resumepc_lo from SMC to PMC */
	sifive_smc0_سیٹ_pmcsync(SIFIVE_SMC_RESUMEPC_LO_OFF, true);
#اگر __riscv_xlen > 32
	/* Set resumepc_hi */
	writel((u32)(raddr >> 32), (خالی *)(smc0_بنیاد + SIFIVE_SMC_RESUMEPC_HI_OFF));
	/* copy resumepc_hi from SMC to PMC */
	sifive_smc0_سیٹ_pmcsync(SIFIVE_SMC_RESUMEPC_HI_OFF, true);
#اگر_ختم
}

ساکن u32 sifive_smc0_get_pgprep_enarsp(خالی)
{
	u32 v = readl((خالی *)(smc0_بنیاد + SIFIVE_SMC_PGPREP_OFF));

	واپس v & SIFIVE_SMC_PGPREP_ENARSP;
}

ساکن خالی sifive_smc0_سیٹ_pgprep_disreq(خالی)
{
	غیرعلامتی طویل addr = smc0_بنیاد + SIFIVE_SMC_PGPREP_OFF;
	u32 v = readl((خالی *)addr);

	writel(v | SIFIVE_SMC_PGPREP_DIS_REQ, (خالی *)addr);
	جب (!(readl((خالی *)addr) & SIFIVE_SMC_PGPREP_DIS_ACK));
}

ساکن u32 sifive_smc0_سیٹ_pgprep_enareq(خالی)
{
	غیرعلامتی طویل addr = smc0_بنیاد + SIFIVE_SMC_PGPREP_OFF;
	u32 v = readl((خالی *)addr);

	writel(v | SIFIVE_SMC_PGPREP_ENA_REQ, (خالی *)addr);
	جب (!(readl((خالی *)addr) & SIFIVE_SMC_PGPREP_ENA_ACK));

	v = readl((خالی *)addr);

	واپس v & SIFIVE_SMC_PGPREP_ABORT;
}

ساکن خالی sifive_smc0_سیٹ_pg_enareq(خالی)
{
	غیرعلامتی طویل addr = smc0_بنیاد + SIFIVE_SMC_PG_OFF;
	u32 v = readl((خالی *)addr);

	writel(v | SIFIVE_SMC_PG_ENA_REQ, (خالی *)addr);
}

ساکن اندرونی خالی sifive_smc0_سیٹ_cg(bool enable)
{
	غیرعلامتی طویل addr = smc0_بنیاد + SIFIVE_SMC_WFI_UNCORE_CG_OFF;

	اگر (enable)
		writel(0, (خالی *)addr);
	ورنہ
		writel(1, (خالی *)addr);
}

ساکن صحیح sifive_smc0_prep(خالی)
{
	ثابت ساخت sbi_domain *dom = &جڑ;
	ساخت sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	غیرعلامتی طویل i;
	صحیح rc;
	u32 ہدف;

	اگر (!smc0_بنیاد)
		واپس SBI_ENODEV;

	/* Prevent all secondary tiles from waking up from PG state */
	sbi_hartmask_for_each_hartindex(i, dom->possible_harts) {
		ہدف = sbi_hartindex_مقصد_hartid(i);
		اگر (ہدف != current_hartid()) {
			rc = sifive_tmc0_سیٹ_wakemask_enareq(ہدف);
			اگر (rc) {
				sbi_printf("Fail to enable wakemask for hart %d\n",
					   ہدف);
				جاؤ fail;
			}
		}
	}

	/* Check if all secondary tiles enter PG state */
	sbi_hartmask_for_each_hartindex(i, dom->possible_harts) {
		ہدف = sbi_hartindex_مقصد_hartid(i);
		اگر (ہدف != current_hartid() &&
		    !sifive_tmc0_is_pg(ہدف)) {
			sbi_printf("Hart %d not in the PG state\n", ہدف);
			جاؤ fail;
		}
	}

	rc = sifive_smc0_سیٹ_pgprep_enareq();
	اگر (rc) {
		sbi_printf("SMC0 error: abort code: 0x%x\n", rc);
		جاؤ fail;
	}

	rc = sifive_smc0_get_pgprep_enarsp();
	اگر (rc) {
		sifive_smc0_سیٹ_pgprep_disreq();
		sbi_printf("SMC0 error: error response code: 0x%x\n", rc);
		جاؤ fail;
	}

	sifive_smc0_سیٹ_resumepc(scratch->warmboot_addr);
	واپس SBI_OK;
fail:
	sbi_hartmask_for_each_hartindex(i, dom->possible_harts) {
		ہدف = sbi_hartindex_مقصد_hartid(i);
		اگر (ہدف != current_hartid())
			sifive_tmc0_سیٹ_wakemask_disreq(ہدف);
	}

	واپس SBI_EFAIL;
}

ساکن صحیح sifive_smc0_enter(خالی)
{
	ثابت ساخت sbi_domain *dom = &جڑ;
	ساخت sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	غیرعلامتی طویل i;
	u32 ہدف, rc;

	/* Flush cache and check if there is wake detect or bus error */
	اگر (fdt_cmo_llc_flush_all() &&
	    sbi_hart_has_extension(scratch, SBI_HART_EXT_XSIFIVE_CFLUSH_D_L1))
		sifive_cflush();

	rc = sifive_smc0_get_pgprep_enarsp();
	اگر (rc) {
		sbi_printf("SMC0 error: error response code: 0x%x\n", rc);
		rc = SBI_EFAIL;
		جاؤ fail;
	}

	اگر (sbi_hart_has_extension(scratch, SBI_HART_EXT_XSIFIVE_CEASE)) {
		sifive_smc0_سیٹ_pg_enareq();
		جب (1)
			sifive_cease();
	}

	rc = SBI_ENOTSUPP;
fail:
	sifive_smc0_سیٹ_pgprep_disreq();
	sbi_hartmask_for_each_hartindex(i, dom->possible_harts) {
		ہدف = sbi_hartindex_مقصد_hartid(i);
		اگر (ہدف != current_hartid())
			sifive_tmc0_سیٹ_wakemask_disreq(ہدف);
	}
	واپس rc;
}

ساکن صحیح sifive_smc0_pg(خالی)
{
	صحیح rc;

	rc = sifive_smc0_prep();
	اگر (rc)
		واپس rc;

	واپس sifive_smc0_enter();
}

ساکن خالی sifive_smc0_mtime_update(خالی)
{
	ساخت aclint_mtimer_ڈیٹا *mt = aclint_get_mtimer_ڈیٹا();

	aclint_mtimer_update(mt, &smc_sync_timer);
}

ساکن صحیح sifive_smc0_system_suspend_check(u32 sleep_type)
{
	واپس sleep_type == SBI_SUSP_SLEEP_TYPE_SUSPEND ? SBI_OK : SBI_EINVAL;
}

ساکن صحیح sifive_smc0_system_suspend(u32 sleep_type, غیرعلامتی طویل addr)
{
	/* Disable the timer interrupt */
	sbi_timer_exit(sbi_scratch_thishart_ptr());

	واپس sifive_smc0_pg();
}

ساکن خالی sifive_smc0_system_resume(خالی)
{
	aplic_reinit_all();
	sifive_smc0_mtime_update();
}

ساکن ساخت sbi_system_suspend_device smc0_sys_susp = {
	.name = "Sifive SMC0",
	.system_suspend_check = sifive_smc0_system_suspend_check,
	.system_suspend = sifive_smc0_system_suspend,
	.system_resume = sifive_smc0_system_resume,
};

ساکن صحیح sifive_smc0_probe(ثابت خالی *fdt, صحیح nodeoff, ثابت ساخت fdt_مطابقت *مطابقت)
{
	صحیح rc;
	u64 addr;

	rc = fdt_get_گرہ_addr_سائز(fdt, nodeoff, 0, &addr, تہی);
	اگر (rc)
		واپس rc;

	smc0_بنیاد = (غیرعلامتی طویل)addr;
	smc_sync_timer.وقت_rd = sifive_smc0_وقت_read;
	smc_sync_timer.mtime_addr = smc0_بنیاد + SIFIVE_SMC_CYCLECOUNT_LO_OFF;

	sbi_system_suspend_سیٹ_device(&smc0_sys_susp);
	sifive_smc0_سیٹ_cg(true);

	واپس SBI_OK;
}

ساکن ثابت ساخت fdt_مطابقت sifive_smc0_مطابقت[] = {
	{ .compatible = "sifive,smc0" },
	{ },
};

ثابت ساخت fdt_driver fdt_suspend_sifive_smc0 = {
	.مطابقت_جدول = sifive_smc0_مطابقت,
	.init = sifive_smc0_probe,
};
