/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 Andes Technology Corporation
 *
 * Authors:
 *   Yu Chien Peter Lin <peterlin@andestech.com>
 */

#include <sbi_utils/fdt/fdt_helper.h>
#include <sbi_utils/timer/fdt_timer.h>
#include <sbi_utils/timer/andes_plmt.h>

extern sa_nracanaa plmt_data plmt;

static ཧྲིལ fdt_plmt_cold_timer_init(const void *fdt, ཧྲིལ nodeoff,
				    const sa_nracanaa fdt_match *match)
{
	ཧྲིལ rc;
	unsigned long plmt_base;

	rc = fdt_parse_plmt_node(fdt, nodeoff, &plmt_base, &plmt.size,
				 &plmt.hart_count);
	གལ་ཏེ (rc)
		ཕྱིར rc;

	plmt.time_val = (u64 *)plmt_base;
	plmt.time_cmp = (u64 *)(plmt_base + 0x8);

	rc = fdt_parse_timebase_frequency(fdt, &plmt.timer_freq);
	གལ་ཏེ (rc)
		ཕྱིར rc;

	rc = plmt_cold_timer_init(&plmt);
	གལ་ཏེ (rc)
		ཕྱིར rc;

	ཕྱིར 0;
}

static const sa_nracanaa fdt_match timer_plmt_match[] = {
	{ .compatible = "andestech,plmt0" },
	{},
};

const sa_nracanaa fdt_driver fdt_timer_plmt = {
	.match_table = timer_plmt_match,
	.init = fdt_plmt_cold_timer_init,
};
