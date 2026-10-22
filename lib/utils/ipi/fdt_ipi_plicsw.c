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

خارجي بنية plicsw_معلومات plicsw;

صحيح fdt_plicsw_cold_ipi_init(ثابت فراغ *fdt, صحيح nodeoff,
			     ثابت بنية fdt_ميلاپ *ميلاپ)
{
	صحيح rc;

	rc = fdt_parse_plicsw_ڳنڍ(fdt, nodeoff, &plicsw.addr, &plicsw.سائيز,
				   &plicsw.hart_ڳڻپ);
	إذا (rc)
		أرجع rc;

	rc = plicsw_cold_ipi_init(&plicsw);
	إذا (rc)
		أرجع rc;

	أرجع 0;
}

ساكن ثابت بنية fdt_ميلاپ ipi_plicsw_ميلاپ[] = {
	{ .compatible = "andestech,plicsw" },
	{},
};

ثابت بنية fdt_driver fdt_ipi_plicsw = {
	.ميلاپ_جدول = ipi_plicsw_ميلاپ,
	.init = fdt_plicsw_cold_ipi_init,
};
