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

#ವ್ಯಾಖ್ಯಾನಿಸು WDT_ಕೀಲಿ_VAL			0x16aa0000

#ವ್ಯಾಖ್ಯಾನಿಸು WDT_SOFT_RST_REG		0x08
#ವ್ಯಾಖ್ಯಾನಿಸು WDT_SOFT_RST_EN			ಬಿಟ್(0)

#ವ್ಯಾಖ್ಯಾನಿಸು WDT_ಬಹುಲಕ_REG			0x18

ಸ್ಥಿರ ಅಸ್ಥಿರ ಅಕ್ಷರ *sunxi_wdt_ತಳ;

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ sunxi_wdt_system_reset_check(u32 type, u32 reason)
{
	ಆಯ್ಕೆ (type) {
	ಸ್ಥಿತಿ SBI_SRST_RESET_TYPE_COLD_REBOOT:
	ಸ್ಥಿತಿ SBI_SRST_RESET_TYPE_WARM_REBOOT:
		ಹಿಂತಿರುಗಿ 1;
	}

	ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಶೂನ್ಯ sunxi_wdt_system_reset(u32 type, u32 reason)
{
	/* Disable the watchdog. */
	writel_relaxed(WDT_ಕೀಲಿ_VAL,
		       sunxi_wdt_ತಳ + WDT_ಬಹುಲಕ_REG);

	/* Trigger soft reset. */
	writel_relaxed(WDT_ಕೀಲಿ_VAL | WDT_SOFT_RST_EN,
		       sunxi_wdt_ತಳ + WDT_SOFT_RST_REG);
}

ಸ್ಥಿರ ರಚನೆ sbi_system_reset_device sunxi_wdt_reset = {
	.name = "sunxi-wdt-reset",
	.system_reset_check = sunxi_wdt_system_reset_check,
	.system_reset = sunxi_wdt_system_reset,
};

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ sunxi_wdt_reset_init(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ nodeoff,
				ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_ಹೊಂದಿಕೆ *ಹೊಂದಿಕೆ)
{
	uint64_t reg_addr;
	ಪೂರ್ಣಾಂಕ rc;

	rc = fdt_get_ಗಂಟು_addr_ಗಾತ್ರ(fdt, nodeoff, 0, &reg_addr, ಖಾಲಿ);
	ಆದರೆ (rc < 0 || !reg_addr)
		ಹಿಂತಿರುಗಿ SBI_ENODEV;

	sunxi_wdt_ತಳ = (ಅಸ್ಥಿರ ಅಕ್ಷರ *)(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)reg_addr;

	sbi_system_reset_ಕೂಡಿಸು_device(&sunxi_wdt_reset);

	ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_ಹೊಂದಿಕೆ sunxi_wdt_reset_ಹೊಂದಿಕೆ[] = {
	{ .compatible = "allwinner,sun20i-d1-wdt-reset" },
	{ },
};

ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_driver fdt_reset_sunxi_wdt = {
	.ಹೊಂದಿಕೆ_ಕೋಷ್ಟಕ = sunxi_wdt_reset_ಹೊಂದಿಕೆ,
	.init = sunxi_wdt_reset_init,
};
