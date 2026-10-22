/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 Andes Technology Corporation
 *
 * Authors:
 *   Yu Chien Peter Lin <peterlin@andestech.com>
 */

#समावेश <sbi_utils/fdt/fdt_helper.h>
#समावेश <sbi_utils/timer/fdt_timer.h>
#समावेश <sbi_utils/timer/andes_plmt.h>

बाह्य काष्ठा plmt_तथ्य plmt;

अटल पूर्णांक fdt_plmt_cold_timer_init(स्थिर व्योम *fdt, पूर्णांक nodeoff,
				    स्थिर काष्ठा fdt_match *match)
{
	पूर्णांक rc;
	अचिन्हित दीर्घ plmt_base;

	rc = fdt_parse_plmt_गाँठ(fdt, nodeoff, &plmt_base, &plmt.आकार,
				 &plmt.hart_गिनती);
	अगर (rc)
		वापस rc;

	plmt.time_val = (u64 *)plmt_base;
	plmt.time_cmp = (u64 *)(plmt_base + 0x8);

	rc = fdt_parse_timebase_frequency(fdt, &plmt.timer_freq);
	अगर (rc)
		वापस rc;

	rc = plmt_cold_timer_init(&plmt);
	अगर (rc)
		वापस rc;

	वापस 0;
}

अटल स्थिर काष्ठा fdt_match timer_plmt_match[] = {
	{ .compatible = "andestech,plmt0" },
	{},
};

स्थिर काष्ठा fdt_driver fdt_timer_plmt = {
	.match_table = timer_plmt_match,
	.init = fdt_plmt_cold_timer_init,
};
