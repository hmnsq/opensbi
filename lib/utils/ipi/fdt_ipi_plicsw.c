/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 Andes Technology Corporation
 *
 * Authors:
 *   Zong Li <zong@andestech.com>
 *   Nylon Chen <nylon7@andestech.com>
 *   Leo Yu-Chi Liang <ycliang@andestech.com>
 *   Yu Chien Peter Lin <peterlin@andestech.com>
 */

#شامل <sbi/riscv_io.h>
#شامل <sbi_utils/fdt/fdt_driver.h>
#شامل <sbi_utils/fdt/fdt_helper.h>
#شامل <sbi_utils/ipi/andes_plicsw.h>

خارجی ساخت plicsw_ڈیٹا plicsw;

صحیح fdt_plicsw_cold_ipi_init(ثابت خالی *fdt, صحیح nodeoff,
			     ثابت ساخت fdt_مطابقت *مطابقت)
{
	صحیح rc;

	rc = fdt_parse_plicsw_گرہ(fdt, nodeoff, &plicsw.addr, &plicsw.سائز,
				   &plicsw.hart_گنتی);
	اگر (rc)
		واپس rc;

	rc = plicsw_cold_ipi_init(&plicsw);
	اگر (rc)
		واپس rc;

	واپس 0;
}

ساکن ثابت ساخت fdt_مطابقت ipi_plicsw_مطابقت[] = {
	{ .compatible = "andestech,plicsw" },
	{},
};

ثابت ساخت fdt_driver fdt_ipi_plicsw = {
	.مطابقت_جدول = ipi_plicsw_مطابقت,
	.init = fdt_plicsw_cold_ipi_init,
};
