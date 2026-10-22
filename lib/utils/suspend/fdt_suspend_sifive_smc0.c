/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 SiFive
 */

#ଅନ୍ତର୍ଭାଵ <libfdt.h>
#ଅନ୍ତର୍ଭାଵ <sbi/riscv_asm.h>
#ଅନ୍ତର୍ଭାଵ <sbi/riscv_io.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_console.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_domain.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_error.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_hart.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_hsm.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_system.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_timer.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/cache/fdt_cmo_helper.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/fdt/fdt_driver.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/fdt/fdt_helper.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/hsm/fdt_hsm_sifive_inst.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/hsm/fdt_hsm_sifive_tmc0.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/irqchip/aplic.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/timer/aclint_mtimer.h>

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

ସ୍ଥିର ସଂରଚନା aclint_mtimer_data smc_sync_timer;
ସ୍ଥିର unsigned long smc0_base;

ସ୍ଥିର ଶୂନ୍ୟ sifive_smc0_set_pmcsync(ଅକ୍ଷର regid, bool write_mode)
{
	unsigned long addr = smc0_base + SIFIVE_SMC_SYNC_PMC_OFF;
	u32 v = regid | SIFIVE_SMC_SYNCPMC_SYNC_REQ;

	ଯଦି (write_mode)
		v |= SIFIVE_SMC_SYNCPMC_SYNC_WREQ;

	writel(v, (ଶୂନ୍ୟ *)addr);
	ଯେତେବେଳେ (!(readl((ଶୂନ୍ୟ *)addr) & SIFIVE_SMC_SYNCPMC_SYNC_ACK));
}

ସ୍ଥିର u64 sifive_smc0_time_read(ଅସ୍ଥିର u64 *addr)
{
	u32 lo, hi;

	କର {
		sifive_smc0_set_pmcsync(SIFIVE_SMC_CYCLECOUNT_LO_OFF, false);
		sifive_smc0_set_pmcsync(SIFIVE_SMC_CYCLECOUNT_HI_OFF, false);
		hi = readl_relaxed((u32 *)addr + 1);
		lo = readl_relaxed((u32 *)addr);
	} ଯେତେବେଳେ (hi != readl_relaxed((u32 *)addr + 1));

	ଫେରନ୍ତୁ ((u64)hi << 32) | (u64)lo;
}

ସ୍ଥିର ଶୂନ୍ୟ sifive_smc0_set_resumepc(physical_addr_t raddr)
{
	/* Set resumepc_lo */
	writel((u32)raddr, (ଶୂନ୍ୟ *)(smc0_base + SIFIVE_SMC_RESUMEPC_LO_OFF));
	/* copy resumepc_lo from SMC to PMC */
	sifive_smc0_set_pmcsync(SIFIVE_SMC_RESUMEPC_LO_OFF, true);
#ଯଦି __riscv_xlen > 32
	/* Set resumepc_hi */
	writel((u32)(raddr >> 32), (ଶୂନ୍ୟ *)(smc0_base + SIFIVE_SMC_RESUMEPC_HI_OFF));
	/* copy resumepc_hi from SMC to PMC */
	sifive_smc0_set_pmcsync(SIFIVE_SMC_RESUMEPC_HI_OFF, true);
#endif
}

ସ୍ଥିର u32 sifive_smc0_get_pgprep_enarsp(ଶୂନ୍ୟ)
{
	u32 v = readl((ଶୂନ୍ୟ *)(smc0_base + SIFIVE_SMC_PGPREP_OFF));

	ଫେରନ୍ତୁ v & SIFIVE_SMC_PGPREP_ENARSP;
}

ସ୍ଥିର ଶୂନ୍ୟ sifive_smc0_set_pgprep_disreq(ଶୂନ୍ୟ)
{
	unsigned long addr = smc0_base + SIFIVE_SMC_PGPREP_OFF;
	u32 v = readl((ଶୂନ୍ୟ *)addr);

	writel(v | SIFIVE_SMC_PGPREP_DIS_REQ, (ଶୂନ୍ୟ *)addr);
	ଯେତେବେଳେ (!(readl((ଶୂନ୍ୟ *)addr) & SIFIVE_SMC_PGPREP_DIS_ACK));
}

ସ୍ଥିର u32 sifive_smc0_set_pgprep_enareq(ଶୂନ୍ୟ)
{
	unsigned long addr = smc0_base + SIFIVE_SMC_PGPREP_OFF;
	u32 v = readl((ଶୂନ୍ୟ *)addr);

	writel(v | SIFIVE_SMC_PGPREP_ENA_REQ, (ଶୂନ୍ୟ *)addr);
	ଯେତେବେଳେ (!(readl((ଶୂନ୍ୟ *)addr) & SIFIVE_SMC_PGPREP_ENA_ACK));

	v = readl((ଶୂନ୍ୟ *)addr);

	ଫେରନ୍ତୁ v & SIFIVE_SMC_PGPREP_ABORT;
}

ସ୍ଥିର ଶୂନ୍ୟ sifive_smc0_set_pg_enareq(ଶୂନ୍ୟ)
{
	unsigned long addr = smc0_base + SIFIVE_SMC_PG_OFF;
	u32 v = readl((ଶୂନ୍ୟ *)addr);

	writel(v | SIFIVE_SMC_PG_ENA_REQ, (ଶୂନ୍ୟ *)addr);
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ sifive_smc0_set_cg(bool enable)
{
	unsigned long addr = smc0_base + SIFIVE_SMC_WFI_UNCORE_CG_OFF;

	ଯଦି (enable)
		writel(0, (ଶୂନ୍ୟ *)addr);
	ଅନ୍ୟଥା
		writel(1, (ଶୂନ୍ୟ *)addr);
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା sifive_smc0_prep(ଶୂନ୍ୟ)
{
	ଅଚଳ ସଂରଚନା sbi_domain *dom = &ମୂଳ;
	ସଂରଚନା sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	unsigned long i;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;
	u32 target;

	ଯଦି (!smc0_base)
		ଫେରନ୍ତୁ SBI_ENODEV;

	/* Prevent all secondary tiles from waking up from PG state */
	sbi_hartmask_for_each_hartindex(i, dom->possible_harts) {
		target = sbi_hartindex_to_hartid(i);
		ଯଦି (target != current_hartid()) {
			rc = sifive_tmc0_set_wakemask_enareq(target);
			ଯଦି (rc) {
				sbi_printf("Fail to enable wakemask for hart %d\n",
					   target);
				ଯାଅ fail;
			}
		}
	}

	/* Check if all secondary tiles enter PG state */
	sbi_hartmask_for_each_hartindex(i, dom->possible_harts) {
		target = sbi_hartindex_to_hartid(i);
		ଯଦି (target != current_hartid() &&
		    !sifive_tmc0_is_pg(target)) {
			sbi_printf("Hart %d not in the PG state\n", target);
			ଯାଅ fail;
		}
	}

	rc = sifive_smc0_set_pgprep_enareq();
	ଯଦି (rc) {
		sbi_printf("SMC0 error: abort code: 0x%x\n", rc);
		ଯାଅ fail;
	}

	rc = sifive_smc0_get_pgprep_enarsp();
	ଯଦି (rc) {
		sifive_smc0_set_pgprep_disreq();
		sbi_printf("SMC0 error: error response code: 0x%x\n", rc);
		ଯାଅ fail;
	}

	sifive_smc0_set_resumepc(scratch->warmboot_addr);
	ଫେରନ୍ତୁ SBI_OK;
fail:
	sbi_hartmask_for_each_hartindex(i, dom->possible_harts) {
		target = sbi_hartindex_to_hartid(i);
		ଯଦି (target != current_hartid())
			sifive_tmc0_set_wakemask_disreq(target);
	}

	ଫେରନ୍ତୁ SBI_EFAIL;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା sifive_smc0_enter(ଶୂନ୍ୟ)
{
	ଅଚଳ ସଂରଚନା sbi_domain *dom = &ମୂଳ;
	ସଂରଚନା sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	unsigned long i;
	u32 target, rc;

	/* Flush cache and check if there is wake detect or bus error */
	ଯଦି (fdt_cmo_llc_flush_all() &&
	    sbi_hart_has_extension(scratch, SBI_HART_EXT_XSIFIVE_CFLUSH_D_L1))
		sifive_cflush();

	rc = sifive_smc0_get_pgprep_enarsp();
	ଯଦି (rc) {
		sbi_printf("SMC0 error: error response code: 0x%x\n", rc);
		rc = SBI_EFAIL;
		ଯାଅ fail;
	}

	ଯଦି (sbi_hart_has_extension(scratch, SBI_HART_EXT_XSIFIVE_CEASE)) {
		sifive_smc0_set_pg_enareq();
		ଯେତେବେଳେ (1)
			sifive_cease();
	}

	rc = SBI_ENOTSUPP;
fail:
	sifive_smc0_set_pgprep_disreq();
	sbi_hartmask_for_each_hartindex(i, dom->possible_harts) {
		target = sbi_hartindex_to_hartid(i);
		ଯଦି (target != current_hartid())
			sifive_tmc0_set_wakemask_disreq(target);
	}
	ଫେରନ୍ତୁ rc;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା sifive_smc0_pg(ଶୂନ୍ୟ)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;

	rc = sifive_smc0_prep();
	ଯଦି (rc)
		ଫେରନ୍ତୁ rc;

	ଫେରନ୍ତୁ sifive_smc0_enter();
}

ସ୍ଥିର ଶୂନ୍ୟ sifive_smc0_mtime_update(ଶୂନ୍ୟ)
{
	ସଂରଚନା aclint_mtimer_data *mt = aclint_get_mtimer_data();

	aclint_mtimer_update(mt, &smc_sync_timer);
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା sifive_smc0_system_suspend_check(u32 sleep_type)
{
	ଫେରନ୍ତୁ sleep_type == SBI_SUSP_SLEEP_TYPE_SUSPEND ? SBI_OK : SBI_EINVAL;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା sifive_smc0_system_suspend(u32 sleep_type, unsigned long addr)
{
	/* Disable the timer interrupt */
	sbi_timer_exit(sbi_scratch_thishart_ptr());

	ଫେରନ୍ତୁ sifive_smc0_pg();
}

ସ୍ଥିର ଶୂନ୍ୟ sifive_smc0_system_resume(ଶୂନ୍ୟ)
{
	aplic_reinit_all();
	sifive_smc0_mtime_update();
}

ସ୍ଥିର ସଂରଚନା sbi_system_suspend_device smc0_sys_susp = {
	.name = "Sifive SMC0",
	.system_suspend_check = sifive_smc0_system_suspend_check,
	.system_suspend = sifive_smc0_system_suspend,
	.system_resume = sifive_smc0_system_resume,
};

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା sifive_smc0_probe(ଅଚଳ ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nodeoff, ଅଚଳ ସଂରଚନା fdt_match *match)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;
	u64 addr;

	rc = fdt_get_ଗଣ୍ଠି_addr_size(fdt, nodeoff, 0, &addr, ଖାଲି);
	ଯଦି (rc)
		ଫେରନ୍ତୁ rc;

	smc0_base = (unsigned long)addr;
	smc_sync_timer.time_rd = sifive_smc0_time_read;
	smc_sync_timer.mtime_addr = smc0_base + SIFIVE_SMC_CYCLECOUNT_LO_OFF;

	sbi_system_suspend_set_device(&smc0_sys_susp);
	sifive_smc0_set_cg(true);

	ଫେରନ୍ତୁ SBI_OK;
}

ସ୍ଥିର ଅଚଳ ସଂରଚନା fdt_match sifive_smc0_match[] = {
	{ .compatible = "sifive,smc0" },
	{ },
};

ଅଚଳ ସଂରଚନା fdt_driver fdt_suspend_sifive_smc0 = {
	.match_table = sifive_smc0_match,
	.init = sifive_smc0_probe,
};
