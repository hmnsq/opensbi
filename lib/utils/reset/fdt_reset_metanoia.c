/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2026 Metanoia Communications Inc.
 *
 */

#شامل <sbi/riscv_io.h>
#شامل <sbi/sbi_platform.h>
#شامل <sbi/sbi_system.h>
#شامل <sbi_utils/fdt/fdt_helper.h>
#شامل <sbi_utils/fdt/fdt_driver.h>

#define SWRSTREQ_CTRL_REG_OFFSET 0x00
#define SWRSTREQ_REG_OFFSET 0x04

ساختار reset_metanoia_data {
	u8 *reg_base;
};

ایستا ساختار reset_metanoia_data reset_data;

ایستا صحیح metanoia_system_reset_check(u32 type, u32 reason)
{
	انتخاب (type) {
	حالت SBI_SRST_RESET_TYPE_WARM_REBOOT:
	حالت SBI_SRST_RESET_TYPE_COLD_REBOOT:
		بازگشت 1;
	حالت SBI_SRST_RESET_TYPE_SHUTDOWN:
	پیشفرض:
		بازگشت 0;
	}
}

ایستا تهی metanoia_system_reset(u32 type, u32 reason)
{
	writew(0x1, reset_data.reg_base + SWRSTREQ_CTRL_REG_OFFSET);
	writew(0x1, reset_data.reg_base + SWRSTREQ_REG_OFFSET);
}

ایستا ساختار sbi_system_reset_device metanoia_reset = {
	.name		    = "metanoia-mt2824-reboot",
	.system_reset_check = metanoia_system_reset_check,
	.system_reset	    = metanoia_system_reset,
};

ایستا صحیح metanoia_reset_init(ثابت تهی *fdt, صحیح nodeoff,
			       ثابت ساختار fdt_match *match)
{
	u64 reg_addr;
	صحیح rc;

	rc = fdt_get_گره_addr_size(fdt, nodeoff, 0, &reg_addr, پوچ);
	اگر (rc < 0 || !reg_addr)
		بازگشت SBI_ENODEV;

	reset_data.reg_base = (u8 *)(ulong)reg_addr;

	sbi_system_reset_add_device(&metanoia_reset);

	بازگشت 0;
}

ایستا ثابت ساختار fdt_match metanoia_reset_match[] = {
	{ .compatible = "metanoia,mt2824-reboot" },
	{ /* sentinel */ }
};

ثابت ساختار fdt_driver fdt_reset_metanoia = {
	.match_table = metanoia_reset_match,
	.init	     = metanoia_reset_init,
};
