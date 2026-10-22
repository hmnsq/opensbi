/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 Andes Technology Corporation
 *
 * Authors:
 *   Yu Chien Peter Lin <peterlin@andestech.com>
 */

#شامل <sbi/riscv_asm.h>
#شامل <sbi/riscv_io.h>
#شامل <sbi/sbi_domain.h>
#شامل <sbi/sbi_error.h>
#شامل <sbi/sbi_timer.h>
#شامل <sbi_utils/timer/andes_plmt.h>

ساختار plmt_data plmt;

ایستا u64 plmt_timer_مقدار(تهی)
{
#اگر __riscv_xlen == 64
	بازگشت readq_relaxed(plmt.time_val);
#وگرنه
	u32 lo, hi;

	انجامبده {
		hi = readl_relaxed((تهی *)plmt.time_val + 0x04);
		lo = readl_relaxed(plmt.time_val);
	} وقتی (hi != readl_relaxed((تهی *)plmt.time_val + 0x04));

	بازگشت ((u64)hi << 32) | (u64)lo;
#endif
}

ایستا تهی plmt_timer_event_stop(تهی)
{
	u32 target_hart = current_hartid();

	اگر (plmt.hart_شمار <= target_hart)
		ebreak();

	/* Clear PLMT Time Compare */
#اگر __riscv_xlen == 64
	writeq_relaxed(-1ULL, &plmt.time_cmp[target_hart]);
#وگرنه
	writel_relaxed(-1UL, &plmt.time_cmp[target_hart]);
	writel_relaxed(-1UL, (تهی *)(&plmt.time_cmp[target_hart]) + 0x04);
#endif
}

ایستا تهی plmt_timer_event_start(u64 next_event)
{
	u32 target_hart = current_hartid();

	اگر (plmt.hart_شمار <= target_hart)
		ebreak();

	/* Program PLMT Time Compare */
#اگر __riscv_xlen == 64
	writeq_relaxed(next_event, &plmt.time_cmp[target_hart]);
#وگرنه
	u32 mask = -1UL;

	writel_relaxed(next_event & mask, &plmt.time_cmp[target_hart]);
	writel_relaxed(next_event >> 32,
		       (تهی *)(&plmt.time_cmp[target_hart]) + 0x04);
#endif
}

ایستا صحیح plmt_warm_timer_init(تهی)
{
	اگر (!plmt.time_val)
		بازگشت SBI_ENODEV;

	plmt_timer_event_stop();

	بازگشت 0;
}

ایستا ساختار sbi_timer_device plmt_timer = {
	.name		   = "andes_plmt",
	.timer_freq	   = DEFAULT_AE350_PLMT_FREQ,
	.timer_مقدار	   = plmt_timer_مقدار,
	.timer_event_start = plmt_timer_event_start,
	.timer_event_stop  = plmt_timer_event_stop,
	.warm_init	   = plmt_warm_timer_init,
};

صحیح plmt_cold_timer_init(ساختار plmt_data *plmt)
{
	صحیح rc;

	/* Add PLMT region to the root domain */
	rc = sbi_domain_ریشه_add_memrange(
		(unsigned long)plmt->time_val, plmt->size,
		PLMT_REGION_ALIGN,
		SBI_DOMAIN_MEMREGION_MMIO |
		SBI_DOMAIN_MEMREGION_M_READABLE |
		SBI_DOMAIN_MEMREGION_M_WRITABLE);
	اگر (rc)
		بازگشت rc;

	plmt_timer.timer_freq = plmt->timer_freq;

	sbi_timer_set_device(&plmt_timer);

	بازگشت 0;
}
