/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 Andes Technology Corporation
 *
 * Authors:
 *   Yu Chien Peter Lin <peterlin@andestech.com>
 */

#অন্তর্ভাব <sbi_utils/fdt/fdt_helper.h>
#অন্তর্ভাব <sbi_utils/timer/fdt_timer.h>
#অন্তর্ভাব <sbi_utils/timer/andes_plmt.h>

বাহ্যিক কাঠামো plmt_তথ্য plmt;

স্থির পূর্ণসংখ্যা fdt_plmt_cold_timer_init(ধ্রুবক শূন্য *fdt, পূর্ণসংখ্যা nodeoff,
				    ধ্রুবক কাঠামো fdt_match *match)
{
	পূর্ণসংখ্যা rc;
	unsigned long plmt_base;

	rc = fdt_parse_plmt_গ্রন্থি(fdt, nodeoff, &plmt_base, &plmt.আকার_2,
				 &plmt.hart_গুনতি);
	যদি (rc)
		ফেরত rc;

	plmt.time_val = (u64 *)plmt_base;
	plmt.time_cmp = (u64 *)(plmt_base + 0x8);

	rc = fdt_parse_timebase_frequency(fdt, &plmt.timer_freq);
	যদি (rc)
		ফেরত rc;

	rc = plmt_cold_timer_init(&plmt);
	যদি (rc)
		ফেরত rc;

	ফেরত 0;
}

স্থির ধ্রুবক কাঠামো fdt_match timer_plmt_match[] = {
	{ .compatible = "andestech,plmt0" },
	{},
};

ধ্রুবক কাঠামো fdt_driver fdt_timer_plmt = {
	.match_table = timer_plmt_match,
	.init = fdt_plmt_cold_timer_init,
};
