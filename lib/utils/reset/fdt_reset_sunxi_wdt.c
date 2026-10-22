/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Samuel Holland <samuel@sholland.org>
 */

#ضمن <libfdt.h>
#ضمن <sbi/riscv_io.h>
#ضمن <sbi/sbi_bitops.h>
#ضمن <sbi/sbi_error.h>
#ضمن <sbi/sbi_system.h>
#ضمن <sbi_utils/fdt/fdt_driver.h>
#ضمن <sbi_utils/fdt/fdt_helper.h>

#تعریف_کرو WDT_کلید_VAL			0x16aa0000

#تعریف_کرو WDT_SOFT_RST_REG		0x08
#تعریف_کرو WDT_SOFT_RST_EN			ثنائی_ہندسہ(0)

#تعریف_کرو WDT_منوال_REG			0x18

ساكن متطاير حرف *sunxi_wdt_ادھار;

ساكن صحيح sunxi_wdt_system_reset_check(u32 type, u32 reason)
{
	بدل (type) {
	حالة SBI_SRST_RESET_TYPE_COLD_REBOOT:
	حالة SBI_SRST_RESET_TYPE_WARM_REBOOT:
		أرجع 1;
	}

	أرجع 0;
}

ساكن فراغ sunxi_wdt_system_reset(u32 type, u32 reason)
{
	/* Disable the watchdog. */
	writel_relaxed(WDT_کلید_VAL,
		       sunxi_wdt_ادھار + WDT_منوال_REG);

	/* Trigger soft reset. */
	writel_relaxed(WDT_کلید_VAL | WDT_SOFT_RST_EN,
		       sunxi_wdt_ادھار + WDT_SOFT_RST_REG);
}

ساكن بنية sbi_system_reset_device sunxi_wdt_reset = {
	.name = "sunxi-wdt-reset",
	.system_reset_check = sunxi_wdt_system_reset_check,
	.system_reset = sunxi_wdt_system_reset,
};

ساكن صحيح sunxi_wdt_reset_init(ثابت فراغ *fdt, صحيح nodeoff,
				ثابت بنية fdt_میل *میل)
{
	uint64_t reg_addr;
	صحيح rc;

	rc = fdt_get_گرہ_addr_ناپ(fdt, nodeoff, 0, &reg_addr, عدم);
	إذا (rc < 0 || !reg_addr)
		أرجع SBI_ENODEV;

	sunxi_wdt_ادھار = (متطاير حرف *)(بنانشانی لما)reg_addr;

	sbi_system_reset_جوڑو_device(&sunxi_wdt_reset);

	أرجع 0;
}

ساكن ثابت بنية fdt_میل sunxi_wdt_reset_میل[] = {
	{ .compatible = "allwinner,sun20i-d1-wdt-reset" },
	{ },
};

ثابت بنية fdt_driver fdt_reset_sunxi_wdt = {
	.میل_جدول = sunxi_wdt_reset_میل,
	.init = sunxi_wdt_reset_init,
};
