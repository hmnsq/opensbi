/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 Andes Technology Corporation
 *
 * Authors:
 *   Yu Chien Peter Lin <peterlin@andestech.com>
 */

#अन्तर्भाव <sbi_utils/fdt/fdt_helper.h>
#अन्तर्भाव <sbi_utils/timer/fdt_timer.h>
#अन्तर्भाव <sbi_utils/timer/andes_plmt.h>

बाह्य संरचना plmt_data plmt;

स्थिर पूर्णांक fdt_plmt_cold_timer_init(स्थिरांक शून्य *fdt, पूर्णांक nodeoff,
				    स्थिरांक संरचना fdt_match *match)
{
	पूर्णांक rc;
	अचिन्हित दीर्घ plmt_base;

	rc = fdt_parse_plmt_गाठ(fdt, nodeoff, &plmt_base, &plmt.size,
				 &plmt.hart_मोजणी);
	जर (rc)
		परत rc;

	plmt.time_val = (u64 *)plmt_base;
	plmt.time_cmp = (u64 *)(plmt_base + 0x8);

	rc = fdt_parse_timebase_frequency(fdt, &plmt.timer_freq);
	जर (rc)
		परत rc;

	rc = plmt_cold_timer_init(&plmt);
	जर (rc)
		परत rc;

	परत 0;
}

स्थिर स्थिरांक संरचना fdt_match timer_plmt_match[] = {
	{ .compatible = "andestech,plmt0" },
	{},
};

स्थिरांक संरचना fdt_driver fdt_timer_plmt = {
	.match_table = timer_plmt_match,
	.init = fdt_plmt_cold_timer_init,
};
