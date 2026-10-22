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

#ضمن <sbi/riscv_io.h>
#ضمن <sbi_utils/fdt/fdt_driver.h>
#ضمن <sbi_utils/fdt/fdt_helper.h>
#ضمن <sbi_utils/ipi/andes_plicsw.h>

خارجي بنية plicsw_data plicsw;

صحيح fdt_plicsw_cold_ipi_init(ثابت فراغ *fdt, صحيح nodeoff,
			     ثابت بنية fdt_match *match)
{
	صحيح rc;

	rc = fdt_parse_plicsw_عقدة(fdt, nodeoff, &plicsw.addr, &plicsw.size,
				   &plicsw.hart_عدد);
	إذا (rc)
		أرجع rc;

	rc = plicsw_cold_ipi_init(&plicsw);
	إذا (rc)
		أرجع rc;

	أرجع 0;
}

ساكن ثابت بنية fdt_match ipi_plicsw_match[] = {
	{ .compatible = "andestech,plicsw" },
	{},
};

ثابت بنية fdt_driver fdt_ipi_plicsw = {
	.match_table = ipi_plicsw_match,
	.init = fdt_plicsw_cold_ipi_init,
};
