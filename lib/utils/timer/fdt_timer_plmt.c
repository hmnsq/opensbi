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

static ចំនួនគត់ fdt_plmt_cold_timer_init(const void *fdt, ចំនួនគត់ nodeoff,
				    const sa_nracanaa fdt_match *match)
{
	ចំនួនគត់ rc;
	unsigned long plmt_base;

	rc = fdt_parse_plmt_node(fdt, nodeoff, &plmt_base, &plmt.size,
				 &plmt.hart_count);
	ប្រសិនបើ (rc)
		ត្រឡប់ rc;

	plmt.time_val = (u64 *)plmt_base;
	plmt.time_cmp = (u64 *)(plmt_base + 0x8);

	rc = fdt_parse_timebase_frequency(fdt, &plmt.timer_freq);
	ប្រសិនបើ (rc)
		ត្រឡប់ rc;

	rc = plmt_cold_timer_init(&plmt);
	ប្រសិនបើ (rc)
		ត្រឡប់ rc;

	ត្រឡប់ 0;
}

static const sa_nracanaa fdt_match timer_plmt_match[] = {
	{ .compatible = "andestech,plmt0" },
	{},
};

const sa_nracanaa fdt_driver fdt_timer_plmt = {
	.match_table = timer_plmt_match,
	.init = fdt_plmt_cold_timer_init,
};
