/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Samuel Holland <samuel@sholland.org>
 */

#অন্তর্ভাৱ <libfdt.h>
#অন্তর্ভাৱ <sbi/riscv_io.h>
#অন্তর্ভাৱ <sbi/sbi_bitops.h>
#অন্তর্ভাৱ <sbi/sbi_error.h>
#অন্তর্ভাৱ <sbi/sbi_system.h>
#অন্তর্ভাৱ <sbi_utils/fdt/fdt_driver.h>
#অন্তর্ভাৱ <sbi_utils/fdt/fdt_helper.h>

#define WDT_চাবি_VAL			0x16aa0000

#define WDT_SOFT_RST_REG		0x08
#define WDT_SOFT_RST_EN			BIT(0)

#define WDT_MODE_REG			0x18

স্থির চঞ্চল ৱর্ণ *sunxi_wdt_base;

স্থির পূর্ণাঙ্ক sunxi_wdt_system_reset_check(u32 type, u32 reason)
{
	ৱিকল্প (type) {
	পক্ষ SBI_SRST_RESET_TYPE_COLD_REBOOT:
	পক্ষ SBI_SRST_RESET_TYPE_WARM_REBOOT:
		ঘূৰাই 1;
	}

	ঘূৰাই 0;
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

স্থির সংরচনা sbi_system_reset_device sunxi_wdt_reset = {
	.name = "sunxi-wdt-reset",
	.system_reset_check = sunxi_wdt_system_reset_check,
	.system_reset = sunxi_wdt_system_reset,
};

স্থির পূর্ণাঙ্ক sunxi_wdt_reset_init(ধ্রুৱ শূন্য *fdt, পূর্ণাঙ্ক nodeoff,
				ধ্রুৱ সংরচনা fdt_match *match)
{
	uint64_t reg_addr;
	পূর্ণাঙ্ক rc;

	rc = fdt_get_গ্ৰন্থি_addr_size(fdt, nodeoff, 0, &reg_addr, রিক্ত);
	যদি (rc < 0 || !reg_addr)
		ঘূৰাই SBI_ENODEV;

	sunxi_wdt_base = (চঞ্চল ৱর্ণ *)(unsigned long)reg_addr;

	sbi_system_reset_add_device(&sunxi_wdt_reset);

	ঘূৰাই 0;
}

স্থির ধ্রুৱ সংরচনা fdt_match sunxi_wdt_reset_match[] = {
	{ .compatible = "allwinner,sun20i-d1-wdt-reset" },
	{ },
};

ধ্রুৱ সংরচনা fdt_driver fdt_reset_sunxi_wdt = {
	.match_table = sunxi_wdt_reset_match,
	.init = sunxi_wdt_reset_init,
};
