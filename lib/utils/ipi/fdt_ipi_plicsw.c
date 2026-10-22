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

#অন্তর্ভাব <sbi/riscv_io.h>
#অন্তর্ভাব <sbi_utils/fdt/fdt_driver.h>
#অন্তর্ভাব <sbi_utils/fdt/fdt_helper.h>
#অন্তর্ভাব <sbi_utils/ipi/andes_plicsw.h>

বাহ্যিক কাঠামো plicsw_তথ্য plicsw;

পূর্ণসংখ্যা fdt_plicsw_cold_ipi_init(ধ্রুবক শূন্য *fdt, পূর্ণসংখ্যা nodeoff,
			     ধ্রুবক কাঠামো fdt_match *match)
{
	পূর্ণসংখ্যা rc;

	rc = fdt_parse_plicsw_গ্রন্থি(fdt, nodeoff, &plicsw.addr, &plicsw.আকার_2,
				   &plicsw.hart_গুনতি);
	যদি (rc)
		ফেরত rc;

	rc = plicsw_cold_ipi_init(&plicsw);
	যদি (rc)
		ফেরত rc;

	ফেরত 0;
}

স্থির ধ্রুবক কাঠামো fdt_match ipi_plicsw_match[] = {
	{ .compatible = "andestech,plicsw" },
	{},
};

ধ্রুবক কাঠামো fdt_driver fdt_ipi_plicsw = {
	.match_table = ipi_plicsw_match,
	.init = fdt_plicsw_cold_ipi_init,
};
