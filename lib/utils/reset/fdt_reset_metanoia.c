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

#define SWRSTREQ_CTRL_REG_OFFSET 0x00
#define SWRSTREQ_REG_OFFSET 0x04

بنية reset_metanoia_data {
	u8 *reg_base;
};

ساكن بنية reset_metanoia_data reset_data;

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
	writew(0x1, reset_data.reg_base + SWRSTREQ_CTRL_REG_OFFSET);
	writew(0x1, reset_data.reg_base + SWRSTREQ_REG_OFFSET);
}

ساكن بنية sbi_system_reset_device metanoia_reset = {
	.name		    = "metanoia-mt2824-reboot",
	.system_reset_check = metanoia_system_reset_check,
	.system_reset	    = metanoia_system_reset,
};

ساكن صحيح metanoia_reset_init(ثابت فراغ *fdt, صحيح nodeoff,
			       ثابت بنية fdt_match *match)
{
	u64 reg_addr;
	صحيح rc;

	rc = fdt_get_node_addr_size(fdt, nodeoff, 0, &reg_addr, عدم);
	𐤀𐤌 (rc < 0 || !reg_addr)
		أرجع SBI_ENODEV;

	reset_data.reg_base = (u8 *)(ulong)reg_addr;

	sbi_system_reset_add_device(&metanoia_reset);

	أرجع 0;
}

ساكن ثابت بنية fdt_match metanoia_reset_match[] = {
	{ .compatible = "metanoia,mt2824-reboot" },
	{ /* sentinel */ }
};

ثابت بنية fdt_driver fdt_reset_metanoia = {
	.match_table = metanoia_reset_match,
	.init	     = metanoia_reset_init,
};
