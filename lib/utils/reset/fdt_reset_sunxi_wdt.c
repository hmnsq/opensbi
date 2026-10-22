/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Samuel Holland <samuel@sholland.org>
 */

#شامل <libfdt.h>
#شامل <sbi/riscv_io.h>
#شامل <sbi/sbi_bitops.h>
#شامل <sbi/sbi_error.h>
#شامل <sbi/sbi_system.h>
#شامل <sbi_utils/fdt/fdt_driver.h>
#شامل <sbi_utils/fdt/fdt_helper.h>

#تعریف_کرو WDT_کلید_VAL			0x16aa0000

#تعریف_کرو WDT_SOFT_RST_REG		0x08
#تعریف_کرو WDT_SOFT_RST_EN			ثنائی_ہندسہ(0)

#تعریف_کرو WDT_منوال_REG			0x18

ساکن متلون حرف *sunxi_wdt_بنیاد;

ساکن صحیح sunxi_wdt_system_reset_check(u32 type, u32 reason)
{
	انتخاب (type) {
	حالت SBI_SRST_RESET_TYPE_COLD_REBOOT:
	حالت SBI_SRST_RESET_TYPE_WARM_REBOOT:
		واپس 1;
	}

	واپس 0;
}

ساکن خالی sunxi_wdt_system_reset(u32 type, u32 reason)
{
	/* Disable the watchdog. */
	writel_relaxed(WDT_کلید_VAL,
		       sunxi_wdt_بنیاد + WDT_منوال_REG);

	/* Trigger soft reset. */
	writel_relaxed(WDT_کلید_VAL | WDT_SOFT_RST_EN,
		       sunxi_wdt_بنیاد + WDT_SOFT_RST_REG);
}

ساکن ساخت sbi_system_reset_device sunxi_wdt_reset = {
	.name = "sunxi-wdt-reset",
	.system_reset_check = sunxi_wdt_system_reset_check,
	.system_reset = sunxi_wdt_system_reset,
};

ساکن صحیح sunxi_wdt_reset_init(ثابت خالی *fdt, صحیح nodeoff,
				ثابت ساخت fdt_مطابقت *مطابقت)
{
	uint64_t reg_addr;
	صحیح rc;

	rc = fdt_get_گرہ_addr_سائز(fdt, nodeoff, 0, &reg_addr, تہی);
	اگر (rc < 0 || !reg_addr)
		واپس SBI_ENODEV;

	sunxi_wdt_بنیاد = (متلون حرف *)(غیرعلامتی طویل)reg_addr;

	sbi_system_reset_جوڑو_device(&sunxi_wdt_reset);

	واپس 0;
}

ساکن ثابت ساخت fdt_مطابقت sunxi_wdt_reset_مطابقت[] = {
	{ .compatible = "allwinner,sun20i-d1-wdt-reset" },
	{ },
};

ثابت ساخت fdt_driver fdt_reset_sunxi_wdt = {
	.مطابقت_جدول = sunxi_wdt_reset_مطابقت,
	.init = sunxi_wdt_reset_init,
};
