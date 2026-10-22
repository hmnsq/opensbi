/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 Andes Technology Corporation
 *
 * Authors:
 *   Yu Chien Peter Lin <peterlin@andestech.com>
 */

#ಅನ್ತರ್ಭಾವ <sbi/riscv_asm.h>
#ಅನ್ತರ್ಭಾವ <sbi/riscv_io.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_domain.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_error.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_timer.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/timer/andes_plmt.h>

ಸಂರಚನಾ plmt_data plmt;

ಸ್ಥಿರ u64 plmt_timer_ಮೌಲ್ಯ(ಶೂನ್ಯ)
{
#ಯದಿ __riscv_xlen == 64
	ಪ್ರತ್ಯಾಗಮ readq_relaxed(plmt.time_val);
#ಅನ್ಯಥಾ
	u32 lo, hi;

	ಕುರು {
		hi = readl_relaxed((ಶೂನ್ಯ *)plmt.time_val + 0x04);
		lo = readl_relaxed(plmt.time_val);
	} ಯಾವತ್ (hi != readl_relaxed((ಶೂನ್ಯ *)plmt.time_val + 0x04));

	ಪ್ರತ್ಯಾಗಮ ((u64)hi << 32) | (u64)lo;
#endif
}

ಸ್ಥಿರ ಶೂನ್ಯ plmt_timer_event_stop(ಶೂನ್ಯ)
{
	u32 target_hart = current_hartid();

	ಯದಿ (plmt.hart_ಎಣಿಕೆ <= target_hart)
		ebreak();

	/* Clear PLMT Time Compare */
#ಯದಿ __riscv_xlen == 64
	writeq_relaxed(-1ULL, &plmt.time_cmp[target_hart]);
#ಅನ್ಯಥಾ
	writel_relaxed(-1UL, &plmt.time_cmp[target_hart]);
	writel_relaxed(-1UL, (ಶೂನ್ಯ *)(&plmt.time_cmp[target_hart]) + 0x04);
#endif
}

ಸ್ಥಿರ ಶೂನ್ಯ plmt_timer_event_start(u64 next_event)
{
	u32 target_hart = current_hartid();

	ಯದಿ (plmt.hart_ಎಣಿಕೆ <= target_hart)
		ebreak();

	/* Program PLMT Time Compare */
#ಯದಿ __riscv_xlen == 64
	writeq_relaxed(next_event, &plmt.time_cmp[target_hart]);
#ಅನ್ಯಥಾ
	u32 mask = -1UL;

	writel_relaxed(next_event & mask, &plmt.time_cmp[target_hart]);
	writel_relaxed(next_event >> 32,
		       (ಶೂನ್ಯ *)(&plmt.time_cmp[target_hart]) + 0x04);
#endif
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ plmt_warm_timer_init(ಶೂನ್ಯ)
{
	ಯದಿ (!plmt.time_val)
		ಪ್ರತ್ಯಾಗಮ SBI_ENODEV;

	plmt_timer_event_stop();

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಸಂರಚನಾ sbi_timer_device plmt_timer = {
	.name		   = "andes_plmt",
	.timer_freq	   = DEFAULT_AE350_PLMT_FREQ,
	.timer_ಮೌಲ್ಯ	   = plmt_timer_ಮೌಲ್ಯ,
	.timer_event_start = plmt_timer_event_start,
	.timer_event_stop  = plmt_timer_event_stop,
	.warm_init	   = plmt_warm_timer_init,
};

ಪೂರ್ಣಾಙ್ಕ plmt_cold_timer_init(ಸಂರಚನಾ plmt_data *plmt)
{
	ಪೂರ್ಣಾಙ್ಕ rc;

	/* Add PLMT region to the root domain */
	rc = sbi_domain_ಬೇರು_add_memrange(
		(unsigned long)plmt->time_val, plmt->size,
		PLMT_REGION_ALIGN,
		SBI_DOMAIN_MEMREGION_MMIO |
		SBI_DOMAIN_MEMREGION_M_READABLE |
		SBI_DOMAIN_MEMREGION_M_WRITABLE);
	ಯದಿ (rc)
		ಪ್ರತ್ಯಾಗಮ rc;

	plmt_timer.timer_freq = plmt->timer_freq;

	sbi_timer_set_device(&plmt_timer);

	ಪ್ರತ್ಯಾಗಮ 0;
}
