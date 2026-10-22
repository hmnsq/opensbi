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

#define WDT_کلید_VAL			0x16aa0000

#define WDT_SOFT_RST_REG		0x08
#define WDT_SOFT_RST_EN			BIT(0)

#define WDT_MODE_REG			0x18

ایستا فرار نویسه *sunxi_wdt_base;

ایستا صحیح sunxi_wdt_system_reset_check(u32 type, u32 reason)
{
	انتخاب (type) {
	حالت SBI_SRST_RESET_TYPE_COLD_REBOOT:
	حالت SBI_SRST_RESET_TYPE_WARM_REBOOT:
		بازگشت 1;
	}

	بازگشت 0;
}

ایستا تهی sunxi_wdt_system_reset(u32 type, u32 reason)
{
	/* Disable the watchdog. */
	writel_relaxed(WDT_کلید_VAL,
		       sunxi_wdt_base + WDT_MODE_REG);

	/* Trigger soft reset. */
	writel_relaxed(WDT_کلید_VAL | WDT_SOFT_RST_EN,
		       sunxi_wdt_base + WDT_SOFT_RST_REG);
}

ایستا ساختار sbi_system_reset_device sunxi_wdt_reset = {
	.name = "sunxi-wdt-reset",
	.system_reset_check = sunxi_wdt_system_reset_check,
	.system_reset = sunxi_wdt_system_reset,
};

ایستا صحیح sunxi_wdt_reset_init(ثابت تهی *fdt, صحیح nodeoff,
				ثابت ساختار fdt_match *match)
{
	uint64_t reg_addr;
	صحیح rc;

	rc = fdt_get_گره_addr_size(fdt, nodeoff, 0, &reg_addr, پوچ);
	اگر (rc < 0 || !reg_addr)
		بازگشت SBI_ENODEV;

	sunxi_wdt_base = (فرار نویسه *)(unsigned long)reg_addr;

	sbi_system_reset_add_device(&sunxi_wdt_reset);

	بازگشت 0;
}

ایستا ثابت ساختار fdt_match sunxi_wdt_reset_match[] = {
	{ .compatible = "allwinner,sun20i-d1-wdt-reset" },
	{ },
};

ثابت ساختار fdt_driver fdt_reset_sunxi_wdt = {
	.match_table = sunxi_wdt_reset_match,
	.init = sunxi_wdt_reset_init,
};
