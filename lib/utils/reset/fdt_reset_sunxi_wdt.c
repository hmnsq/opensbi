/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Samuel Holland <samuel@sholland.org>
 */

#അന്തര്ഭാവ <libfdt.h>
#അന്തര്ഭാവ <sbi/riscv_io.h>
#അന്തര്ഭാവ <sbi/sbi_bitops.h>
#അന്തര്ഭാവ <sbi/sbi_error.h>
#അന്തര്ഭാവ <sbi/sbi_system.h>
#അന്തര്ഭാവ <sbi_utils/fdt/fdt_driver.h>
#അന്തര്ഭാവ <sbi_utils/fdt/fdt_helper.h>

#define WDT_താക്കോൽ_VAL			0x16aa0000

#define WDT_SOFT_RST_REG		0x08
#define WDT_SOFT_RST_EN			BIT(0)

#define WDT_MODE_REG			0x18

സ്ഥിതം അസ്ഥിര അക്ഷരം *sunxi_wdt_base;

സ്ഥിതം പൂർണ്ണസംഖ്യ sunxi_wdt_system_reset_check(u32 type, u32 reason)
{
	തിരഞ്ഞെടുപ്പ് (type) {
	അവസ്ഥ SBI_SRST_RESET_TYPE_COLD_REBOOT:
	അവസ്ഥ SBI_SRST_RESET_TYPE_WARM_REBOOT:
		തിരികെ 1;
	}

	തിരികെ 0;
}

സ്ഥിതം ശൂന്യം sunxi_wdt_system_reset(u32 type, u32 reason)
{
	/* Disable the watchdog. */
	writel_relaxed(WDT_താക്കോൽ_VAL,
		       sunxi_wdt_base + WDT_MODE_REG);

	/* Trigger soft reset. */
	writel_relaxed(WDT_താക്കോൽ_VAL | WDT_SOFT_RST_EN,
		       sunxi_wdt_base + WDT_SOFT_RST_REG);
}

സ്ഥിതം ഘടന sbi_system_reset_device sunxi_wdt_reset = {
	.name = "sunxi-wdt-reset",
	.system_reset_check = sunxi_wdt_system_reset_check,
	.system_reset = sunxi_wdt_system_reset,
};

സ്ഥിതം പൂർണ്ണസംഖ്യ sunxi_wdt_reset_init(സ്ഥിരം ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ nodeoff,
				സ്ഥിരം ഘടന fdt_match *match)
{
	uint64_t reg_addr;
	പൂർണ്ണസംഖ്യ rc;

	rc = fdt_get_കണ്ണി_addr_size(fdt, nodeoff, 0, &reg_addr, ശൂന്യ);
	എങ്കിൽ (rc < 0 || !reg_addr)
		തിരികെ SBI_ENODEV;

	sunxi_wdt_base = (അസ്ഥിര അക്ഷരം *)(unsigned long)reg_addr;

	sbi_system_reset_add_device(&sunxi_wdt_reset);

	തിരികെ 0;
}

സ്ഥിതം സ്ഥിരം ഘടന fdt_match sunxi_wdt_reset_match[] = {
	{ .compatible = "allwinner,sun20i-d1-wdt-reset" },
	{ },
};

സ്ഥിരം ഘടന fdt_driver fdt_reset_sunxi_wdt = {
	.match_table = sunxi_wdt_reset_match,
	.init = sunxi_wdt_reset_init,
};
