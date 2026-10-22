/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 Andes Technology Corporation
 *
 * Authors:
 *   Yu Chien Peter Lin <peterlin@andestech.com>
 */

#അന്തര്ഭാവ <sbi_utils/fdt/fdt_helper.h>
#അന്തര്ഭാവ <sbi_utils/timer/fdt_timer.h>
#അന്തര്ഭാവ <sbi_utils/timer/andes_plmt.h>

ബാഹ്യ ഘടന plmt_data plmt;

സ്ഥിതം പൂർണ്ണസംഖ്യ fdt_plmt_cold_timer_init(സ്ഥിരം ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ nodeoff,
				    സ്ഥിരം ഘടന fdt_match *match)
{
	പൂർണ്ണസംഖ്യ rc;
	unsigned long plmt_base;

	rc = fdt_parse_plmt_കണ്ണി(fdt, nodeoff, &plmt_base, &plmt.size,
				 &plmt.hart_എണ്ണം);
	എങ്കിൽ (rc)
		തിരികെ rc;

	plmt.time_val = (u64 *)plmt_base;
	plmt.time_cmp = (u64 *)(plmt_base + 0x8);

	rc = fdt_parse_timebase_frequency(fdt, &plmt.timer_freq);
	എങ്കിൽ (rc)
		തിരികെ rc;

	rc = plmt_cold_timer_init(&plmt);
	എങ്കിൽ (rc)
		തിരികെ rc;

	തിരികെ 0;
}

സ്ഥിതം സ്ഥിരം ഘടന fdt_match timer_plmt_match[] = {
	{ .compatible = "andestech,plmt0" },
	{},
};

സ്ഥിരം ഘടന fdt_driver fdt_timer_plmt = {
	.match_table = timer_plmt_match,
	.init = fdt_plmt_cold_timer_init,
};
