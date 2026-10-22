/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 Andes Technology Corporation
 *
 * Authors:
 *   Yu Chien Peter Lin <peterlin@andestech.com>
 */

#அந்தர்பாவ <sbi_utils/fdt/fdt_helper.h>
#அந்தர்பாவ <sbi_utils/timer/fdt_timer.h>
#அந்தர்பாவ <sbi_utils/timer/andes_plmt.h>

வெளி கட்டமைப்பு plmt_data plmt;

நிலையான முழு fdt_plmt_cold_timer_init(மாறிலி வெறுமை *fdt, முழு nodeoff,
				    மாறிலி கட்டமைப்பு fdt_match *match)
{
	முழு rc;
	unsigned long plmt_base;

	rc = fdt_parse_plmt_முனை(fdt, nodeoff, &plmt_base, &plmt.size,
				 &plmt.hart_மொத்தம்);
	எனில் (rc)
		திரும்பு rc;

	plmt.time_val = (u64 *)plmt_base;
	plmt.time_cmp = (u64 *)(plmt_base + 0x8);

	rc = fdt_parse_timebase_frequency(fdt, &plmt.timer_freq);
	எனில் (rc)
		திரும்பு rc;

	rc = plmt_cold_timer_init(&plmt);
	எனில் (rc)
		திரும்பு rc;

	திரும்பு 0;
}

நிலையான மாறிலி கட்டமைப்பு fdt_match timer_plmt_match[] = {
	{ .compatible = "andestech,plmt0" },
	{},
};

மாறிலி கட்டமைப்பு fdt_driver fdt_timer_plmt = {
	.match_table = timer_plmt_match,
	.init = fdt_plmt_cold_timer_init,
};
