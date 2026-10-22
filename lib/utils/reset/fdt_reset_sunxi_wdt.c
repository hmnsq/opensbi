/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Samuel Holland <samuel@sholland.org>
 */

#અન્તર્ભાવ <libfdt.h>
#અન્તર્ભાવ <sbi/riscv_io.h>
#અન્તર્ભાવ <sbi/sbi_bitops.h>
#અન્તર્ભાવ <sbi/sbi_error.h>
#અન્તર્ભાવ <sbi/sbi_system.h>
#અન્તર્ભાવ <sbi_utils/fdt/fdt_driver.h>
#અન્તર્ભાવ <sbi_utils/fdt/fdt_helper.h>

#define WDT_ચાવી_VAL			0x16aa0000

#define WDT_SOFT_RST_REG		0x08
#define WDT_SOFT_RST_EN			BIT(0)

#define WDT_MODE_REG			0x18

સ્થિર અસ્થિર અક્ષર *sunxi_wdt_base;

સ્થિર પૂર્ણાંક sunxi_wdt_system_reset_check(u32 type, u32 reason)
{
	પસંદગી (type) {
	સ્થિતિ SBI_SRST_RESET_TYPE_COLD_REBOOT:
	સ્થિતિ SBI_SRST_RESET_TYPE_WARM_REBOOT:
		પાછા 1;
	}

	પાછા 0;
}

સ્થિર શૂન્ય sunxi_wdt_system_reset(u32 type, u32 reason)
{
	/* Disable the watchdog. */
	writel_relaxed(WDT_ચાવી_VAL,
		       sunxi_wdt_base + WDT_MODE_REG);

	/* Trigger soft reset. */
	writel_relaxed(WDT_ચાવી_VAL | WDT_SOFT_RST_EN,
		       sunxi_wdt_base + WDT_SOFT_RST_REG);
}

સ્થિર સંરચના sbi_system_reset_device sunxi_wdt_reset = {
	.name = "sunxi-wdt-reset",
	.system_reset_check = sunxi_wdt_system_reset_check,
	.system_reset = sunxi_wdt_system_reset,
};

સ્થિર પૂર્ણાંક sunxi_wdt_reset_init(અચલ શૂન્ય *fdt, પૂર્ણાંક nodeoff,
				અચલ સંરચના fdt_match *match)
{
	uint64_t reg_addr;
	પૂર્ણાંક rc;

	rc = fdt_get_ગાંઠ_addr_size(fdt, nodeoff, 0, &reg_addr, ખાલી);
	જો (rc < 0 || !reg_addr)
		પાછા SBI_ENODEV;

	sunxi_wdt_base = (અસ્થિર અક્ષર *)(unsigned long)reg_addr;

	sbi_system_reset_add_device(&sunxi_wdt_reset);

	પાછા 0;
}

સ્થિર અચલ સંરચના fdt_match sunxi_wdt_reset_match[] = {
	{ .compatible = "allwinner,sun20i-d1-wdt-reset" },
	{ },
};

અચલ સંરચના fdt_driver fdt_reset_sunxi_wdt = {
	.match_table = sunxi_wdt_reset_match,
	.init = sunxi_wdt_reset_init,
};
