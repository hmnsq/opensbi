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

ಬಾಹ್ಯ ಸಂರಚನಾ plmt_data plmt;

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ fdt_plmt_cold_timer_init(ಧ್ರುವ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ nodeoff,
				    ಧ್ರುವ ಸಂರಚನಾ fdt_match *match)
{
	ಪೂರ್ಣಾಙ್ಕ rc;
	unsigned long plmt_base;

	rc = fdt_parse_plmt_ಗಂಟು(fdt, nodeoff, &plmt_base, &plmt.size,
				 &plmt.hart_ಎಣಿಕೆ);
	ಯದಿ (rc)
		ಪ್ರತ್ಯಾಗಮ rc;

	plmt.time_val = (u64 *)plmt_base;
	plmt.time_cmp = (u64 *)(plmt_base + 0x8);

	rc = fdt_parse_timebase_frequency(fdt, &plmt.timer_freq);
	ಯದಿ (rc)
		ಪ್ರತ್ಯಾಗಮ rc;

	rc = plmt_cold_timer_init(&plmt);
	ಯದಿ (rc)
		ಪ್ರತ್ಯಾಗಮ rc;

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ fdt_match timer_plmt_match[] = {
	{ .compatible = "andestech,plmt0" },
	{},
};

ಧ್ರುವ ಸಂರಚನಾ fdt_driver fdt_timer_plmt = {
	.match_table = timer_plmt_match,
	.init = fdt_plmt_cold_timer_init,
};
