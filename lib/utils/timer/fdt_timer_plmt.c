/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 Andes Technology Corporation
 *
 * Authors:
 *   Yu Chien Peter Lin <peterlin@andestech.com>
 */

#شامل <sbi_utils/fdt/fdt_helper.h>
#شامل <sbi_utils/timer/fdt_timer.h>
#شامل <sbi_utils/timer/andes_plmt.h>

خارجی ساخت plmt_ڈیٹا plmt;

ساکن صحیح fdt_plmt_cold_timer_init(ثابت خالی *fdt, صحیح nodeoff,
				    ثابت ساخت fdt_مطابقت *مطابقت)
{
	صحیح rc;
	غیرعلامتی طویل plmt_بنیاد;

	rc = fdt_parse_plmt_گرہ(fdt, nodeoff, &plmt_بنیاد, &plmt.سائز,
				 &plmt.hart_گنتی);
	اگر (rc)
		واپس rc;

	plmt.وقت_val = (u64 *)plmt_بنیاد;
	plmt.وقت_cmp = (u64 *)(plmt_بنیاد + 0x8);

	rc = fdt_parse_timebase_frequency(fdt, &plmt.timer_freq);
	اگر (rc)
		واپس rc;

	rc = plmt_cold_timer_init(&plmt);
	اگر (rc)
		واپس rc;

	واپس 0;
}

ساکن ثابت ساخت fdt_مطابقت timer_plmt_مطابقت[] = {
	{ .compatible = "andestech,plmt0" },
	{},
};

ثابت ساخت fdt_driver fdt_timer_plmt = {
	.مطابقت_جدول = timer_plmt_مطابقت,
	.init = fdt_plmt_cold_timer_init,
};
