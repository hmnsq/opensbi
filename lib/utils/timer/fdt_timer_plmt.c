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

خارجي بنية plmt_داده plmt;

ساكن صحيح fdt_plmt_cold_timer_init(ثابت فراغ *fdt, صحيح nodeoff,
				    ثابت بنية fdt_تطابق *تطابق)
{
	صحيح rc;
	بیعلامت دراز plmt_پایه;

	rc = fdt_parse_plmt_گره(fdt, nodeoff, &plmt_پایه, &plmt.اندازه,
				 &plmt.hart_شمار);
	إذا (rc)
		أرجع rc;

	plmt.زمان_val = (u64 *)plmt_پایه;
	plmt.زمان_cmp = (u64 *)(plmt_پایه + 0x8);

	rc = fdt_parse_timebase_frequency(fdt, &plmt.timer_freq);
	إذا (rc)
		أرجع rc;

	rc = plmt_cold_timer_init(&plmt);
	إذا (rc)
		أرجع rc;

	أرجع 0;
}

ساكن ثابت بنية fdt_تطابق timer_plmt_تطابق[] = {
	{ .compatible = "andestech,plmt0" },
	{},
};

ثابت بنية fdt_driver fdt_timer_plmt = {
	.تطابق_جدول = timer_plmt_تطابق,
	.init = fdt_plmt_cold_timer_init,
};
