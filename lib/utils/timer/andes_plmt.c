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

ರಚನೆ plmt_ದತ್ತಾಂಶ plmt;

ಸ್ಥಿರ u64 plmt_timer_ಮೌಲ್ಯ(ಶೂನ್ಯ)
{
#ಆದರೆ __riscv_xlen == 64
	ಹಿಂತಿರುಗಿ readq_relaxed(plmt.ಸಮಯ_val);
#ಇಲ್ಲದಿದ್ದರೆ
	u32 lo, hi;

	ಮಾಡು {
		hi = readl_relaxed((ಶೂನ್ಯ *)plmt.ಸಮಯ_val + 0x04);
		lo = readl_relaxed(plmt.ಸಮಯ_val);
	} ಆಗ (hi != readl_relaxed((ಶೂನ್ಯ *)plmt.ಸಮಯ_val + 0x04));

	ಹಿಂತಿರುಗಿ ((u64)hi << 32) | (u64)lo;
#ಆದರೆ_ಅಂತ್ಯ
}

ಸ್ಥಿರ ಶೂನ್ಯ plmt_timer_event_stop(ಶೂನ್ಯ)
{
	u32 ಗುರಿ_hart = current_hartid();

	ಆದರೆ (plmt.hart_ಎಣಿಕೆ <= ಗುರಿ_hart)
		ebreak();

	/* Clear PLMT Time Compare */
#ಆದರೆ __riscv_xlen == 64
	writeq_relaxed(-1ULL, &plmt.ಸಮಯ_cmp[ಗುರಿ_hart]);
#ಇಲ್ಲದಿದ್ದರೆ
	writel_relaxed(-1UL, &plmt.ಸಮಯ_cmp[ಗುರಿ_hart]);
	writel_relaxed(-1UL, (ಶೂನ್ಯ *)(&plmt.ಸಮಯ_cmp[ಗುರಿ_hart]) + 0x04);
#ಆದರೆ_ಅಂತ್ಯ
}

ಸ್ಥಿರ ಶೂನ್ಯ plmt_timer_event_ಆರಂಭ(u64 ಮುಂದಿನ_event)
{
	u32 ಗುರಿ_hart = current_hartid();

	ಆದರೆ (plmt.hart_ಎಣಿಕೆ <= ಗುರಿ_hart)
		ebreak();

	/* Program PLMT Time Compare */
#ಆದರೆ __riscv_xlen == 64
	writeq_relaxed(ಮುಂದಿನ_event, &plmt.ಸಮಯ_cmp[ಗುರಿ_hart]);
#ಇಲ್ಲದಿದ್ದರೆ
	u32 ಮುಖವಾಡ = -1UL;

	writel_relaxed(ಮುಂದಿನ_event & ಮುಖವಾಡ, &plmt.ಸಮಯ_cmp[ಗುರಿ_hart]);
	writel_relaxed(ಮುಂದಿನ_event >> 32,
		       (ಶೂನ್ಯ *)(&plmt.ಸಮಯ_cmp[ಗುರಿ_hart]) + 0x04);
#ಆದರೆ_ಅಂತ್ಯ
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ plmt_warm_timer_init(ಶೂನ್ಯ)
{
	ಆದರೆ (!plmt.ಸಮಯ_val)
		ಹಿಂತಿರುಗಿ SBI_ENODEV;

	plmt_timer_event_stop();

	ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ರಚನೆ sbi_timer_device plmt_timer = {
	.name		   = "andes_plmt",
	.timer_freq	   = DEFAULT_AE350_PLMT_FREQ,
	.timer_ಮೌಲ್ಯ	   = plmt_timer_ಮೌಲ್ಯ,
	.timer_event_ಆರಂಭ = plmt_timer_event_ಆರಂಭ,
	.timer_event_stop  = plmt_timer_event_stop,
	.warm_init	   = plmt_warm_timer_init,
};

ಪೂರ್ಣಾಂಕ plmt_cold_timer_init(ರಚನೆ plmt_ದತ್ತಾಂಶ *plmt)
{
	ಪೂರ್ಣಾಂಕ rc;

	/* Add PLMT region to the root domain */
	rc = sbi_domain_ಬೇರು_ಕೂಡಿಸು_memrange(
		(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)plmt->ಸಮಯ_val, plmt->ಗಾತ್ರ_2,
		PLMT_REGION_ALIGN,
		SBI_DOMAIN_MEMREGION_MMIO |
		SBI_DOMAIN_MEMREGION_M_READABLE |
		SBI_DOMAIN_MEMREGION_M_WRITABLE);
	ಆದರೆ (rc)
		ಹಿಂತಿರುಗಿ rc;

	plmt_timer.timer_freq = plmt->timer_freq;

	sbi_timer_ಗಣ_device(&plmt_timer);

	ಹಿಂತಿರುಗಿ 0;
}
