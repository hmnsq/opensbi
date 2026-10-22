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

बाह्य संरचना plmt_दत्त plmt;

स्थिर पूर्णाङ्क fdt_plmt_cold_timer_init(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoff,
				    ध्रुव संरचना fdt_मेलनम् *मेलनम्)
{
	पूर्णाङ्क rc;
	अचिह्नित दीर्घ plmt_आधारः;

	rc = fdt_parse_plmt_ग्रन्थिः(fdt, nodeoff, &plmt_आधारः, &plmt.आकार,
				 &plmt.hart_गणनम्);
	यदि (rc)
		प्रत्यागम rc;

	plmt.time_val = (u64 *)plmt_आधारः;
	plmt.time_cmp = (u64 *)(plmt_आधारः + 0x8);

	rc = fdt_parse_timebase_frequency(fdt, &plmt.timer_freq);
	यदि (rc)
		प्रत्यागम rc;

	rc = plmt_cold_timer_init(&plmt);
	यदि (rc)
		प्रत्यागम rc;

	प्रत्यागम 0;
}

स्थिर ध्रुव संरचना fdt_मेलनम् timer_plmt_मेलनम्[] = {
	{ .compatible = "andestech,plmt0" },
	{},
};

ध्रुव संरचना fdt_driver fdt_timer_plmt = {
	.मेलनम्_सारणी = timer_plmt_मेलनम्,
	.init = fdt_plmt_cold_timer_init,
};
