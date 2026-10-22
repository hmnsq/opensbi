/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Samuel Holland <samuel@sholland.org>
 */

#அந்தர்பாவ <libfdt.h>
#அந்தர்பாவ <sbi/riscv_io.h>
#அந்தர்பாவ <sbi/sbi_bitops.h>
#அந்தர்பாவ <sbi/sbi_error.h>
#அந்தர்பாவ <sbi/sbi_system.h>
#அந்தர்பாவ <sbi_utils/fdt/fdt_driver.h>
#அந்தர்பாவ <sbi_utils/fdt/fdt_helper.h>

#define WDT_சாவி_VAL			0x16aa0000

#define WDT_SOFT_RST_REG		0x08
#define WDT_SOFT_RST_EN			BIT(0)

#define WDT_MODE_REG			0x18

நிலையான நிலையற்ற எழுத்து *sunxi_wdt_base;

நிலையான முழு sunxi_wdt_system_reset_check(u32 type, u32 reason)
{
	தேர்வு (type) {
	நிலை SBI_SRST_RESET_TYPE_COLD_REBOOT:
	நிலை SBI_SRST_RESET_TYPE_WARM_REBOOT:
		திரும்பு 1;
	}

	திரும்பு 0;
}

நிலையான வெறுமை sunxi_wdt_system_reset(u32 type, u32 reason)
{
	/* Disable the watchdog. */
	writel_relaxed(WDT_சாவி_VAL,
		       sunxi_wdt_base + WDT_MODE_REG);

	/* Trigger soft reset. */
	writel_relaxed(WDT_சாவி_VAL | WDT_SOFT_RST_EN,
		       sunxi_wdt_base + WDT_SOFT_RST_REG);
}

நிலையான கட்டமைப்பு sbi_system_reset_device sunxi_wdt_reset = {
	.name = "sunxi-wdt-reset",
	.system_reset_check = sunxi_wdt_system_reset_check,
	.system_reset = sunxi_wdt_system_reset,
};

நிலையான முழு sunxi_wdt_reset_init(மாறிலி வெறுமை *fdt, முழு nodeoff,
				மாறிலி கட்டமைப்பு fdt_match *match)
{
	uint64_t reg_addr;
	முழு rc;

	rc = fdt_get_முனை_addr_size(fdt, nodeoff, 0, &reg_addr, வெற்று);
	எனில் (rc < 0 || !reg_addr)
		திரும்பு SBI_ENODEV;

	sunxi_wdt_base = (நிலையற்ற எழுத்து *)(unsigned long)reg_addr;

	sbi_system_reset_add_device(&sunxi_wdt_reset);

	திரும்பு 0;
}

நிலையான மாறிலி கட்டமைப்பு fdt_match sunxi_wdt_reset_match[] = {
	{ .compatible = "allwinner,sun20i-d1-wdt-reset" },
	{ },
};

மாறிலி கட்டமைப்பு fdt_driver fdt_reset_sunxi_wdt = {
	.match_table = sunxi_wdt_reset_match,
	.init = sunxi_wdt_reset_init,
};
