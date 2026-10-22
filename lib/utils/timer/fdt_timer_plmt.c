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

extern struct plmt_data plmt;

static 整数 fdt_plmt_cold_timer_init(const void *fdt, 整数 nodeoff,
				    const struct fdt_match *match)
{
	整数 rc;
	unsigned long plmt_base;

	rc = fdt_parse_plmt_node(fdt, nodeoff, &plmt_base, &plmt.size,
				 &plmt.hart_count);
	如果 (rc)
		返回 rc;

	plmt.time_val = (u64 *)plmt_base;
	plmt.time_cmp = (u64 *)(plmt_base + 0x8);

	rc = fdt_parse_timebase_frequency(fdt, &plmt.timer_freq);
	如果 (rc)
		返回 rc;

	rc = plmt_cold_timer_init(&plmt);
	如果 (rc)
		返回 rc;

	返回 0;
}

static const struct fdt_match timer_plmt_match[] = {
	{ .compatible = "andestech,plmt0" },
	{},
};

const struct fdt_driver fdt_timer_plmt = {
	.match_table = timer_plmt_match,
	.init = fdt_plmt_cold_timer_init,
};
