/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Samuel Holland <samuel@sholland.org>
 */

#अन्तर्भाव <libfdt.h>
#अन्तर्भाव <sbi/riscv_io.h>
#अन्तर्भाव <sbi/sbi_bitops.h>
#अन्तर्भाव <sbi/sbi_error.h>
#अन्तर्भाव <sbi/sbi_system.h>
#अन्तर्भाव <sbi_utils/fdt/fdt_driver.h>
#अन्तर्भाव <sbi_utils/fdt/fdt_helper.h>

#define WDT_किल्ली_VAL			0x16aa0000

#define WDT_SOFT_RST_REG		0x08
#define WDT_SOFT_RST_EN			BIT(0)

#define WDT_MODE_REG			0x18

स्थिर अस्थिर अक्षर *sunxi_wdt_base;

स्थिर पूर्णांक sunxi_wdt_system_reset_check(u32 type, u32 reason)
{
	निवड (type) {
	स्थिती SBI_SRST_RESET_TYPE_COLD_REBOOT:
	स्थिती SBI_SRST_RESET_TYPE_WARM_REBOOT:
		परत 1;
	}

	परत 0;
}

स्थिर शून्य sunxi_wdt_system_reset(u32 type, u32 reason)
{
	/* Disable the watchdog. */
	writel_relaxed(WDT_किल्ली_VAL,
		       sunxi_wdt_base + WDT_MODE_REG);

	/* Trigger soft reset. */
	writel_relaxed(WDT_किल्ली_VAL | WDT_SOFT_RST_EN,
		       sunxi_wdt_base + WDT_SOFT_RST_REG);
}

स्थिर संरचना sbi_system_reset_device sunxi_wdt_reset = {
	.name = "sunxi-wdt-reset",
	.system_reset_check = sunxi_wdt_system_reset_check,
	.system_reset = sunxi_wdt_system_reset,
};

स्थिर पूर्णांक sunxi_wdt_reset_init(स्थिरांक शून्य *fdt, पूर्णांक nodeoff,
				स्थिरांक संरचना fdt_match *match)
{
	uint64_t reg_addr;
	पूर्णांक rc;

	rc = fdt_get_गाठ_addr_size(fdt, nodeoff, 0, &reg_addr, रिक्त);
	जर (rc < 0 || !reg_addr)
		परत SBI_ENODEV;

	sunxi_wdt_base = (अस्थिर अक्षर *)(अचिन्हित दीर्घ)reg_addr;

	sbi_system_reset_add_device(&sunxi_wdt_reset);

	परत 0;
}

स्थिर स्थिरांक संरचना fdt_match sunxi_wdt_reset_match[] = {
	{ .compatible = "allwinner,sun20i-d1-wdt-reset" },
	{ },
};

स्थिरांक संरचना fdt_driver fdt_reset_sunxi_wdt = {
	.match_table = sunxi_wdt_reset_match,
	.init = sunxi_wdt_reset_init,
};
