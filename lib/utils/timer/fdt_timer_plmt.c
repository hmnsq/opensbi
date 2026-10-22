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

خارجي بنية plmt_معلومات plmt;

ساكن صحيح fdt_plmt_cold_timer_init(ثابت فراغ *fdt, صحيح nodeoff,
				    ثابت بنية fdt_ميلاپ *ميلاپ)
{
	صحيح rc;
	بنانشان ڊگهو plmt_بنياد;

	rc = fdt_parse_plmt_ڳنڍ(fdt, nodeoff, &plmt_بنياد, &plmt.سائيز,
				 &plmt.hart_ڳڻپ);
	إذا (rc)
		أرجع rc;

	plmt.وقت_val = (u64 *)plmt_بنياد;
	plmt.وقت_cmp = (u64 *)(plmt_بنياد + 0x8);

	rc = fdt_parse_timebase_frequency(fdt, &plmt.timer_freq);
	إذا (rc)
		أرجع rc;

	rc = plmt_cold_timer_init(&plmt);
	إذا (rc)
		أرجع rc;

	أرجع 0;
}

ساكن ثابت بنية fdt_ميلاپ timer_plmt_ميلاپ[] = {
	{ .compatible = "andestech,plmt0" },
	{},
};

ثابت بنية fdt_driver fdt_timer_plmt = {
	.ميلاپ_جدول = timer_plmt_ميلاپ,
	.init = fdt_plmt_cold_timer_init,
};
