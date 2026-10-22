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

خارجي بنية plmt_انکڑے plmt;

ساكن صحيح fdt_plmt_cold_timer_init(ثابت فراغ *fdt, صحيح nodeoff,
				    ثابت بنية fdt_میل *میل)
{
	صحيح rc;
	بنانشانی لما plmt_ادھار;

	rc = fdt_parse_plmt_گرہ(fdt, nodeoff, &plmt_ادھار, &plmt.ناپ,
				 &plmt.hart_گنتی);
	إذا (rc)
		أرجع rc;

	plmt.ویلا_val = (u64 *)plmt_ادھار;
	plmt.ویلا_cmp = (u64 *)(plmt_ادھار + 0x8);

	rc = fdt_parse_timebase_frequency(fdt, &plmt.timer_freq);
	إذا (rc)
		أرجع rc;

	rc = plmt_cold_timer_init(&plmt);
	إذا (rc)
		أرجع rc;

	أرجع 0;
}

ساكن ثابت بنية fdt_میل timer_plmt_میل[] = {
	{ .compatible = "andestech,plmt0" },
	{},
};

ثابت بنية fdt_driver fdt_timer_plmt = {
	.میل_جدول = timer_plmt_میل,
	.init = fdt_plmt_cold_timer_init,
};
