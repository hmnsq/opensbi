/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Samuel Holland <samuel@sholland.org>
 */

#অন্তর্ভাব <libfdt.h>
#অন্তর্ভাব <sbi/riscv_io.h>
#অন্তর্ভাব <sbi/sbi_bitops.h>
#অন্তর্ভাব <sbi/sbi_error.h>
#অন্তর্ভাব <sbi/sbi_system.h>
#অন্তর্ভাব <sbi_utils/fdt/fdt_driver.h>
#অন্তর্ভাব <sbi_utils/fdt/fdt_helper.h>

#define WDT_চাবি_VAL			0x16aa0000

#define WDT_SOFT_RST_REG		0x08
#define WDT_SOFT_RST_EN			BIT(0)

#define WDT_MODE_REG			0x18

স্থির অস্থির অক্ষর *sunxi_wdt_base;

স্থির পূর্ণসংখ্যা sunxi_wdt_system_reset_check(u32 type, u32 reason)
{
	নির্বাচন (type) {
	ক্ষেত্র SBI_SRST_RESET_TYPE_COLD_REBOOT:
	ক্ষেত্র SBI_SRST_RESET_TYPE_WARM_REBOOT:
		ফেরত 1;
	}

	ফেরত 0;
}

স্থির শূন্য sunxi_wdt_system_reset(u32 type, u32 reason)
{
	/* Disable the watchdog. */
	writel_relaxed(WDT_চাবি_VAL,
		       sunxi_wdt_base + WDT_MODE_REG);

	/* Trigger soft reset. */
	writel_relaxed(WDT_চাবি_VAL | WDT_SOFT_RST_EN,
		       sunxi_wdt_base + WDT_SOFT_RST_REG);
}

স্থির কাঠামো sbi_system_reset_device sunxi_wdt_reset = {
	.name = "sunxi-wdt-reset",
	.system_reset_check = sunxi_wdt_system_reset_check,
	.system_reset = sunxi_wdt_system_reset,
};

স্থির পূর্ণসংখ্যা sunxi_wdt_reset_init(ধ্রুবক শূন্য *fdt, পূর্ণসংখ্যা nodeoff,
				ধ্রুবক কাঠামো fdt_match *match)
{
	uint64_t reg_addr;
	পূর্ণসংখ্যা rc;

	rc = fdt_get_গ্রন্থি_addr_আকার(fdt, nodeoff, 0, &reg_addr, ফাঁকা);
	যদি (rc < 0 || !reg_addr)
		ফেরত SBI_ENODEV;

	sunxi_wdt_base = (অস্থির অক্ষর *)(unsigned long)reg_addr;

	sbi_system_reset_add_device(&sunxi_wdt_reset);

	ফেরত 0;
}

স্থির ধ্রুবক কাঠামো fdt_match sunxi_wdt_reset_match[] = {
	{ .compatible = "allwinner,sun20i-d1-wdt-reset" },
	{ },
};

ধ্রুবক কাঠামো fdt_driver fdt_reset_sunxi_wdt = {
	.match_table = sunxi_wdt_reset_match,
	.init = sunxi_wdt_reset_init,
};
