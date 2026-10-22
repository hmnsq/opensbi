/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Samuel Holland <samuel@sholland.org>
 */

#ಅನ್ತರ್ಭಾವ <libfdt.h>
#ಅನ್ತರ್ಭಾವ <sbi/riscv_io.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_bitops.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_error.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_system.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/fdt/fdt_driver.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/fdt/fdt_helper.h>

#define WDT_ಕೀಲಿ_VAL			0x16aa0000

#define WDT_SOFT_RST_REG		0x08
#define WDT_SOFT_RST_EN			BIT(0)

#define WDT_MODE_REG			0x18

ಸ್ಥಿರ ಚಞ್ಚಲ ವರ್ಣ *sunxi_wdt_base;

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ sunxi_wdt_system_reset_check(u32 type, u32 reason)
{
	ವಿಕಲ್ಪ (type) {
	ಪಕ್ಷ SBI_SRST_RESET_TYPE_COLD_REBOOT:
	ಪಕ್ಷ SBI_SRST_RESET_TYPE_WARM_REBOOT:
		ಪ್ರತ್ಯಾಗಮ 1;
	}

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಶೂನ್ಯ sunxi_wdt_system_reset(u32 type, u32 reason)
{
	/* Disable the watchdog. */
	writel_relaxed(WDT_ಕೀಲಿ_VAL,
		       sunxi_wdt_base + WDT_MODE_REG);

	/* Trigger soft reset. */
	writel_relaxed(WDT_ಕೀಲಿ_VAL | WDT_SOFT_RST_EN,
		       sunxi_wdt_base + WDT_SOFT_RST_REG);
}

ಸ್ಥಿರ ಸಂರಚನಾ sbi_system_reset_device sunxi_wdt_reset = {
	.name = "sunxi-wdt-reset",
	.system_reset_check = sunxi_wdt_system_reset_check,
	.system_reset = sunxi_wdt_system_reset,
};

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ sunxi_wdt_reset_init(ಧ್ರುವ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ nodeoff,
				ಧ್ರುವ ಸಂರಚನಾ fdt_match *match)
{
	uint64_t reg_addr;
	ಪೂರ್ಣಾಙ್ಕ rc;

	rc = fdt_get_ಗಂಟು_addr_size(fdt, nodeoff, 0, &reg_addr, ರಿಕ್ತ);
	ಯದಿ (rc < 0 || !reg_addr)
		ಪ್ರತ್ಯಾಗಮ SBI_ENODEV;

	sunxi_wdt_base = (ಚಞ್ಚಲ ವರ್ಣ *)(unsigned long)reg_addr;

	sbi_system_reset_add_device(&sunxi_wdt_reset);

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ fdt_match sunxi_wdt_reset_match[] = {
	{ .compatible = "allwinner,sun20i-d1-wdt-reset" },
	{ },
};

ಧ್ರುವ ಸಂರಚನಾ fdt_driver fdt_reset_sunxi_wdt = {
	.match_table = sunxi_wdt_reset_match,
	.init = sunxi_wdt_reset_init,
};
