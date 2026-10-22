/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 Andes Technology Corporation
 *
 * Authors:
 *   Yu Chien Peter Lin <peterlin@andestech.com>
 */

#ಅನ್ತರ್ಭಾವ <sbi_utils/fdt/fdt_helper.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/timer/fdt_timer.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/timer/andes_plmt.h>

ಬಾಹ್ಯ ರಚನೆ plmt_ದತ್ತಾಂಶ plmt;

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ fdt_plmt_cold_timer_init(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ nodeoff,
				    ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_ಹೊಂದಿಕೆ *ಹೊಂದಿಕೆ)
{
	ಪೂರ್ಣಾಂಕ rc;
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ plmt_ತಳ;

	rc = fdt_parse_plmt_ಗಂಟು(fdt, nodeoff, &plmt_ತಳ, &plmt.ಗಾತ್ರ_2,
				 &plmt.hart_ಎಣಿಕೆ);
	ಆದರೆ (rc)
		ಹಿಂತಿರುಗಿ rc;

	plmt.ಸಮಯ_val = (u64 *)plmt_ತಳ;
	plmt.ಸಮಯ_cmp = (u64 *)(plmt_ತಳ + 0x8);

	rc = fdt_parse_timebase_frequency(fdt, &plmt.timer_freq);
	ಆದರೆ (rc)
		ಹಿಂತಿರುಗಿ rc;

	rc = plmt_cold_timer_init(&plmt);
	ಆದರೆ (rc)
		ಹಿಂತಿರುಗಿ rc;

	ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_ಹೊಂದಿಕೆ timer_plmt_ಹೊಂದಿಕೆ[] = {
	{ .compatible = "andestech,plmt0" },
	{},
};

ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_driver fdt_timer_plmt = {
	.ಹೊಂದಿಕೆ_ಕೋಷ್ಟಕ = timer_plmt_ಹೊಂದಿಕೆ,
	.init = fdt_plmt_cold_timer_init,
};
