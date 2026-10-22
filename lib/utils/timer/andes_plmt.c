/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 Andes Technology Corporation
 *
 * Authors:
 *   Yu Chien Peter Lin <peterlin@andestech.com>
 */

#ଅନ୍ତର୍ଭାଵ <sbi/riscv_asm.h>
#ଅନ୍ତର୍ଭାଵ <sbi/riscv_io.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_domain.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_error.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_timer.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/timer/andes_plmt.h>

ସଂରଚନା plmt_data plmt;

ସ୍ଥିର u64 plmt_timer_ମୂଲ୍ୟ(ଶୂନ୍ୟ)
{
#ଯଦି __riscv_xlen == 64
	ଫେରନ୍ତୁ readq_relaxed(plmt.time_val);
#ଅନ୍ୟଥା
	u32 lo, hi;

	କର {
		hi = readl_relaxed((ଶୂନ୍ୟ *)plmt.time_val + 0x04);
		lo = readl_relaxed(plmt.time_val);
	} ଯେତେବେଳେ (hi != readl_relaxed((ଶୂନ୍ୟ *)plmt.time_val + 0x04));

	ଫେରନ୍ତୁ ((u64)hi << 32) | (u64)lo;
#endif
}

ସ୍ଥିର ଶୂନ୍ୟ plmt_timer_event_stop(ଶୂନ୍ୟ)
{
	u32 target_hart = current_hartid();

	ଯଦି (plmt.hart_ଗଣନା <= target_hart)
		ebreak();

	/* Clear PLMT Time Compare */
#ଯଦି __riscv_xlen == 64
	writeq_relaxed(-1ULL, &plmt.time_cmp[target_hart]);
#ଅନ୍ୟଥା
	writel_relaxed(-1UL, &plmt.time_cmp[target_hart]);
	writel_relaxed(-1UL, (ଶୂନ୍ୟ *)(&plmt.time_cmp[target_hart]) + 0x04);
#endif
}

ସ୍ଥିର ଶୂନ୍ୟ plmt_timer_event_start(u64 next_event)
{
	u32 target_hart = current_hartid();

	ଯଦି (plmt.hart_ଗଣନା <= target_hart)
		ebreak();

	/* Program PLMT Time Compare */
#ଯଦି __riscv_xlen == 64
	writeq_relaxed(next_event, &plmt.time_cmp[target_hart]);
#ଅନ୍ୟଥା
	u32 mask = -1UL;

	writel_relaxed(next_event & mask, &plmt.time_cmp[target_hart]);
	writel_relaxed(next_event >> 32,
		       (ଶୂନ୍ୟ *)(&plmt.time_cmp[target_hart]) + 0x04);
#endif
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା plmt_warm_timer_init(ଶୂନ୍ୟ)
{
	ଯଦି (!plmt.time_val)
		ଫେରନ୍ତୁ SBI_ENODEV;

	plmt_timer_event_stop();

	ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ସଂରଚନା sbi_timer_device plmt_timer = {
	.name		   = "andes_plmt",
	.timer_freq	   = DEFAULT_AE350_PLMT_FREQ,
	.timer_ମୂଲ୍ୟ	   = plmt_timer_ମୂଲ୍ୟ,
	.timer_event_start = plmt_timer_event_start,
	.timer_event_stop  = plmt_timer_event_stop,
	.warm_init	   = plmt_warm_timer_init,
};

ପୂର୍ଣ୍ଣସଂଖ୍ୟା plmt_cold_timer_init(ସଂରଚନା plmt_data *plmt)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;

	/* Add PLMT region to the root domain */
	rc = sbi_domain_ମୂଳ_add_memrange(
		(unsigned long)plmt->time_val, plmt->size,
		PLMT_REGION_ALIGN,
		SBI_DOMAIN_MEMREGION_MMIO |
		SBI_DOMAIN_MEMREGION_M_READABLE |
		SBI_DOMAIN_MEMREGION_M_WRITABLE);
	ଯଦି (rc)
		ଫେରନ୍ତୁ rc;

	plmt_timer.timer_freq = plmt->timer_freq;

	sbi_timer_set_device(&plmt_timer);

	ଫେରନ୍ତୁ 0;
}
