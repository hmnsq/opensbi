/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 SiFive
 */

#அந்தர்பாவ <libfdt.h>
#அந்தர்பாவ <sbi/riscv_asm.h>
#அந்தர்பாவ <sbi/riscv_io.h>
#அந்தர்பாவ <sbi/sbi_console.h>
#அந்தர்பாவ <sbi/sbi_domain.h>
#அந்தர்பாவ <sbi/sbi_error.h>
#அந்தர்பாவ <sbi/sbi_hart.h>
#அந்தர்பாவ <sbi/sbi_hsm.h>
#அந்தர்பாவ <sbi/sbi_system.h>
#அந்தர்பாவ <sbi/sbi_timer.h>
#அந்தர்பாவ <sbi_utils/cache/fdt_cmo_helper.h>
#அந்தர்பாவ <sbi_utils/fdt/fdt_driver.h>
#அந்தர்பாவ <sbi_utils/fdt/fdt_helper.h>
#அந்தர்பாவ <sbi_utils/hsm/fdt_hsm_sifive_inst.h>
#அந்தர்பாவ <sbi_utils/hsm/fdt_hsm_sifive_tmc0.h>
#அந்தர்பாவ <sbi_utils/irqchip/aplic.h>
#அந்தர்பாவ <sbi_utils/timer/aclint_mtimer.h>

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

நிலையான கட்டமைப்பு aclint_mtimer_data smc_sync_timer;
நிலையான unsigned long smc0_base;

நிலையான வெறுமை sifive_smc0_set_pmcsync(எழுத்து regid, bool write_mode)
{
	unsigned long addr = smc0_base + SIFIVE_SMC_SYNC_PMC_OFF;
	u32 v = regid | SIFIVE_SMC_SYNCPMC_SYNC_REQ;

	எனில் (write_mode)
		v |= SIFIVE_SMC_SYNCPMC_SYNC_WREQ;

	writel(v, (வெறுமை *)addr);
	போது (!(readl((வெறுமை *)addr) & SIFIVE_SMC_SYNCPMC_SYNC_ACK));
}

நிலையான u64 sifive_smc0_time_read(நிலையற்ற u64 *addr)
{
	u32 lo, hi;

	செய் {
		sifive_smc0_set_pmcsync(SIFIVE_SMC_CYCLECOUNT_LO_OFF, false);
		sifive_smc0_set_pmcsync(SIFIVE_SMC_CYCLECOUNT_HI_OFF, false);
		hi = readl_relaxed((u32 *)addr + 1);
		lo = readl_relaxed((u32 *)addr);
	} போது (hi != readl_relaxed((u32 *)addr + 1));

	திரும்பு ((u64)hi << 32) | (u64)lo;
}

நிலையான வெறுமை sifive_smc0_set_resumepc(physical_addr_t raddr)
{
	/* Set resumepc_lo */
	writel((u32)raddr, (வெறுமை *)(smc0_base + SIFIVE_SMC_RESUMEPC_LO_OFF));
	/* copy resumepc_lo from SMC to PMC */
	sifive_smc0_set_pmcsync(SIFIVE_SMC_RESUMEPC_LO_OFF, true);
#எனில் __riscv_xlen > 32
	/* Set resumepc_hi */
	writel((u32)(raddr >> 32), (வெறுமை *)(smc0_base + SIFIVE_SMC_RESUMEPC_HI_OFF));
	/* copy resumepc_hi from SMC to PMC */
	sifive_smc0_set_pmcsync(SIFIVE_SMC_RESUMEPC_HI_OFF, true);
#endif
}

நிலையான u32 sifive_smc0_get_pgprep_enarsp(வெறுமை)
{
	u32 v = readl((வெறுமை *)(smc0_base + SIFIVE_SMC_PGPREP_OFF));

	திரும்பு v & SIFIVE_SMC_PGPREP_ENARSP;
}

நிலையான வெறுமை sifive_smc0_set_pgprep_disreq(வெறுமை)
{
	unsigned long addr = smc0_base + SIFIVE_SMC_PGPREP_OFF;
	u32 v = readl((வெறுமை *)addr);

	writel(v | SIFIVE_SMC_PGPREP_DIS_REQ, (வெறுமை *)addr);
	போது (!(readl((வெறுமை *)addr) & SIFIVE_SMC_PGPREP_DIS_ACK));
}

நிலையான u32 sifive_smc0_set_pgprep_enareq(வெறுமை)
{
	unsigned long addr = smc0_base + SIFIVE_SMC_PGPREP_OFF;
	u32 v = readl((வெறுமை *)addr);

	writel(v | SIFIVE_SMC_PGPREP_ENA_REQ, (வெறுமை *)addr);
	போது (!(readl((வெறுமை *)addr) & SIFIVE_SMC_PGPREP_ENA_ACK));

	v = readl((வெறுமை *)addr);

	திரும்பு v & SIFIVE_SMC_PGPREP_ABORT;
}

நிலையான வெறுமை sifive_smc0_set_pg_enareq(வெறுமை)
{
	unsigned long addr = smc0_base + SIFIVE_SMC_PG_OFF;
	u32 v = readl((வெறுமை *)addr);

	writel(v | SIFIVE_SMC_PG_ENA_REQ, (வெறுமை *)addr);
}

நிலையான உள்வரி வெறுமை sifive_smc0_set_cg(bool enable)
{
	unsigned long addr = smc0_base + SIFIVE_SMC_WFI_UNCORE_CG_OFF;

	எனில் (enable)
		writel(0, (வெறுமை *)addr);
	இல்லையெனில்
		writel(1, (வெறுமை *)addr);
}

நிலையான முழு sifive_smc0_prep(வெறுமை)
{
	மாறிலி கட்டமைப்பு sbi_domain *dom = &வேர்;
	கட்டமைப்பு sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	unsigned long i;
	முழு rc;
	u32 target;

	எனில் (!smc0_base)
		திரும்பு SBI_ENODEV;

	/* Prevent all secondary tiles from waking up from PG state */
	sbi_hartmask_for_each_hartindex(i, dom->possible_harts) {
		target = sbi_hartindex_to_hartid(i);
		எனில் (target != current_hartid()) {
			rc = sifive_tmc0_set_wakemask_enareq(target);
			எனில் (rc) {
				sbi_printf("Fail to enable wakemask for hart %d\n",
					   target);
				செல் fail;
			}
		}
	}

	/* Check if all secondary tiles enter PG state */
	sbi_hartmask_for_each_hartindex(i, dom->possible_harts) {
		target = sbi_hartindex_to_hartid(i);
		எனில் (target != current_hartid() &&
		    !sifive_tmc0_is_pg(target)) {
			sbi_printf("Hart %d not in the PG state\n", target);
			செல் fail;
		}
	}

	rc = sifive_smc0_set_pgprep_enareq();
	எனில் (rc) {
		sbi_printf("SMC0 error: abort code: 0x%x\n", rc);
		செல் fail;
	}

	rc = sifive_smc0_get_pgprep_enarsp();
	எனில் (rc) {
		sifive_smc0_set_pgprep_disreq();
		sbi_printf("SMC0 error: error response code: 0x%x\n", rc);
		செல் fail;
	}

	sifive_smc0_set_resumepc(scratch->warmboot_addr);
	திரும்பு SBI_OK;
fail:
	sbi_hartmask_for_each_hartindex(i, dom->possible_harts) {
		target = sbi_hartindex_to_hartid(i);
		எனில் (target != current_hartid())
			sifive_tmc0_set_wakemask_disreq(target);
	}

	திரும்பு SBI_EFAIL;
}

நிலையான முழு sifive_smc0_enter(வெறுமை)
{
	மாறிலி கட்டமைப்பு sbi_domain *dom = &வேர்;
	கட்டமைப்பு sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	unsigned long i;
	u32 target, rc;

	/* Flush cache and check if there is wake detect or bus error */
	எனில் (fdt_cmo_llc_flush_all() &&
	    sbi_hart_has_extension(scratch, SBI_HART_EXT_XSIFIVE_CFLUSH_D_L1))
		sifive_cflush();

	rc = sifive_smc0_get_pgprep_enarsp();
	எனில் (rc) {
		sbi_printf("SMC0 error: error response code: 0x%x\n", rc);
		rc = SBI_EFAIL;
		செல் fail;
	}

	எனில் (sbi_hart_has_extension(scratch, SBI_HART_EXT_XSIFIVE_CEASE)) {
		sifive_smc0_set_pg_enareq();
		போது (1)
			sifive_cease();
	}

	rc = SBI_ENOTSUPP;
fail:
	sifive_smc0_set_pgprep_disreq();
	sbi_hartmask_for_each_hartindex(i, dom->possible_harts) {
		target = sbi_hartindex_to_hartid(i);
		எனில் (target != current_hartid())
			sifive_tmc0_set_wakemask_disreq(target);
	}
	திரும்பு rc;
}

நிலையான முழு sifive_smc0_pg(வெறுமை)
{
	முழு rc;

	rc = sifive_smc0_prep();
	எனில் (rc)
		திரும்பு rc;

	திரும்பு sifive_smc0_enter();
}

நிலையான வெறுமை sifive_smc0_mtime_update(வெறுமை)
{
	கட்டமைப்பு aclint_mtimer_data *mt = aclint_get_mtimer_data();

	aclint_mtimer_update(mt, &smc_sync_timer);
}

நிலையான முழு sifive_smc0_system_suspend_check(u32 sleep_type)
{
	திரும்பு sleep_type == SBI_SUSP_SLEEP_TYPE_SUSPEND ? SBI_OK : SBI_EINVAL;
}

நிலையான முழு sifive_smc0_system_suspend(u32 sleep_type, unsigned long addr)
{
	/* Disable the timer interrupt */
	sbi_timer_exit(sbi_scratch_thishart_ptr());

	திரும்பு sifive_smc0_pg();
}

நிலையான வெறுமை sifive_smc0_system_resume(வெறுமை)
{
	aplic_reinit_all();
	sifive_smc0_mtime_update();
}

நிலையான கட்டமைப்பு sbi_system_suspend_device smc0_sys_susp = {
	.name = "Sifive SMC0",
	.system_suspend_check = sifive_smc0_system_suspend_check,
	.system_suspend = sifive_smc0_system_suspend,
	.system_resume = sifive_smc0_system_resume,
};

நிலையான முழு sifive_smc0_probe(மாறிலி வெறுமை *fdt, முழு nodeoff, மாறிலி கட்டமைப்பு fdt_match *match)
{
	முழு rc;
	u64 addr;

	rc = fdt_get_முனை_addr_size(fdt, nodeoff, 0, &addr, வெற்று);
	எனில் (rc)
		திரும்பு rc;

	smc0_base = (unsigned long)addr;
	smc_sync_timer.time_rd = sifive_smc0_time_read;
	smc_sync_timer.mtime_addr = smc0_base + SIFIVE_SMC_CYCLECOUNT_LO_OFF;

	sbi_system_suspend_set_device(&smc0_sys_susp);
	sifive_smc0_set_cg(true);

	திரும்பு SBI_OK;
}

நிலையான மாறிலி கட்டமைப்பு fdt_match sifive_smc0_match[] = {
	{ .compatible = "sifive,smc0" },
	{ },
};

மாறிலி கட்டமைப்பு fdt_driver fdt_suspend_sifive_smc0 = {
	.match_table = sifive_smc0_match,
	.init = sifive_smc0_probe,
};
