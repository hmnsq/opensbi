/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 Andes Technology Corporation
 *
 * Authors:
 *   Yu Chien Peter Lin <peterlin@andestech.com>
 */

#অন্তর্ভাৱ <sbi_utils/fdt/fdt_helper.h>
#অন্তর্ভাৱ <sbi_utils/timer/fdt_timer.h>
#অন্তর্ভাৱ <sbi_utils/timer/andes_plmt.h>

বাহ্য সংরচনা plmt_data plmt;

স্থির পূর্ণাঙ্ক fdt_plmt_cold_timer_init(ধ্রুৱ শূন্য *fdt, পূর্ণাঙ্ক nodeoff,
				    ধ্রুৱ সংরচনা fdt_match *match)
{
	পূর্ণাঙ্ক rc;
	unsigned long plmt_base;

	rc = fdt_parse_plmt_গ্ৰন্থি(fdt, nodeoff, &plmt_base, &plmt.size,
				 &plmt.hart_গুনতি);
	যদি (rc)
		ঘূৰাই rc;

	plmt.time_val = (u64 *)plmt_base;
	plmt.time_cmp = (u64 *)(plmt_base + 0x8);

	rc = fdt_parse_timebase_frequency(fdt, &plmt.timer_freq);
	যদি (rc)
		ঘূৰাই rc;

	rc = plmt_cold_timer_init(&plmt);
	যদি (rc)
		ঘূৰাই rc;

	ঘূৰাই 0;
}

স্থির ধ্রুৱ সংরচনা fdt_match timer_plmt_match[] = {
	{ .compatible = "andestech,plmt0" },
	{},
};

ধ্রুৱ সংরচনা fdt_driver fdt_timer_plmt = {
	.match_table = timer_plmt_match,
	.init = fdt_plmt_cold_timer_init,
};
