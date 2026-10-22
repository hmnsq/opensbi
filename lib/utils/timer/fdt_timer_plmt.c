/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 Andes Technology Corporation
 *
 * Authors:
 *   Yu Chien Peter Lin <peterlin@andestech.com>
 */

#અન્તર્ભાવ <sbi_utils/fdt/fdt_helper.h>
#અન્તર્ભાવ <sbi_utils/timer/fdt_timer.h>
#અન્તર્ભાવ <sbi_utils/timer/andes_plmt.h>

બાહ્ય સંરચના plmt_data plmt;

સ્થિર પૂર્ણાંક fdt_plmt_cold_timer_init(અચલ શૂન્ય *fdt, પૂર્ણાંક nodeoff,
				    અચલ સંરચના fdt_match *match)
{
	પૂર્ણાંક rc;
	unsigned long plmt_base;

	rc = fdt_parse_plmt_ગાંઠ(fdt, nodeoff, &plmt_base, &plmt.size,
				 &plmt.hart_ગણતરી);
	જો (rc)
		પાછા rc;

	plmt.time_val = (u64 *)plmt_base;
	plmt.time_cmp = (u64 *)(plmt_base + 0x8);

	rc = fdt_parse_timebase_frequency(fdt, &plmt.timer_freq);
	જો (rc)
		પાછા rc;

	rc = plmt_cold_timer_init(&plmt);
	જો (rc)
		પાછા rc;

	પાછા 0;
}

સ્થિર અચલ સંરચના fdt_match timer_plmt_match[] = {
	{ .compatible = "andestech,plmt0" },
	{},
};

અચલ સંરચના fdt_driver fdt_timer_plmt = {
	.match_table = timer_plmt_match,
	.init = fdt_plmt_cold_timer_init,
};
