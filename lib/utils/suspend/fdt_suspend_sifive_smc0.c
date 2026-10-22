/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 SiFive
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <libfdt.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/riscv_asm.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/riscv_io.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_console.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_domain.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_error.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_hart.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_hsm.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_system.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_timer.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/cache/fdt_cmo_helper.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/fdt/fdt_driver.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/fdt/fdt_helper.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/hsm/fdt_hsm_sifive_inst.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/hsm/fdt_hsm_sifive_tmc0.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/irqchip/aplic.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/timer/aclint_mtimer.h>

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SIFIVE_SMC_PGPREP_OFF			0x0
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SIFIVE_SMC_PG_OFF			0x4
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SIFIVE_SMC_CCTIMER_OFF			0xc
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SIFIVE_SMC_RESUMEPC_LO_OFF		0x10
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SIFIVE_SMC_RESUMEPC_HI_OFF		0x14
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SIFIVE_SMC_SYNC_PMC_OFF			0x24
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SIFIVE_SMC_CYCLECOUNT_LO_OFF		0x28
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SIFIVE_SMC_CYCLECOUNT_HI_OFF		0x2c
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SIFIVE_SMC_WFI_UNCORE_CG_OFF		0x50

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SIFIVE_SMC_PGPREP_ENA_REQ		ਦੋਆਧਾਰੀ_ਅੰਕ(31)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SIFIVE_SMC_PGPREP_ENA_ACK		ਦੋਆਧਾਰੀ_ਅੰਕ(30)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SIFIVE_SMC_PGPREP_DIS_REQ		ਦੋਆਧਾਰੀ_ਅੰਕ(29)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SIFIVE_SMC_PGPREP_DIS_ACK		ਦੋਆਧਾਰੀ_ਅੰਕ(29)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SIFIVE_SMC_PGPREP_FRONTNOTQ		ਦੋਆਧਾਰੀ_ਅੰਕ(19)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SIFIVE_SMC_PGPREP_CLFPNOTQ		ਦੋਆਧਾਰੀ_ਅੰਕ(18)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SIFIVE_SMC_PGPREP_PMCENAERR		ਦੋਆਧਾਰੀ_ਅੰਕ(17)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SIFIVE_SMC_PGPREP_WAKE_DETECT		ਦੋਆਧਾਰੀ_ਅੰਕ(16)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SIFIVE_SMC_PGPREP_BUSERR		ਦੋਆਧਾਰੀ_ਅੰਕ(15)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SIFIVE_SMC_PGPREP_EARLY_ABORT		ਦੋਆਧਾਰੀ_ਅੰਕ(3)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SIFIVE_SMC_PGPREP_ਅੰਦਰੂਨੀ_ABORT	ਦੋਆਧਾਰੀ_ਅੰਕ(2)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SIFIVE_SMC_PGPREP_ENARSP		(SIFIVE_SMC_PGPREP_FRONTNOTQ | \
						 SIFIVE_SMC_PGPREP_CLFPNOTQ | \
						 SIFIVE_SMC_PGPREP_PMCENAERR | \
						 SIFIVE_SMC_PGPREP_WAKE_DETECT | \
						 SIFIVE_SMC_PGPREP_BUSERR)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SIFIVE_SMC_PGPREP_ABORT			(SIFIVE_SMC_PGPREP_EARLY_ABORT | \
						 SIFIVE_SMC_PGPREP_ਅੰਦਰੂਨੀ_ABORT)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SIFIVE_SMC_PG_ENA_REQ			ਦੋਆਧਾਰੀ_ਅੰਕ(31)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SIFIVE_SMC_PG_WARM_RESET		ਦੋਆਧਾਰੀ_ਅੰਕ(1)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SIFIVE_SMC_SYNCPMC_SYNC_REQ		ਦੋਆਧਾਰੀ_ਅੰਕ(31)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SIFIVE_SMC_SYNCPMC_SYNC_WREQ		ਦੋਆਧਾਰੀ_ਅੰਕ(30)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SIFIVE_SMC_SYNCPMC_SYNC_ACK		ਦੋਆਧਾਰੀ_ਅੰਕ(29)

ਸਥਿਰ ਬਣਤਰ aclint_mtimer_ਅੰਕੜੇ smc_sync_timer;
ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ smc0_ਅਧਾਰ;

ਸਥਿਰ ਖਾਲੀ sifive_smc0_ਸੈੱਟ_pmcsync(ਅੱਖਰ regid, bool write_ਬਹੁਲਕ)
{
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ addr = smc0_ਅਧਾਰ + SIFIVE_SMC_SYNC_PMC_OFF;
	u32 v = regid | SIFIVE_SMC_SYNCPMC_SYNC_REQ;

	ਜੇ (write_ਬਹੁਲਕ)
		v |= SIFIVE_SMC_SYNCPMC_SYNC_WREQ;

	writel(v, (ਖਾਲੀ *)addr);
	ਜਦੋਂ (!(readl((ਖਾਲੀ *)addr) & SIFIVE_SMC_SYNCPMC_SYNC_ACK));
}

ਸਥਿਰ u64 sifive_smc0_ਸਮਾਂ_read(ਅਸਥਿਰ u64 *addr)
{
	u32 lo, hi;

	ਕਰੋ {
		sifive_smc0_ਸੈੱਟ_pmcsync(SIFIVE_SMC_CYCLECOUNT_LO_OFF, false);
		sifive_smc0_ਸੈੱਟ_pmcsync(SIFIVE_SMC_CYCLECOUNT_HI_OFF, false);
		hi = readl_relaxed((u32 *)addr + 1);
		lo = readl_relaxed((u32 *)addr);
	} ਜਦੋਂ (hi != readl_relaxed((u32 *)addr + 1));

	ਵਾਪਸ ((u64)hi << 32) | (u64)lo;
}

ਸਥਿਰ ਖਾਲੀ sifive_smc0_ਸੈੱਟ_resumepc(physical_addr_t raddr)
{
	/* Set resumepc_lo */
	writel((u32)raddr, (ਖਾਲੀ *)(smc0_ਅਧਾਰ + SIFIVE_SMC_RESUMEPC_LO_OFF));
	/* copy resumepc_lo from SMC to PMC */
	sifive_smc0_ਸੈੱਟ_pmcsync(SIFIVE_SMC_RESUMEPC_LO_OFF, true);
#ਜੇ __riscv_xlen > 32
	/* Set resumepc_hi */
	writel((u32)(raddr >> 32), (ਖਾਲੀ *)(smc0_ਅਧਾਰ + SIFIVE_SMC_RESUMEPC_HI_OFF));
	/* copy resumepc_hi from SMC to PMC */
	sifive_smc0_ਸੈੱਟ_pmcsync(SIFIVE_SMC_RESUMEPC_HI_OFF, true);
#ਜੇ_ਅੰਤ
}

ਸਥਿਰ u32 sifive_smc0_get_pgprep_enarsp(ਖਾਲੀ)
{
	u32 v = readl((ਖਾਲੀ *)(smc0_ਅਧਾਰ + SIFIVE_SMC_PGPREP_OFF));

	ਵਾਪਸ v & SIFIVE_SMC_PGPREP_ENARSP;
}

ਸਥਿਰ ਖਾਲੀ sifive_smc0_ਸੈੱਟ_pgprep_disreq(ਖਾਲੀ)
{
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ addr = smc0_ਅਧਾਰ + SIFIVE_SMC_PGPREP_OFF;
	u32 v = readl((ਖਾਲੀ *)addr);

	writel(v | SIFIVE_SMC_PGPREP_DIS_REQ, (ਖਾਲੀ *)addr);
	ਜਦੋਂ (!(readl((ਖਾਲੀ *)addr) & SIFIVE_SMC_PGPREP_DIS_ACK));
}

ਸਥਿਰ u32 sifive_smc0_ਸੈੱਟ_pgprep_enareq(ਖਾਲੀ)
{
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ addr = smc0_ਅਧਾਰ + SIFIVE_SMC_PGPREP_OFF;
	u32 v = readl((ਖਾਲੀ *)addr);

	writel(v | SIFIVE_SMC_PGPREP_ENA_REQ, (ਖਾਲੀ *)addr);
	ਜਦੋਂ (!(readl((ਖਾਲੀ *)addr) & SIFIVE_SMC_PGPREP_ENA_ACK));

	v = readl((ਖਾਲੀ *)addr);

	ਵਾਪਸ v & SIFIVE_SMC_PGPREP_ABORT;
}

ਸਥਿਰ ਖਾਲੀ sifive_smc0_ਸੈੱਟ_pg_enareq(ਖਾਲੀ)
{
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ addr = smc0_ਅਧਾਰ + SIFIVE_SMC_PG_OFF;
	u32 v = readl((ਖਾਲੀ *)addr);

	writel(v | SIFIVE_SMC_PG_ENA_REQ, (ਖਾਲੀ *)addr);
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ sifive_smc0_ਸੈੱਟ_cg(bool enable)
{
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ addr = smc0_ਅਧਾਰ + SIFIVE_SMC_WFI_UNCORE_CG_OFF;

	ਜੇ (enable)
		writel(0, (ਖਾਲੀ *)addr);
	ਨਹੀਂਤਾਂ
		writel(1, (ਖਾਲੀ *)addr);
}

ਸਥਿਰ ਪੂਰਨ sifive_smc0_prep(ਖਾਲੀ)
{
	ਅਚਲ ਬਣਤਰ sbi_domain *dom = &ਜੜ੍ਹ;
	ਬਣਤਰ sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ i;
	ਪੂਰਨ rc;
	u32 ਨਿਸ਼ਾਨਾ;

	ਜੇ (!smc0_ਅਧਾਰ)
		ਵਾਪਸ SBI_ENODEV;

	/* Prevent all secondary tiles from waking up from PG state */
	sbi_hartmask_for_each_hartindex(i, dom->possible_harts) {
		ਨਿਸ਼ਾਨਾ = sbi_hartindex_ਮੰਜ਼ਿਲ_hartid(i);
		ਜੇ (ਨਿਸ਼ਾਨਾ != current_hartid()) {
			rc = sifive_tmc0_ਸੈੱਟ_wakemask_enareq(ਨਿਸ਼ਾਨਾ);
			ਜੇ (rc) {
				sbi_printf("Fail to enable wakemask for hart %d\n",
					   ਨਿਸ਼ਾਨਾ);
				ਜਾਓ fail;
			}
		}
	}

	/* Check if all secondary tiles enter PG state */
	sbi_hartmask_for_each_hartindex(i, dom->possible_harts) {
		ਨਿਸ਼ਾਨਾ = sbi_hartindex_ਮੰਜ਼ਿਲ_hartid(i);
		ਜੇ (ਨਿਸ਼ਾਨਾ != current_hartid() &&
		    !sifive_tmc0_is_pg(ਨਿਸ਼ਾਨਾ)) {
			sbi_printf("Hart %d not in the PG state\n", ਨਿਸ਼ਾਨਾ);
			ਜਾਓ fail;
		}
	}

	rc = sifive_smc0_ਸੈੱਟ_pgprep_enareq();
	ਜੇ (rc) {
		sbi_printf("SMC0 error: abort code: 0x%x\n", rc);
		ਜਾਓ fail;
	}

	rc = sifive_smc0_get_pgprep_enarsp();
	ਜੇ (rc) {
		sifive_smc0_ਸੈੱਟ_pgprep_disreq();
		sbi_printf("SMC0 error: error response code: 0x%x\n", rc);
		ਜਾਓ fail;
	}

	sifive_smc0_ਸੈੱਟ_resumepc(scratch->warmboot_addr);
	ਵਾਪਸ SBI_OK;
fail:
	sbi_hartmask_for_each_hartindex(i, dom->possible_harts) {
		ਨਿਸ਼ਾਨਾ = sbi_hartindex_ਮੰਜ਼ਿਲ_hartid(i);
		ਜੇ (ਨਿਸ਼ਾਨਾ != current_hartid())
			sifive_tmc0_ਸੈੱਟ_wakemask_disreq(ਨਿਸ਼ਾਨਾ);
	}

	ਵਾਪਸ SBI_EFAIL;
}

ਸਥਿਰ ਪੂਰਨ sifive_smc0_enter(ਖਾਲੀ)
{
	ਅਚਲ ਬਣਤਰ sbi_domain *dom = &ਜੜ੍ਹ;
	ਬਣਤਰ sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ i;
	u32 ਨਿਸ਼ਾਨਾ, rc;

	/* Flush cache and check if there is wake detect or bus error */
	ਜੇ (fdt_cmo_llc_flush_all() &&
	    sbi_hart_has_extension(scratch, SBI_HART_EXT_XSIFIVE_CFLUSH_D_L1))
		sifive_cflush();

	rc = sifive_smc0_get_pgprep_enarsp();
	ਜੇ (rc) {
		sbi_printf("SMC0 error: error response code: 0x%x\n", rc);
		rc = SBI_EFAIL;
		ਜਾਓ fail;
	}

	ਜੇ (sbi_hart_has_extension(scratch, SBI_HART_EXT_XSIFIVE_CEASE)) {
		sifive_smc0_ਸੈੱਟ_pg_enareq();
		ਜਦੋਂ (1)
			sifive_cease();
	}

	rc = SBI_ENOTSUPP;
fail:
	sifive_smc0_ਸੈੱਟ_pgprep_disreq();
	sbi_hartmask_for_each_hartindex(i, dom->possible_harts) {
		ਨਿਸ਼ਾਨਾ = sbi_hartindex_ਮੰਜ਼ਿਲ_hartid(i);
		ਜੇ (ਨਿਸ਼ਾਨਾ != current_hartid())
			sifive_tmc0_ਸੈੱਟ_wakemask_disreq(ਨਿਸ਼ਾਨਾ);
	}
	ਵਾਪਸ rc;
}

ਸਥਿਰ ਪੂਰਨ sifive_smc0_pg(ਖਾਲੀ)
{
	ਪੂਰਨ rc;

	rc = sifive_smc0_prep();
	ਜੇ (rc)
		ਵਾਪਸ rc;

	ਵਾਪਸ sifive_smc0_enter();
}

ਸਥਿਰ ਖਾਲੀ sifive_smc0_mtime_update(ਖਾਲੀ)
{
	ਬਣਤਰ aclint_mtimer_ਅੰਕੜੇ *mt = aclint_get_mtimer_ਅੰਕੜੇ();

	aclint_mtimer_update(mt, &smc_sync_timer);
}

ਸਥਿਰ ਪੂਰਨ sifive_smc0_system_suspend_check(u32 sleep_type)
{
	ਵਾਪਸ sleep_type == SBI_SUSP_SLEEP_TYPE_SUSPEND ? SBI_OK : SBI_EINVAL;
}

ਸਥਿਰ ਪੂਰਨ sifive_smc0_system_suspend(u32 sleep_type, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ addr)
{
	/* Disable the timer interrupt */
	sbi_timer_exit(sbi_scratch_thishart_ptr());

	ਵਾਪਸ sifive_smc0_pg();
}

ਸਥਿਰ ਖਾਲੀ sifive_smc0_system_resume(ਖਾਲੀ)
{
	aplic_reinit_all();
	sifive_smc0_mtime_update();
}

ਸਥਿਰ ਬਣਤਰ sbi_system_suspend_device smc0_sys_susp = {
	.name = "Sifive SMC0",
	.system_suspend_check = sifive_smc0_system_suspend_check,
	.system_suspend = sifive_smc0_system_suspend,
	.system_resume = sifive_smc0_system_resume,
};

ਸਥਿਰ ਪੂਰਨ sifive_smc0_probe(ਅਚਲ ਖਾਲੀ *fdt, ਪੂਰਨ nodeoff, ਅਚਲ ਬਣਤਰ fdt_ਮੇਲ *ਮੇਲ)
{
	ਪੂਰਨ rc;
	u64 addr;

	rc = fdt_get_ਗੰਢ_addr_ਮਾਪ(fdt, nodeoff, 0, &addr, ਨਿਰਾ);
	ਜੇ (rc)
		ਵਾਪਸ rc;

	smc0_ਅਧਾਰ = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)addr;
	smc_sync_timer.ਸਮਾਂ_rd = sifive_smc0_ਸਮਾਂ_read;
	smc_sync_timer.mtime_addr = smc0_ਅਧਾਰ + SIFIVE_SMC_CYCLECOUNT_LO_OFF;

	sbi_system_suspend_ਸੈੱਟ_device(&smc0_sys_susp);
	sifive_smc0_ਸੈੱਟ_cg(true);

	ਵਾਪਸ SBI_OK;
}

ਸਥਿਰ ਅਚਲ ਬਣਤਰ fdt_ਮੇਲ sifive_smc0_ਮੇਲ[] = {
	{ .compatible = "sifive,smc0" },
	{ },
};

ਅਚਲ ਬਣਤਰ fdt_driver fdt_suspend_sifive_smc0 = {
	.ਮੇਲ_ਸਾਰਣੀ = sifive_smc0_ਮੇਲ,
	.init = sifive_smc0_probe,
};
