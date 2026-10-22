/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2026 Metanoia Communications Inc.
 *
 */

#ضمن <sbi/riscv_io.h>
#ضمن <sbi/sbi_platform.h>
#ضمن <sbi/sbi_system.h>
#ضمن <sbi_utils/fdt/fdt_helper.h>
#ضمن <sbi_utils/fdt/fdt_driver.h>

#وصف_ڪريو SWRSTREQ_CTRL_REG_وٿي 0x00
#وصف_ڪريو SWRSTREQ_REG_وٿي 0x04

بنية reset_metanoia_معلومات {
	u8 *reg_بنياد;
};

ساكن بنية reset_metanoia_معلومات reset_معلومات;

ساكن صحيح metanoia_system_reset_check(u32 type, u32 reason)
{
	بدل (type) {
	حالة SBI_SRST_RESET_TYPE_WARM_REBOOT:
	حالة SBI_SRST_RESET_TYPE_COLD_REBOOT:
		أرجع 1;
	حالة SBI_SRST_RESET_TYPE_SHUTDOWN:
	افتراضي:
		أرجع 0;
	}
}

ساكن فراغ metanoia_system_reset(u32 type, u32 reason)
{
	writew(0x1, reset_معلومات.reg_بنياد + SWRSTREQ_CTRL_REG_وٿي);
	writew(0x1, reset_معلومات.reg_بنياد + SWRSTREQ_REG_وٿي);
}

ساكن بنية sbi_system_reset_device metanoia_reset = {
	.name		    = "metanoia-mt2824-reboot",
	.system_reset_check = metanoia_system_reset_check,
	.system_reset	    = metanoia_system_reset,
};

ساكن صحيح metanoia_reset_init(ثابت فراغ *fdt, صحيح nodeoff,
			       ثابت بنية fdt_ميلاپ *ميلاپ)
{
	u64 reg_addr;
	صحيح rc;

	rc = fdt_get_ڳنڍ_addr_سائيز(fdt, nodeoff, 0, &reg_addr, عدم);
	إذا (rc < 0 || !reg_addr)
		أرجع SBI_ENODEV;

	reset_معلومات.reg_بنياد = (u8 *)(ulong)reg_addr;

	sbi_system_reset_جوڙيو_device(&metanoia_reset);

	أرجع 0;
}

ساكن ثابت بنية fdt_ميلاپ metanoia_reset_ميلاپ[] = {
	{ .compatible = "metanoia,mt2824-reboot" },
	{ /* sentinel */ }
};

ثابت بنية fdt_driver fdt_reset_metanoia = {
	.ميلاپ_جدول = metanoia_reset_ميلاپ,
	.init	     = metanoia_reset_init,
};
