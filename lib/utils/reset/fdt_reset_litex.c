/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2026 Inochi Amaoto <inochiama@gmail.com>
 */

#ضمن <libfdt.h>
#ضمن <sbi/riscv_io.h>
#ضمن <sbi/sbi_error.h>
#ضمن <sbi/sbi_system.h>
#ضمن <sbi_utils/fdt/fdt_driver.h>
#ضمن <sbi_utils/fdt/fdt_helper.h>

#وصف_ڪريو RESET_CTRL			0x0

ساكن متطاير u32 *litex_soc_بنياد;

ساكن صحيح litex_reset_check(u32 type, u32 reason)
{
	بدل (type) {
	حالة SBI_SRST_RESET_TYPE_COLD_REBOOT:
	حالة SBI_SRST_RESET_TYPE_WARM_REBOOT:
		أرجع 255;
	}

	أرجع 0;
}

ساكن فراغ litex_do_reset(u32 type, u32 reason)
{
	writel_relaxed(0x1, litex_soc_بنياد + RESET_CTRL);
}

ساكن بنية sbi_system_reset_device litex_reset = {
	.name = "litex-reset",
	.system_reset_check = litex_reset_check,
	.system_reset = litex_do_reset
};

ساكن صحيح litex_reset_init(ثابت فراغ *fdt, صحيح nodeoff,
			    ثابت بنية fdt_ميلاپ *ميلاپ)
{
	uint64_t reg_addr;
	صحيح rc;

	rc = fdt_get_ڳنڍ_addr_سائيز(fdt, nodeoff, 0, &reg_addr, عدم);
	إذا (rc < 0 || !reg_addr)
		أرجع SBI_ENODEV;


	litex_soc_بنياد = (متطاير u32 *)(بنانشان ڊگهو)reg_addr;

	sbi_system_reset_جوڙيو_device(&litex_reset);

	أرجع 0;
}

ساكن ثابت بنية fdt_ميلاپ litex_reset_ميلاپ[] = {
	{ .compatible = "litex,soc-controller" },
	{ },
};

ثابت بنية fdt_driver fdt_reset_litex = {
	.ميلاپ_جدول = litex_reset_ميلاپ,
	.init = litex_reset_init,
};
