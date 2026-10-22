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

بیرونی ساختار plicsw_data plicsw;

صحیح fdt_plicsw_cold_ipi_init(ثابت تهی *fdt, صحیح nodeoff,
			     ثابت ساختار fdt_match *match)
{
	صحیح rc;

	rc = fdt_parse_plicsw_گره(fdt, nodeoff, &plicsw.addr, &plicsw.size,
				   &plicsw.hart_شمار);
	اگر (rc)
		بازگشت rc;

	rc = plicsw_cold_ipi_init(&plicsw);
	اگر (rc)
		بازگشت rc;

	بازگشت 0;
}

ایستا ثابت ساختار fdt_match ipi_plicsw_match[] = {
	{ .compatible = "andestech,plicsw" },
	{},
};

ثابت ساختار fdt_driver fdt_ipi_plicsw = {
	.match_table = ipi_plicsw_match,
	.init = fdt_plicsw_cold_ipi_init,
};
