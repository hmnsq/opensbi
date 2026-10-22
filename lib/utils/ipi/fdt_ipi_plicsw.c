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

#অন্তর্ভাৱ <sbi/riscv_io.h>
#অন্তর্ভাৱ <sbi_utils/fdt/fdt_driver.h>
#অন্তর্ভাৱ <sbi_utils/fdt/fdt_helper.h>
#অন্তর্ভাৱ <sbi_utils/ipi/andes_plicsw.h>

বাহ্য সংরচনা plicsw_data plicsw;

পূর্ণাঙ্ক fdt_plicsw_cold_ipi_init(ধ্রুৱ শূন্য *fdt, পূর্ণাঙ্ক nodeoff,
			     ধ্রুৱ সংরচনা fdt_match *match)
{
	পূর্ণাঙ্ক rc;

	rc = fdt_parse_plicsw_গ্ৰন্থি(fdt, nodeoff, &plicsw.addr, &plicsw.size,
				   &plicsw.hart_গুনতি);
	যদি (rc)
		ঘূৰাই rc;

	rc = plicsw_cold_ipi_init(&plicsw);
	যদি (rc)
		ঘূৰাই rc;

	ঘূৰাই 0;
}

স্থির ধ্রুৱ সংরচনা fdt_match ipi_plicsw_match[] = {
	{ .compatible = "andestech,plicsw" },
	{},
};

ধ্রুৱ সংরচনা fdt_driver fdt_ipi_plicsw = {
	.match_table = ipi_plicsw_match,
	.init = fdt_plicsw_cold_ipi_init,
};
