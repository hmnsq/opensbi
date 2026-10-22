/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 Andes Technology Corporation
 *
 * Authors:
 *   Yu Chien Peter Lin <peterlin@andestech.com>
 */

#અન્તર્ભાવ <sbi/riscv_asm.h>
#અન્તર્ભાવ <sbi/riscv_io.h>
#અન્તર્ભાવ <sbi/sbi_domain.h>
#અન્તર્ભાવ <sbi/sbi_error.h>
#અન્તર્ભાવ <sbi/sbi_timer.h>
#અન્તર્ભાવ <sbi_utils/timer/andes_plmt.h>

સંરચના plmt_data plmt;

સ્થિર u64 plmt_timer_કિંમત(શૂન્ય)
{
#જો __riscv_xlen == 64
	પાછા readq_relaxed(plmt.time_val);
#નહીંતર
	u32 lo, hi;

	કરો {
		hi = readl_relaxed((શૂન્ય *)plmt.time_val + 0x04);
		lo = readl_relaxed(plmt.time_val);
	} જ્યારે (hi != readl_relaxed((શૂન્ય *)plmt.time_val + 0x04));

	પાછા ((u64)hi << 32) | (u64)lo;
#endif
}

સ્થિર શૂન્ય plmt_timer_event_stop(શૂન્ય)
{
	u32 target_hart = current_hartid();

	જો (plmt.hart_ગણતરી <= target_hart)
		ebreak();

	/* Clear PLMT Time Compare */
#જો __riscv_xlen == 64
	writeq_relaxed(-1ULL, &plmt.time_cmp[target_hart]);
#નહીંતર
	writel_relaxed(-1UL, &plmt.time_cmp[target_hart]);
	writel_relaxed(-1UL, (શૂન્ય *)(&plmt.time_cmp[target_hart]) + 0x04);
#endif
}

સ્થિર શૂન્ય plmt_timer_event_start(u64 next_event)
{
	u32 target_hart = current_hartid();

	જો (plmt.hart_ગણતરી <= target_hart)
		ebreak();

	/* Program PLMT Time Compare */
#જો __riscv_xlen == 64
	writeq_relaxed(next_event, &plmt.time_cmp[target_hart]);
#નહીંતર
	u32 mask = -1UL;

	writel_relaxed(next_event & mask, &plmt.time_cmp[target_hart]);
	writel_relaxed(next_event >> 32,
		       (શૂન્ય *)(&plmt.time_cmp[target_hart]) + 0x04);
#endif
}

સ્થિર પૂર્ણાંક plmt_warm_timer_init(શૂન્ય)
{
	જો (!plmt.time_val)
		પાછા SBI_ENODEV;

	plmt_timer_event_stop();

	પાછા 0;
}

સ્થિર સંરચના sbi_timer_device plmt_timer = {
	.name		   = "andes_plmt",
	.timer_freq	   = DEFAULT_AE350_PLMT_FREQ,
	.timer_કિંમત	   = plmt_timer_કિંમત,
	.timer_event_start = plmt_timer_event_start,
	.timer_event_stop  = plmt_timer_event_stop,
	.warm_init	   = plmt_warm_timer_init,
};

પૂર્ણાંક plmt_cold_timer_init(સંરચના plmt_data *plmt)
{
	પૂર્ણાંક rc;

	/* Add PLMT region to the root domain */
	rc = sbi_domain_મૂળ_add_memrange(
		(unsigned long)plmt->time_val, plmt->size,
		PLMT_REGION_ALIGN,
		SBI_DOMAIN_MEMREGION_MMIO |
		SBI_DOMAIN_MEMREGION_M_READABLE |
		SBI_DOMAIN_MEMREGION_M_WRITABLE);
	જો (rc)
		પાછા rc;

	plmt_timer.timer_freq = plmt->timer_freq;

	sbi_timer_set_device(&plmt_timer);

	પાછા 0;
}
