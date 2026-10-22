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

#تعریف_کرو SWRSTREQ_CTRL_REG_انحراف 0x00
#تعریف_کرو SWRSTREQ_REG_انحراف 0x04

ساخت reset_metanoia_ڈیٹا {
	u8 *reg_بنیاد;
};

ساکن ساخت reset_metanoia_ڈیٹا reset_ڈیٹا;

ساکن صحیح metanoia_system_reset_check(u32 type, u32 reason)
{
	انتخاب (type) {
	حالت SBI_SRST_RESET_TYPE_WARM_REBOOT:
	حالت SBI_SRST_RESET_TYPE_COLD_REBOOT:
		واپس 1;
	حالت SBI_SRST_RESET_TYPE_SHUTDOWN:
	طےشدہ:
		واپس 0;
	}
}

ساکن خالی metanoia_system_reset(u32 type, u32 reason)
{
	writew(0x1, reset_ڈیٹا.reg_بنیاد + SWRSTREQ_CTRL_REG_انحراف);
	writew(0x1, reset_ڈیٹا.reg_بنیاد + SWRSTREQ_REG_انحراف);
}

ساکن ساخت sbi_system_reset_device metanoia_reset = {
	.name		    = "metanoia-mt2824-reboot",
	.system_reset_check = metanoia_system_reset_check,
	.system_reset	    = metanoia_system_reset,
};

ساکن صحیح metanoia_reset_init(ثابت خالی *fdt, صحیح nodeoff,
			       ثابت ساخت fdt_مطابقت *مطابقت)
{
	u64 reg_addr;
	صحیح rc;

	rc = fdt_get_گرہ_addr_سائز(fdt, nodeoff, 0, &reg_addr, تہی);
	اگر (rc < 0 || !reg_addr)
		واپس SBI_ENODEV;

	reset_ڈیٹا.reg_بنیاد = (u8 *)(ulong)reg_addr;

	sbi_system_reset_جوڑو_device(&metanoia_reset);

	واپس 0;
}

ساکن ثابت ساخت fdt_مطابقت metanoia_reset_مطابقت[] = {
	{ .compatible = "metanoia,mt2824-reboot" },
	{ /* sentinel */ }
};

ثابت ساخت fdt_driver fdt_reset_metanoia = {
	.مطابقت_جدول = metanoia_reset_مطابقت,
	.init	     = metanoia_reset_init,
};
