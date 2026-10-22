/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 Andes Technology Corporation
 *
 * Authors:
 *   Yu Chien Peter Lin <peterlin@andestech.com>
 */

#அந்தர்பாவ <sbi/riscv_asm.h>
#அந்தர்பாவ <sbi/riscv_io.h>
#அந்தர்பாவ <sbi/sbi_domain.h>
#அந்தர்பாவ <sbi/sbi_error.h>
#அந்தர்பாவ <sbi/sbi_timer.h>
#அந்தர்பாவ <sbi_utils/timer/andes_plmt.h>

கட்டமைப்பு plmt_data plmt;

நிலையான u64 plmt_timer_மதிப்பு(வெறுமை)
{
#எனில் __riscv_xlen == 64
	திரும்பு readq_relaxed(plmt.time_val);
#இல்லையெனில்
	u32 lo, hi;

	செய் {
		hi = readl_relaxed((வெறுமை *)plmt.time_val + 0x04);
		lo = readl_relaxed(plmt.time_val);
	} போது (hi != readl_relaxed((வெறுமை *)plmt.time_val + 0x04));

	திரும்பு ((u64)hi << 32) | (u64)lo;
#endif
}

நிலையான வெறுமை plmt_timer_event_stop(வெறுமை)
{
	u32 target_hart = current_hartid();

	எனில் (plmt.hart_மொத்தம் <= target_hart)
		ebreak();

	/* Clear PLMT Time Compare */
#எனில் __riscv_xlen == 64
	writeq_relaxed(-1ULL, &plmt.time_cmp[target_hart]);
#இல்லையெனில்
	writel_relaxed(-1UL, &plmt.time_cmp[target_hart]);
	writel_relaxed(-1UL, (வெறுமை *)(&plmt.time_cmp[target_hart]) + 0x04);
#endif
}

நிலையான வெறுமை plmt_timer_event_start(u64 next_event)
{
	u32 target_hart = current_hartid();

	எனில் (plmt.hart_மொத்தம் <= target_hart)
		ebreak();

	/* Program PLMT Time Compare */
#எனில் __riscv_xlen == 64
	writeq_relaxed(next_event, &plmt.time_cmp[target_hart]);
#இல்லையெனில்
	u32 mask = -1UL;

	writel_relaxed(next_event & mask, &plmt.time_cmp[target_hart]);
	writel_relaxed(next_event >> 32,
		       (வெறுமை *)(&plmt.time_cmp[target_hart]) + 0x04);
#endif
}

நிலையான முழு plmt_warm_timer_init(வெறுமை)
{
	எனில் (!plmt.time_val)
		திரும்பு SBI_ENODEV;

	plmt_timer_event_stop();

	திரும்பு 0;
}

நிலையான கட்டமைப்பு sbi_timer_device plmt_timer = {
	.name		   = "andes_plmt",
	.timer_freq	   = DEFAULT_AE350_PLMT_FREQ,
	.timer_மதிப்பு	   = plmt_timer_மதிப்பு,
	.timer_event_start = plmt_timer_event_start,
	.timer_event_stop  = plmt_timer_event_stop,
	.warm_init	   = plmt_warm_timer_init,
};

முழு plmt_cold_timer_init(கட்டமைப்பு plmt_data *plmt)
{
	முழு rc;

	/* Add PLMT region to the root domain */
	rc = sbi_domain_வேர்_add_memrange(
		(unsigned long)plmt->time_val, plmt->size,
		PLMT_REGION_ALIGN,
		SBI_DOMAIN_MEMREGION_MMIO |
		SBI_DOMAIN_MEMREGION_M_READABLE |
		SBI_DOMAIN_MEMREGION_M_WRITABLE);
	எனில் (rc)
		திரும்பு rc;

	plmt_timer.timer_freq = plmt->timer_freq;

	sbi_timer_set_device(&plmt_timer);

	திரும்பு 0;
}
