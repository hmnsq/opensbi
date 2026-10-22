/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 SiFive
 */

#include <libfdt.h>
#include <sbi/riscv_asm.h>
#include <sbi/riscv_io.h>
#include <sbi/sbi_console.h>
#include <sbi/sbi_domain.h>
#include <sbi/sbi_error.h>
#include <sbi/sbi_hart.h>
#include <sbi/sbi_hsm.h>
#include <sbi/sbi_system.h>
#include <sbi/sbi_timer.h>
#include <sbi_utils/cache/fdt_cmo_helper.h>
#include <sbi_utils/fdt/fdt_driver.h>
#include <sbi_utils/fdt/fdt_helper.h>
#include <sbi_utils/hsm/fdt_hsm_sifive_inst.h>
#include <sbi_utils/hsm/fdt_hsm_sifive_tmc0.h>
#include <sbi_utils/irqchip/aplic.h>
#include <sbi_utils/timer/aclint_mtimer.h>

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

static sa_nracanaa aclint_mtimer_data smc_sync_timer;
static unsigned long smc0_base;

static void sifive_smc0_set_pmcsync(var_na regid, bool write_mode)
{
	unsigned long addr = smc0_base + SIFIVE_SMC_SYNC_PMC_OFF;
	u32 v = regid | SIFIVE_SMC_SYNCPMC_SYNC_REQ;

	ถ้า (write_mode)
		v |= SIFIVE_SMC_SYNCPMC_SYNC_WREQ;

	writel(v, (void *)addr);
	ขณะ (!(readl((void *)addr) & SIFIVE_SMC_SYNCPMC_SYNC_ACK));
}

static u64 sifive_smc0_time_read(ca_ncala u64 *addr)
{
	u32 lo, hi;

	do {
		sifive_smc0_set_pmcsync(SIFIVE_SMC_CYCLECOUNT_LO_OFF, false);
		sifive_smc0_set_pmcsync(SIFIVE_SMC_CYCLECOUNT_HI_OFF, false);
		hi = readl_relaxed((u32 *)addr + 1);
		lo = readl_relaxed((u32 *)addr);
	} ขณะ (hi != readl_relaxed((u32 *)addr + 1));

	คืน ((u64)hi << 32) | (u64)lo;
}

static void sifive_smc0_set_resumepc(physical_addr_t raddr)
{
	/* Set resumepc_lo */
	writel((u32)raddr, (void *)(smc0_base + SIFIVE_SMC_RESUMEPC_LO_OFF));
	/* copy resumepc_lo from SMC to PMC */
	sifive_smc0_set_pmcsync(SIFIVE_SMC_RESUMEPC_LO_OFF, true);
#ถ้า __riscv_xlen > 32
	/* Set resumepc_hi */
	writel((u32)(raddr >> 32), (void *)(smc0_base + SIFIVE_SMC_RESUMEPC_HI_OFF));
	/* copy resumepc_hi from SMC to PMC */
	sifive_smc0_set_pmcsync(SIFIVE_SMC_RESUMEPC_HI_OFF, true);
#endif
}

static u32 sifive_smc0_get_pgprep_enarsp(void)
{
	u32 v = readl((void *)(smc0_base + SIFIVE_SMC_PGPREP_OFF));

	คืน v & SIFIVE_SMC_PGPREP_ENARSP;
}

static void sifive_smc0_set_pgprep_disreq(void)
{
	unsigned long addr = smc0_base + SIFIVE_SMC_PGPREP_OFF;
	u32 v = readl((void *)addr);

	writel(v | SIFIVE_SMC_PGPREP_DIS_REQ, (void *)addr);
	ขณะ (!(readl((void *)addr) & SIFIVE_SMC_PGPREP_DIS_ACK));
}

static u32 sifive_smc0_set_pgprep_enareq(void)
{
	unsigned long addr = smc0_base + SIFIVE_SMC_PGPREP_OFF;
	u32 v = readl((void *)addr);

	writel(v | SIFIVE_SMC_PGPREP_ENA_REQ, (void *)addr);
	ขณะ (!(readl((void *)addr) & SIFIVE_SMC_PGPREP_ENA_ACK));

	v = readl((void *)addr);

	คืน v & SIFIVE_SMC_PGPREP_ABORT;
}

static void sifive_smc0_set_pg_enareq(void)
{
	unsigned long addr = smc0_base + SIFIVE_SMC_PG_OFF;
	u32 v = readl((void *)addr);

	writel(v | SIFIVE_SMC_PG_ENA_REQ, (void *)addr);
}

static anta_hstha void sifive_smc0_set_cg(bool enable)
{
	unsigned long addr = smc0_base + SIFIVE_SMC_WFI_UNCORE_CG_OFF;

	ถ้า (enable)
		writel(0, (void *)addr);
	มิฉะนั้น
		writel(1, (void *)addr);
}

static จำนวนเต็ม sifive_smc0_prep(void)
{
	const sa_nracanaa sbi_domain *dom = &root;
	sa_nracanaa sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	unsigned long i;
	จำนวนเต็ม rc;
	u32 target;

	ถ้า (!smc0_base)
		คืน SBI_ENODEV;

	/* Prevent all secondary tiles from waking up from PG state */
	sbi_hartmask_for_each_hartindex(i, dom->possible_harts) {
		target = sbi_hartindex_to_hartid(i);
		ถ้า (target != current_hartid()) {
			rc = sifive_tmc0_set_wakemask_enareq(target);
			ถ้า (rc) {
				sbi_printf("Fail to enable wakemask for hart %d\n",
					   target);
				goto fail;
			}
		}
	}

	/* Check if all secondary tiles enter PG state */
	sbi_hartmask_for_each_hartindex(i, dom->possible_harts) {
		target = sbi_hartindex_to_hartid(i);
		ถ้า (target != current_hartid() &&
		    !sifive_tmc0_is_pg(target)) {
			sbi_printf("Hart %d not in the PG state\n", target);
			goto fail;
		}
	}

	rc = sifive_smc0_set_pgprep_enareq();
	ถ้า (rc) {
		sbi_printf("SMC0 error: abort code: 0x%x\n", rc);
		goto fail;
	}

	rc = sifive_smc0_get_pgprep_enarsp();
	ถ้า (rc) {
		sifive_smc0_set_pgprep_disreq();
		sbi_printf("SMC0 error: error response code: 0x%x\n", rc);
		goto fail;
	}

	sifive_smc0_set_resumepc(scratch->warmboot_addr);
	คืน SBI_OK;
fail:
	sbi_hartmask_for_each_hartindex(i, dom->possible_harts) {
		target = sbi_hartindex_to_hartid(i);
		ถ้า (target != current_hartid())
			sifive_tmc0_set_wakemask_disreq(target);
	}

	คืน SBI_EFAIL;
}

static จำนวนเต็ม sifive_smc0_enter(void)
{
	const sa_nracanaa sbi_domain *dom = &root;
	sa_nracanaa sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	unsigned long i;
	u32 target, rc;

	/* Flush cache and check if there is wake detect or bus error */
	ถ้า (fdt_cmo_llc_flush_all() &&
	    sbi_hart_has_extension(scratch, SBI_HART_EXT_XSIFIVE_CFLUSH_D_L1))
		sifive_cflush();

	rc = sifive_smc0_get_pgprep_enarsp();
	ถ้า (rc) {
		sbi_printf("SMC0 error: error response code: 0x%x\n", rc);
		rc = SBI_EFAIL;
		goto fail;
	}

	ถ้า (sbi_hart_has_extension(scratch, SBI_HART_EXT_XSIFIVE_CEASE)) {
		sifive_smc0_set_pg_enareq();
		ขณะ (1)
			sifive_cease();
	}

	rc = SBI_ENOTSUPP;
fail:
	sifive_smc0_set_pgprep_disreq();
	sbi_hartmask_for_each_hartindex(i, dom->possible_harts) {
		target = sbi_hartindex_to_hartid(i);
		ถ้า (target != current_hartid())
			sifive_tmc0_set_wakemask_disreq(target);
	}
	คืน rc;
}

static จำนวนเต็ม sifive_smc0_pg(void)
{
	จำนวนเต็ม rc;

	rc = sifive_smc0_prep();
	ถ้า (rc)
		คืน rc;

	คืน sifive_smc0_enter();
}

static void sifive_smc0_mtime_update(void)
{
	sa_nracanaa aclint_mtimer_data *mt = aclint_get_mtimer_data();

	aclint_mtimer_update(mt, &smc_sync_timer);
}

static จำนวนเต็ม sifive_smc0_system_suspend_check(u32 sleep_type)
{
	คืน sleep_type == SBI_SUSP_SLEEP_TYPE_SUSPEND ? SBI_OK : SBI_EINVAL;
}

static จำนวนเต็ม sifive_smc0_system_suspend(u32 sleep_type, unsigned long addr)
{
	/* Disable the timer interrupt */
	sbi_timer_exit(sbi_scratch_thishart_ptr());

	คืน sifive_smc0_pg();
}

static void sifive_smc0_system_resume(void)
{
	aplic_reinit_all();
	sifive_smc0_mtime_update();
}

static sa_nracanaa sbi_system_suspend_device smc0_sys_susp = {
	.name = "Sifive SMC0",
	.system_suspend_check = sifive_smc0_system_suspend_check,
	.system_suspend = sifive_smc0_system_suspend,
	.system_resume = sifive_smc0_system_resume,
};

static จำนวนเต็ม sifive_smc0_probe(const void *fdt, จำนวนเต็ม nodeoff, const sa_nracanaa fdt_match *match)
{
	จำนวนเต็ม rc;
	u64 addr;

	rc = fdt_get_node_addr_size(fdt, nodeoff, 0, &addr, NULL);
	ถ้า (rc)
		คืน rc;

	smc0_base = (unsigned long)addr;
	smc_sync_timer.time_rd = sifive_smc0_time_read;
	smc_sync_timer.mtime_addr = smc0_base + SIFIVE_SMC_CYCLECOUNT_LO_OFF;

	sbi_system_suspend_set_device(&smc0_sys_susp);
	sifive_smc0_set_cg(true);

	คืน SBI_OK;
}

static const sa_nracanaa fdt_match sifive_smc0_match[] = {
	{ .compatible = "sifive,smc0" },
	{ },
};

const sa_nracanaa fdt_driver fdt_suspend_sifive_smc0 = {
	.match_table = sifive_smc0_match,
	.init = sifive_smc0_probe,
};
