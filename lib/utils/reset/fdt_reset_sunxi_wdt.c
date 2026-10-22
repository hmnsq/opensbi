/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Samuel Holland <samuel@sholland.org>
 */

#समावेश <libfdt.h>
#समावेश <sbi/riscv_io.h>
#समावेश <sbi/sbi_bitops.h>
#समावेश <sbi/sbi_error.h>
#समावेश <sbi/sbi_system.h>
#समावेश <sbi_utils/fdt/fdt_driver.h>
#समावेश <sbi_utils/fdt/fdt_helper.h>

#घोषणा WDT_कुंजी_VAL			0x16aa0000

#घोषणा WDT_SOFT_RST_REG		0x08
#घोषणा WDT_SOFT_RST_EN			BIT(0)

#घोषणा WDT_MODE_REG			0x18

अटल अस्थिर अक्षर *sunxi_wdt_base;

अटल पूर्णांक sunxi_wdt_system_reset_check(u32 type, u32 reason)
{
	चयन (type) {
	हाल SBI_SRST_RESET_TYPE_COLD_REBOOT:
	हाल SBI_SRST_RESET_TYPE_WARM_REBOOT:
		वापस 1;
	}

	वापस 0;
}

अटल व्योम sunxi_wdt_system_reset(u32 type, u32 reason)
{
	/* Disable the watchdog. */
	writel_relaxed(WDT_कुंजी_VAL,
		       sunxi_wdt_base + WDT_MODE_REG);

	/* Trigger soft reset. */
	writel_relaxed(WDT_कुंजी_VAL | WDT_SOFT_RST_EN,
		       sunxi_wdt_base + WDT_SOFT_RST_REG);
}

अटल काष्ठा sbi_system_reset_device sunxi_wdt_reset = {
	.name = "sunxi-wdt-reset",
	.system_reset_check = sunxi_wdt_system_reset_check,
	.system_reset = sunxi_wdt_system_reset,
};

अटल पूर्णांक sunxi_wdt_reset_init(स्थिर व्योम *fdt, पूर्णांक nodeoff,
				स्थिर काष्ठा fdt_match *match)
{
	uint64_t reg_addr;
	पूर्णांक rc;

	rc = fdt_get_गाँठ_addr_आकार(fdt, nodeoff, 0, &reg_addr, शून्य);
	अगर (rc < 0 || !reg_addr)
		वापस SBI_ENODEV;

	sunxi_wdt_base = (अस्थिर अक्षर *)(अचिन्हित दीर्घ)reg_addr;

	sbi_system_reset_add_device(&sunxi_wdt_reset);

	वापस 0;
}

अटल स्थिर काष्ठा fdt_match sunxi_wdt_reset_match[] = {
	{ .compatible = "allwinner,sun20i-d1-wdt-reset" },
	{ },
};

स्थिर काष्ठा fdt_driver fdt_reset_sunxi_wdt = {
	.match_table = sunxi_wdt_reset_match,
	.init = sunxi_wdt_reset_init,
};
