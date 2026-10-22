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
#အကယ်၍ __riscv_xlen == 64
	ပြန်ပေး readq_relaxed(plmt.time_val);
#မဟုတ်ပါက
	u32 lo, hi;

	do {
		hi = readl_relaxed((void *)plmt.time_val + 0x04);
		lo = readl_relaxed(plmt.time_val);
	} နေစဉ် (hi != readl_relaxed((void *)plmt.time_val + 0x04));

	ပြန်ပေး ((u64)hi << 32) | (u64)lo;
#endif
}

static void plmt_timer_event_stop(void)
{
	u32 target_hart = current_hartid();

	အကယ်၍ (plmt.hart_count <= target_hart)
		ebreak();

	/* Clear PLMT Time Compare */
#အကယ်၍ __riscv_xlen == 64
	writeq_relaxed(-1ULL, &plmt.time_cmp[target_hart]);
#မဟုတ်ပါက
	writel_relaxed(-1UL, &plmt.time_cmp[target_hart]);
	writel_relaxed(-1UL, (void *)(&plmt.time_cmp[target_hart]) + 0x04);
#endif
}

static void plmt_timer_event_start(u64 next_event)
{
	u32 target_hart = current_hartid();

	အကယ်၍ (plmt.hart_count <= target_hart)
		ebreak();

	/* Program PLMT Time Compare */
#အကယ်၍ __riscv_xlen == 64
	writeq_relaxed(next_event, &plmt.time_cmp[target_hart]);
#မဟုတ်ပါက
	u32 mask = -1UL;

	writel_relaxed(next_event & mask, &plmt.time_cmp[target_hart]);
	writel_relaxed(next_event >> 32,
		       (void *)(&plmt.time_cmp[target_hart]) + 0x04);
#endif
}

static ကိန်းပြည့် plmt_warm_timer_init(void)
{
	အကယ်၍ (!plmt.time_val)
		ပြန်ပေး SBI_ENODEV;

	plmt_timer_event_stop();

	ပြန်ပေး 0;
}

static sa_nracanaa sbi_timer_device plmt_timer = {
	.name		   = "andes_plmt",
	.timer_freq	   = DEFAULT_AE350_PLMT_FREQ,
	.timer_value	   = plmt_timer_value,
	.timer_event_start = plmt_timer_event_start,
	.timer_event_stop  = plmt_timer_event_stop,
	.warm_init	   = plmt_warm_timer_init,
};

ကိန်းပြည့် plmt_cold_timer_init(sa_nracanaa plmt_data *plmt)
{
	ကိန်းပြည့် rc;

	/* Add PLMT region to the root domain */
	rc = sbi_domain_root_add_memrange(
		(unsigned long)plmt->time_val, plmt->size,
		PLMT_REGION_ALIGN,
		SBI_DOMAIN_MEMREGION_MMIO |
		SBI_DOMAIN_MEMREGION_M_READABLE |
		SBI_DOMAIN_MEMREGION_M_WRITABLE);
	အကယ်၍ (rc)
		ပြန်ပေး rc;

	plmt_timer.timer_freq = plmt->timer_freq;

	sbi_timer_set_device(&plmt_timer);

	ပြန်ပေး 0;
}
