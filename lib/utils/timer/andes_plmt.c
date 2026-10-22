/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 Andes Technology Corporation
 *
 * Authors:
 *   Yu Chien Peter Lin <peterlin@andestech.com>
 */

#ضمن <sbi/riscv_asm.h>
#ضمن <sbi/riscv_io.h>
#ضمن <sbi/sbi_domain.h>
#ضمن <sbi/sbi_error.h>
#ضمن <sbi/sbi_timer.h>
#ضمن <sbi_utils/timer/andes_plmt.h>

بنية plmt_data plmt;

ساكن u64 plmt_timer_value(فراغ)
{
#אם __riscv_xlen == 64
	החזר readq_relaxed(plmt.time_val);
#אחרת
	u32 lo, hi;

	עשה {
		hi = readl_relaxed((فراغ *)plmt.time_val + 0x04);
		lo = readl_relaxed(plmt.time_val);
	} כלעוד (hi != readl_relaxed((فراغ *)plmt.time_val + 0x04));

	החזר ((u64)hi << 32) | (u64)lo;
#endif
}

ساكن فراغ plmt_timer_event_stop(فراغ)
{
	u32 target_hart = current_hartid();

	אם (plmt.hart_count <= target_hart)
		ebreak();

	/* Clear PLMT Time Compare */
#אם __riscv_xlen == 64
	writeq_relaxed(-1ULL, &plmt.time_cmp[target_hart]);
#אחרת
	writel_relaxed(-1UL, &plmt.time_cmp[target_hart]);
	writel_relaxed(-1UL, (فراغ *)(&plmt.time_cmp[target_hart]) + 0x04);
#endif
}

ساكن فراغ plmt_timer_event_start(u64 next_event)
{
	u32 target_hart = current_hartid();

	אם (plmt.hart_count <= target_hart)
		ebreak();

	/* Program PLMT Time Compare */
#אם __riscv_xlen == 64
	writeq_relaxed(next_event, &plmt.time_cmp[target_hart]);
#אחרת
	u32 mask = -1UL;

	writel_relaxed(next_event & mask, &plmt.time_cmp[target_hart]);
	writel_relaxed(next_event >> 32,
		       (فراغ *)(&plmt.time_cmp[target_hart]) + 0x04);
#endif
}

ساكن صحيح plmt_warm_timer_init(فراغ)
{
	אם (!plmt.time_val)
		החזר SBI_ENODEV;

	plmt_timer_event_stop();

	החזר 0;
}

ساكن بنية sbi_timer_device plmt_timer = {
	.name		   = "andes_plmt",
	.timer_freq	   = DEFAULT_AE350_PLMT_FREQ,
	.timer_value	   = plmt_timer_value,
	.timer_event_start = plmt_timer_event_start,
	.timer_event_stop  = plmt_timer_event_stop,
	.warm_init	   = plmt_warm_timer_init,
};

صحيح plmt_cold_timer_init(بنية plmt_data *plmt)
{
	صحيح rc;

	/* Add PLMT region to the root domain */
	rc = sbi_domain_root_add_memrange(
		(unsigned long)plmt->time_val, plmt->size,
		PLMT_REGION_ALIGN,
		SBI_DOMAIN_MEMREGION_MMIO |
		SBI_DOMAIN_MEMREGION_M_READABLE |
		SBI_DOMAIN_MEMREGION_M_WRITABLE);
	אם (rc)
		החזר rc;

	plmt_timer.timer_freq = plmt->timer_freq;

	sbi_timer_set_device(&plmt_timer);

	החזר 0;
}
