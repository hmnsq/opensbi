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

ساكن بنية aclint_mtimer_انکڑے smc_sync_timer;
ساكن بنانشانی لما smc0_ادھار;

ساكن فراغ sifive_smc0_سیٹ_pmcsync(حرف regid, bool write_منوال)
{
	بنانشانی لما addr = smc0_ادھار + SIFIVE_SMC_SYNC_PMC_OFF;
	u32 v = regid | SIFIVE_SMC_SYNCPMC_SYNC_REQ;

	إذا (write_منوال)
		v |= SIFIVE_SMC_SYNCPMC_SYNC_WREQ;

	writel(v, (فراغ *)addr);
	طالما (!(readl((فراغ *)addr) & SIFIVE_SMC_SYNCPMC_SYNC_ACK));
}

ساكن u64 sifive_smc0_ویلا_read(متطاير u64 *addr)
{
	u32 lo, hi;

	افعل {
		sifive_smc0_سیٹ_pmcsync(SIFIVE_SMC_CYCLECOUNT_LO_OFF, false);
		sifive_smc0_سیٹ_pmcsync(SIFIVE_SMC_CYCLECOUNT_HI_OFF, false);
		hi = readl_relaxed((u32 *)addr + 1);
		lo = readl_relaxed((u32 *)addr);
	} طالما (hi != readl_relaxed((u32 *)addr + 1));

	أرجع ((u64)hi << 32) | (u64)lo;
}

ساكن فراغ sifive_smc0_سیٹ_resumepc(physical_addr_t raddr)
{
	/* Set resumepc_lo */
	writel((u32)raddr, (فراغ *)(smc0_ادھار + SIFIVE_SMC_RESUMEPC_LO_OFF));
	/* copy resumepc_lo from SMC to PMC */
	sifive_smc0_سیٹ_pmcsync(SIFIVE_SMC_RESUMEPC_LO_OFF, true);
#إذا __riscv_xlen > 32
	/* Set resumepc_hi */
	writel((u32)(raddr >> 32), (فراغ *)(smc0_ادھار + SIFIVE_SMC_RESUMEPC_HI_OFF));
	/* copy resumepc_hi from SMC to PMC */
	sifive_smc0_سیٹ_pmcsync(SIFIVE_SMC_RESUMEPC_HI_OFF, true);
#جے_انت
}

ساكن u32 sifive_smc0_get_pgprep_enarsp(فراغ)
{
	u32 v = readl((فراغ *)(smc0_ادھار + SIFIVE_SMC_PGPREP_OFF));

	أرجع v & SIFIVE_SMC_PGPREP_ENARSP;
}

ساكن فراغ sifive_smc0_سیٹ_pgprep_disreq(فراغ)
{
	بنانشانی لما addr = smc0_ادھار + SIFIVE_SMC_PGPREP_OFF;
	u32 v = readl((فراغ *)addr);

	writel(v | SIFIVE_SMC_PGPREP_DIS_REQ, (فراغ *)addr);
	طالما (!(readl((فراغ *)addr) & SIFIVE_SMC_PGPREP_DIS_ACK));
}

ساكن u32 sifive_smc0_سیٹ_pgprep_enareq(فراغ)
{
	بنانشانی لما addr = smc0_ادھار + SIFIVE_SMC_PGPREP_OFF;
	u32 v = readl((فراغ *)addr);

	writel(v | SIFIVE_SMC_PGPREP_ENA_REQ, (فراغ *)addr);
	طالما (!(readl((فراغ *)addr) & SIFIVE_SMC_PGPREP_ENA_ACK));

	v = readl((فراغ *)addr);

	أرجع v & SIFIVE_SMC_PGPREP_ABORT;
}

ساكن فراغ sifive_smc0_سیٹ_pg_enareq(فراغ)
{
	بنانشانی لما addr = smc0_ادھار + SIFIVE_SMC_PG_OFF;
	u32 v = readl((فراغ *)addr);

	writel(v | SIFIVE_SMC_PG_ENA_REQ, (فراغ *)addr);
}

ساكن مضمن فراغ sifive_smc0_سیٹ_cg(bool enable)
{
	بنانشانی لما addr = smc0_ادھار + SIFIVE_SMC_WFI_UNCORE_CG_OFF;

	إذا (enable)
		writel(0, (فراغ *)addr);
	وإلا
		writel(1, (فراغ *)addr);
}

ساكن صحيح sifive_smc0_prep(فراغ)
{
	ثابت بنية sbi_domain *dom = &جڑ;
	بنية sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	بنانشانی لما i;
	صحيح rc;
	u32 نشانہ;

	إذا (!smc0_ادھار)
		أرجع SBI_ENODEV;

	/* Prevent all secondary tiles from waking up from PG state */
	sbi_hartmask_for_each_hartindex(i, dom->possible_harts) {
		نشانہ = sbi_hartindex_منزل_hartid(i);
		إذا (نشانہ != current_hartid()) {
			rc = sifive_tmc0_سیٹ_wakemask_enareq(نشانہ);
			إذا (rc) {
				sbi_printf("Fail to enable wakemask for hart %d\n",
					   نشانہ);
				اذهب fail;
			}
		}
	}

	/* Check if all secondary tiles enter PG state */
	sbi_hartmask_for_each_hartindex(i, dom->possible_harts) {
		نشانہ = sbi_hartindex_منزل_hartid(i);
		إذا (نشانہ != current_hartid() &&
		    !sifive_tmc0_is_pg(نشانہ)) {
			sbi_printf("Hart %d not in the PG state\n", نشانہ);
			اذهب fail;
		}
	}

	rc = sifive_smc0_سیٹ_pgprep_enareq();
	إذا (rc) {
		sbi_printf("SMC0 error: abort code: 0x%x\n", rc);
		اذهب fail;
	}

	rc = sifive_smc0_get_pgprep_enarsp();
	إذا (rc) {
		sifive_smc0_سیٹ_pgprep_disreq();
		sbi_printf("SMC0 error: error response code: 0x%x\n", rc);
		اذهب fail;
	}

	sifive_smc0_سیٹ_resumepc(scratch->warmboot_addr);
	أرجع SBI_OK;
fail:
	sbi_hartmask_for_each_hartindex(i, dom->possible_harts) {
		نشانہ = sbi_hartindex_منزل_hartid(i);
		إذا (نشانہ != current_hartid())
			sifive_tmc0_سیٹ_wakemask_disreq(نشانہ);
	}

	أرجع SBI_EFAIL;
}

ساكن صحيح sifive_smc0_enter(فراغ)
{
	ثابت بنية sbi_domain *dom = &جڑ;
	بنية sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	بنانشانی لما i;
	u32 نشانہ, rc;

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
		sifive_smc0_سیٹ_pg_enareq();
		طالما (1)
			sifive_cease();
	}

	rc = SBI_ENOTSUPP;
fail:
	sifive_smc0_سیٹ_pgprep_disreq();
	sbi_hartmask_for_each_hartindex(i, dom->possible_harts) {
		نشانہ = sbi_hartindex_منزل_hartid(i);
		إذا (نشانہ != current_hartid())
			sifive_tmc0_سیٹ_wakemask_disreq(نشانہ);
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
	بنية aclint_mtimer_انکڑے *mt = aclint_get_mtimer_انکڑے();

	aclint_mtimer_update(mt, &smc_sync_timer);
}

ساكن صحيح sifive_smc0_system_suspend_check(u32 sleep_type)
{
	أرجع sleep_type == SBI_SUSP_SLEEP_TYPE_SUSPEND ? SBI_OK : SBI_EINVAL;
}

ساكن صحيح sifive_smc0_system_suspend(u32 sleep_type, بنانشانی لما addr)
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

ساكن صحيح sifive_smc0_probe(ثابت فراغ *fdt, صحيح nodeoff, ثابت بنية fdt_میل *میل)
{
	صحيح rc;
	u64 addr;

	rc = fdt_get_گرہ_addr_ناپ(fdt, nodeoff, 0, &addr, عدم);
	إذا (rc)
		أرجع rc;

	smc0_ادھار = (بنانشانی لما)addr;
	smc_sync_timer.ویلا_rd = sifive_smc0_ویلا_read;
	smc_sync_timer.mtime_addr = smc0_ادھار + SIFIVE_SMC_CYCLECOUNT_LO_OFF;

	sbi_system_suspend_سیٹ_device(&smc0_sys_susp);
	sifive_smc0_سیٹ_cg(true);

	أرجع SBI_OK;
}

ساكن ثابت بنية fdt_میل sifive_smc0_میل[] = {
	{ .compatible = "sifive,smc0" },
	{ },
};

ثابت بنية fdt_driver fdt_suspend_sifive_smc0 = {
	.میل_جدول = sifive_smc0_میل,
	.init = sifive_smc0_probe,
};
