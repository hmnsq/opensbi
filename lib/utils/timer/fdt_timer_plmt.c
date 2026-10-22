/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 Andes Technology Corporation
 *
 * Authors:
 *   Yu Chien Peter Lin <peterlin@andestech.com>
 */

#ଅନ୍ତର୍ଭାଵ <sbi_utils/fdt/fdt_helper.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/timer/fdt_timer.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/timer/andes_plmt.h>

ବାହ୍ୟ ସଂରଚନା plmt_data plmt;

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_plmt_cold_timer_init(ଅଚଳ ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nodeoff,
				    ଅଚଳ ସଂରଚନା fdt_match *match)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;
	unsigned long plmt_base;

	rc = fdt_parse_plmt_ଗଣ୍ଠି(fdt, nodeoff, &plmt_base, &plmt.size,
				 &plmt.hart_ଗଣନା);
	ଯଦି (rc)
		ଫେରନ୍ତୁ rc;

	plmt.time_val = (u64 *)plmt_base;
	plmt.time_cmp = (u64 *)(plmt_base + 0x8);

	rc = fdt_parse_timebase_frequency(fdt, &plmt.timer_freq);
	ଯଦି (rc)
		ଫେରନ୍ତୁ rc;

	rc = plmt_cold_timer_init(&plmt);
	ଯଦି (rc)
		ଫେରନ୍ତୁ rc;

	ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ଅଚଳ ସଂରଚନା fdt_match timer_plmt_match[] = {
	{ .compatible = "andestech,plmt0" },
	{},
};

ଅଚଳ ସଂରଚନା fdt_driver fdt_timer_plmt = {
	.match_table = timer_plmt_match,
	.init = fdt_plmt_cold_timer_init,
};
