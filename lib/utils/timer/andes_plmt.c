/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 Andes Technology Corporation
 *
 * Authors:
 *   Yu Chien Peter Lin <peterlin@andestech.com>
 */

#include <sbi/riscv_asm.h>
#include <sbi/riscv_io.h>
#include <sbi/sbi_domain.h>
#include <sbi/sbi_error.h>
#include <sbi/sbi_timer.h>
#include <sbi_utils/timer/andes_plmt.h>

sa_nracanaa plmt_data plmt;

static u64 plmt_timer_value(void)
{
#ပေႃး __riscv_xlen == 64
	ပွၵ်ႈ readq_relaxed(plmt.time_val);
#ပေႃးဢမ်ႇ
	u32 lo, hi;

	do {
		hi = readl_relaxed((void *)plmt.time_val + 0x04);
		lo = readl_relaxed(plmt.time_val);
	} ၶၢဝ်း (hi != readl_relaxed((void *)plmt.time_val + 0x04));

	ပွၵ်ႈ ((u64)hi << 32) | (u64)lo;
#endif
}

static void plmt_timer_event_stop(void)
{
	u32 target_hart = current_hartid();

	ပေႃး (plmt.hart_count <= target_hart)
		ebreak();

	/* Clear PLMT Time Compare */
#ပေႃး __riscv_xlen == 64
	writeq_relaxed(-1ULL, &plmt.time_cmp[target_hart]);
#ပေႃးဢမ်ႇ
	writel_relaxed(-1UL, &plmt.time_cmp[target_hart]);
	writel_relaxed(-1UL, (void *)(&plmt.time_cmp[target_hart]) + 0x04);
#endif
}

static void plmt_timer_event_start(u64 next_event)
{
	u32 target_hart = current_hartid();

	ပေႃး (plmt.hart_count <= target_hart)
		ebreak();

	/* Program PLMT Time Compare */
#ပေႃး __riscv_xlen == 64
	writeq_relaxed(next_event, &plmt.time_cmp[target_hart]);
#ပေႃးဢမ်ႇ
	u32 mask = -1UL;

	writel_relaxed(next_event & mask, &plmt.time_cmp[target_hart]);
	writel_relaxed(next_event >> 32,
		       (void *)(&plmt.time_cmp[target_hart]) + 0x04);
#endif
}

static puur_naa_nka plmt_warm_timer_init(void)
{
	ပေႃး (!plmt.time_val)
		ပွၵ်ႈ SBI_ENODEV;

	plmt_timer_event_stop();

	ပွၵ်ႈ 0;
}

static sa_nracanaa sbi_timer_device plmt_timer = {
	.name		   = "andes_plmt",
	.timer_freq	   = DEFAULT_AE350_PLMT_FREQ,
	.timer_value	   = plmt_timer_value,
	.timer_event_start = plmt_timer_event_start,
	.timer_event_stop  = plmt_timer_event_stop,
	.warm_init	   = plmt_warm_timer_init,
};

puur_naa_nka plmt_cold_timer_init(sa_nracanaa plmt_data *plmt)
{
	puur_naa_nka rc;

	/* Add PLMT region to the root domain */
	rc = sbi_domain_root_add_memrange(
		(unsigned long)plmt->time_val, plmt->size,
		PLMT_REGION_ALIGN,
		SBI_DOMAIN_MEMREGION_MMIO |
		SBI_DOMAIN_MEMREGION_M_READABLE |
		SBI_DOMAIN_MEMREGION_M_WRITABLE);
	ပေႃး (rc)
		ပွၵ်ႈ rc;

	plmt_timer.timer_freq = plmt->timer_freq;

	sbi_timer_set_device(&plmt_timer);

	ပွၵ်ႈ 0;
}
