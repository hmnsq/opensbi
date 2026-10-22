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

#تعریف_کړه SWRSTREQ_CTRL_REG_دننه_واټن 0x00
#تعریف_کړه SWRSTREQ_REG_دننه_واټن 0x04

بنية reset_metanoia_معلومات {
	u8 *reg_بنسټ;
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
	writew(0x1, reset_معلومات.reg_بنسټ + SWRSTREQ_CTRL_REG_دننه_واټن);
	writew(0x1, reset_معلومات.reg_بنسټ + SWRSTREQ_REG_دننه_واټن);
}

ساكن بنية sbi_system_reset_device metanoia_reset = {
	.name		    = "metanoia-mt2824-reboot",
	.system_reset_check = metanoia_system_reset_check,
	.system_reset	    = metanoia_system_reset,
};

ساكن صحيح metanoia_reset_init(ثابت فراغ *fdt, صحيح nodeoff,
			       ثابت بنية fdt_سمون *سمون)
{
	u64 reg_addr;
	صحيح rc;

	rc = fdt_get_غوټه_addr_اندازه(fdt, nodeoff, 0, &reg_addr, عدم);
	إذا (rc < 0 || !reg_addr)
		أرجع SBI_ENODEV;

	reset_معلومات.reg_بنسټ = (u8 *)(ulong)reg_addr;

	sbi_system_reset_جمع_کړه_device(&metanoia_reset);

	أرجع 0;
}

ساكن ثابت بنية fdt_سمون metanoia_reset_سمون[] = {
	{ .compatible = "metanoia,mt2824-reboot" },
	{ /* sentinel */ }
};

ثابت بنية fdt_driver fdt_reset_metanoia = {
	.سمون_جدول = metanoia_reset_سمون,
	.init	     = metanoia_reset_init,
};
