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

ایستا ساختار aclint_mtimer_data smc_sync_timer;
ایستا unsigned long smc0_base;

ایستا تهی sifive_smc0_set_pmcsync(نویسه regid, bool write_mode)
{
	unsigned long addr = smc0_base + SIFIVE_SMC_SYNC_PMC_OFF;
	u32 v = regid | SIFIVE_SMC_SYNCPMC_SYNC_REQ;

	اگر (write_mode)
		v |= SIFIVE_SMC_SYNCPMC_SYNC_WREQ;

	writel(v, (تهی *)addr);
	وقتی (!(readl((تهی *)addr) & SIFIVE_SMC_SYNCPMC_SYNC_ACK));
}

ایستا u64 sifive_smc0_time_read(فرار u64 *addr)
{
	u32 lo, hi;

	انجامبده {
		sifive_smc0_set_pmcsync(SIFIVE_SMC_CYCLECOUNT_LO_OFF, false);
		sifive_smc0_set_pmcsync(SIFIVE_SMC_CYCLECOUNT_HI_OFF, false);
		hi = readl_relaxed((u32 *)addr + 1);
		lo = readl_relaxed((u32 *)addr);
	} وقتی (hi != readl_relaxed((u32 *)addr + 1));

	بازگشت ((u64)hi << 32) | (u64)lo;
}

ایستا تهی sifive_smc0_set_resumepc(physical_addr_t raddr)
{
	/* Set resumepc_lo */
	writel((u32)raddr, (تهی *)(smc0_base + SIFIVE_SMC_RESUMEPC_LO_OFF));
	/* copy resumepc_lo from SMC to PMC */
	sifive_smc0_set_pmcsync(SIFIVE_SMC_RESUMEPC_LO_OFF, true);
#اگر __riscv_xlen > 32
	/* Set resumepc_hi */
	writel((u32)(raddr >> 32), (تهی *)(smc0_base + SIFIVE_SMC_RESUMEPC_HI_OFF));
	/* copy resumepc_hi from SMC to PMC */
	sifive_smc0_set_pmcsync(SIFIVE_SMC_RESUMEPC_HI_OFF, true);
#endif
}

ایستا u32 sifive_smc0_get_pgprep_enarsp(تهی)
{
	u32 v = readl((تهی *)(smc0_base + SIFIVE_SMC_PGPREP_OFF));

	بازگشت v & SIFIVE_SMC_PGPREP_ENARSP;
}

ایستا تهی sifive_smc0_set_pgprep_disreq(تهی)
{
	unsigned long addr = smc0_base + SIFIVE_SMC_PGPREP_OFF;
	u32 v = readl((تهی *)addr);

	writel(v | SIFIVE_SMC_PGPREP_DIS_REQ, (تهی *)addr);
	وقتی (!(readl((تهی *)addr) & SIFIVE_SMC_PGPREP_DIS_ACK));
}

ایستا u32 sifive_smc0_set_pgprep_enareq(تهی)
{
	unsigned long addr = smc0_base + SIFIVE_SMC_PGPREP_OFF;
	u32 v = readl((تهی *)addr);

	writel(v | SIFIVE_SMC_PGPREP_ENA_REQ, (تهی *)addr);
	وقتی (!(readl((تهی *)addr) & SIFIVE_SMC_PGPREP_ENA_ACK));

	v = readl((تهی *)addr);

	بازگشت v & SIFIVE_SMC_PGPREP_ABORT;
}

ایستا تهی sifive_smc0_set_pg_enareq(تهی)
{
	unsigned long addr = smc0_base + SIFIVE_SMC_PG_OFF;
	u32 v = readl((تهی *)addr);

	writel(v | SIFIVE_SMC_PG_ENA_REQ, (تهی *)addr);
}

ایستا درونخطی تهی sifive_smc0_set_cg(bool enable)
{
	unsigned long addr = smc0_base + SIFIVE_SMC_WFI_UNCORE_CG_OFF;

	اگر (enable)
		writel(0, (تهی *)addr);
	وگرنه
		writel(1, (تهی *)addr);
}

ایستا صحیح sifive_smc0_prep(تهی)
{
	ثابت ساختار sbi_domain *dom = &ریشه;
	ساختار sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	unsigned long i;
	صحیح rc;
	u32 target;

	اگر (!smc0_base)
		بازگشت SBI_ENODEV;

	/* Prevent all secondary tiles from waking up from PG state */
	sbi_hartmask_for_each_hartindex(i, dom->possible_harts) {
		target = sbi_hartindex_to_hartid(i);
		اگر (target != current_hartid()) {
			rc = sifive_tmc0_set_wakemask_enareq(target);
			اگر (rc) {
				sbi_printf("Fail to enable wakemask for hart %d\n",
					   target);
				برو fail;
			}
		}
	}

	/* Check if all secondary tiles enter PG state */
	sbi_hartmask_for_each_hartindex(i, dom->possible_harts) {
		target = sbi_hartindex_to_hartid(i);
		اگر (target != current_hartid() &&
		    !sifive_tmc0_is_pg(target)) {
			sbi_printf("Hart %d not in the PG state\n", target);
			برو fail;
		}
	}

	rc = sifive_smc0_set_pgprep_enareq();
	اگر (rc) {
		sbi_printf("SMC0 error: abort code: 0x%x\n", rc);
		برو fail;
	}

	rc = sifive_smc0_get_pgprep_enarsp();
	اگر (rc) {
		sifive_smc0_set_pgprep_disreq();
		sbi_printf("SMC0 error: error response code: 0x%x\n", rc);
		برو fail;
	}

	sifive_smc0_set_resumepc(scratch->warmboot_addr);
	بازگشت SBI_OK;
fail:
	sbi_hartmask_for_each_hartindex(i, dom->possible_harts) {
		target = sbi_hartindex_to_hartid(i);
		اگر (target != current_hartid())
			sifive_tmc0_set_wakemask_disreq(target);
	}

	بازگشت SBI_EFAIL;
}

ایستا صحیح sifive_smc0_enter(تهی)
{
	ثابت ساختار sbi_domain *dom = &ریشه;
	ساختار sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	unsigned long i;
	u32 target, rc;

	/* Flush cache and check if there is wake detect or bus error */
	اگر (fdt_cmo_llc_flush_all() &&
	    sbi_hart_has_extension(scratch, SBI_HART_EXT_XSIFIVE_CFLUSH_D_L1))
		sifive_cflush();

	rc = sifive_smc0_get_pgprep_enarsp();
	اگر (rc) {
		sbi_printf("SMC0 error: error response code: 0x%x\n", rc);
		rc = SBI_EFAIL;
		برو fail;
	}

	اگر (sbi_hart_has_extension(scratch, SBI_HART_EXT_XSIFIVE_CEASE)) {
		sifive_smc0_set_pg_enareq();
		وقتی (1)
			sifive_cease();
	}

	rc = SBI_ENOTSUPP;
fail:
	sifive_smc0_set_pgprep_disreq();
	sbi_hartmask_for_each_hartindex(i, dom->possible_harts) {
		target = sbi_hartindex_to_hartid(i);
		اگر (target != current_hartid())
			sifive_tmc0_set_wakemask_disreq(target);
	}
	بازگشت rc;
}

ایستا صحیح sifive_smc0_pg(تهی)
{
	صحیح rc;

	rc = sifive_smc0_prep();
	اگر (rc)
		بازگشت rc;

	بازگشت sifive_smc0_enter();
}

ایستا تهی sifive_smc0_mtime_update(تهی)
{
	ساختار aclint_mtimer_data *mt = aclint_get_mtimer_data();

	aclint_mtimer_update(mt, &smc_sync_timer);
}

ایستا صحیح sifive_smc0_system_suspend_check(u32 sleep_type)
{
	بازگشت sleep_type == SBI_SUSP_SLEEP_TYPE_SUSPEND ? SBI_OK : SBI_EINVAL;
}

ایستا صحیح sifive_smc0_system_suspend(u32 sleep_type, unsigned long addr)
{
	/* Disable the timer interrupt */
	sbi_timer_exit(sbi_scratch_thishart_ptr());

	بازگشت sifive_smc0_pg();
}

ایستا تهی sifive_smc0_system_resume(تهی)
{
	aplic_reinit_all();
	sifive_smc0_mtime_update();
}

ایستا ساختار sbi_system_suspend_device smc0_sys_susp = {
	.name = "Sifive SMC0",
	.system_suspend_check = sifive_smc0_system_suspend_check,
	.system_suspend = sifive_smc0_system_suspend,
	.system_resume = sifive_smc0_system_resume,
};

ایستا صحیح sifive_smc0_probe(ثابت تهی *fdt, صحیح nodeoff, ثابت ساختار fdt_match *match)
{
	صحیح rc;
	u64 addr;

	rc = fdt_get_گره_addr_size(fdt, nodeoff, 0, &addr, پوچ);
	اگر (rc)
		بازگشت rc;

	smc0_base = (unsigned long)addr;
	smc_sync_timer.time_rd = sifive_smc0_time_read;
	smc_sync_timer.mtime_addr = smc0_base + SIFIVE_SMC_CYCLECOUNT_LO_OFF;

	sbi_system_suspend_set_device(&smc0_sys_susp);
	sifive_smc0_set_cg(true);

	بازگشت SBI_OK;
}

ایستا ثابت ساختار fdt_match sifive_smc0_match[] = {
	{ .compatible = "sifive,smc0" },
	{ },
};

ثابت ساختار fdt_driver fdt_suspend_sifive_smc0 = {
	.match_table = sifive_smc0_match,
	.init = sifive_smc0_probe,
};
