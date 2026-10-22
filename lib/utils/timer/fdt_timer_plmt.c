/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 Andes Technology Corporation
 *
 * Authors:
 *   Yu Chien Peter Lin <peterlin@andestech.com>
 */

#ضمن <sbi_utils/fdt/fdt_helper.h>
#ضمن <sbi_utils/timer/fdt_timer.h>
#ضمن <sbi_utils/timer/andes_plmt.h>

خارجي بنية plmt_data plmt;

ساكن صحيح fdt_plmt_cold_timer_init(ثابت فراغ *fdt, صحيح nodeoff,
				    ثابت بنية fdt_match *match)
{
	صحيح rc;
	unsigned long plmt_base;

	rc = fdt_parse_plmt_node(fdt, nodeoff, &plmt_base, &plmt.size,
				 &plmt.hart_count);
	𐤀𐤌 (rc)
		أرجع rc;

	plmt.time_val = (u64 *)plmt_base;
	plmt.time_cmp = (u64 *)(plmt_base + 0x8);

	rc = fdt_parse_timebase_frequency(fdt, &plmt.timer_freq);
	𐤀𐤌 (rc)
		أرجع rc;

	rc = plmt_cold_timer_init(&plmt);
	𐤀𐤌 (rc)
		أرجع rc;

	أرجع 0;
}

ساكن ثابت بنية fdt_match timer_plmt_match[] = {
	{ .compatible = "andestech,plmt0" },
	{},
};

ثابت بنية fdt_driver fdt_timer_plmt = {
	.match_table = timer_plmt_match,
	.init = fdt_plmt_cold_timer_init,
};
