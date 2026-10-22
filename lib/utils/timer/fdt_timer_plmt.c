/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 Andes Technology Corporation
 *
 * Authors:
 *   Yu Chien Peter Lin <peterlin@andestech.com>
 */

#شامل <sbi_utils/fdt/fdt_helper.h>
#شامل <sbi_utils/timer/fdt_timer.h>
#شامل <sbi_utils/timer/andes_plmt.h>

بیرونی ساختار plmt_data plmt;

ایستا صحیح fdt_plmt_cold_timer_init(ثابت تهی *fdt, صحیح nodeoff,
				    ثابت ساختار fdt_match *match)
{
	صحیح rc;
	unsigned long plmt_base;

	rc = fdt_parse_plmt_گره(fdt, nodeoff, &plmt_base, &plmt.size,
				 &plmt.hart_شمار);
	اگر (rc)
		بازگشت rc;

	plmt.time_val = (u64 *)plmt_base;
	plmt.time_cmp = (u64 *)(plmt_base + 0x8);

	rc = fdt_parse_timebase_frequency(fdt, &plmt.timer_freq);
	اگر (rc)
		بازگشت rc;

	rc = plmt_cold_timer_init(&plmt);
	اگر (rc)
		بازگشت rc;

	بازگشت 0;
}

ایستا ثابت ساختار fdt_match timer_plmt_match[] = {
	{ .compatible = "andestech,plmt0" },
	{},
};

ثابت ساختار fdt_driver fdt_timer_plmt = {
	.match_table = timer_plmt_match,
	.init = fdt_plmt_cold_timer_init,
};
