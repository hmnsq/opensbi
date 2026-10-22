/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Samuel Holland <samuel@sholland.org>
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <libfdt.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/riscv_io.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_bitops.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_error.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_system.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/fdt/fdt_driver.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/fdt/fdt_helper.h>

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WDT_ਕੁੰਜੀ_VAL			0x16aa0000

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WDT_SOFT_RST_REG		0x08
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WDT_SOFT_RST_EN			ਦੋਆਧਾਰੀ_ਅੰਕ(0)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WDT_ਬਹੁਲਕ_REG			0x18

ਸਥਿਰ ਅਸਥਿਰ ਅੱਖਰ *sunxi_wdt_ਅਧਾਰ;

ਸਥਿਰ ਪੂਰਨ sunxi_wdt_system_reset_check(u32 type, u32 reason)
{
	ਚੋਣ (type) {
	ਹਾਲਤ SBI_SRST_RESET_TYPE_COLD_REBOOT:
	ਹਾਲਤ SBI_SRST_RESET_TYPE_WARM_REBOOT:
		ਵਾਪਸ 1;
	}

	ਵਾਪਸ 0;
}

ਸਥਿਰ ਖਾਲੀ sunxi_wdt_system_reset(u32 type, u32 reason)
{
	/* Disable the watchdog. */
	writel_relaxed(WDT_ਕੁੰਜੀ_VAL,
		       sunxi_wdt_ਅਧਾਰ + WDT_ਬਹੁਲਕ_REG);

	/* Trigger soft reset. */
	writel_relaxed(WDT_ਕੁੰਜੀ_VAL | WDT_SOFT_RST_EN,
		       sunxi_wdt_ਅਧਾਰ + WDT_SOFT_RST_REG);
}

ਸਥਿਰ ਬਣਤਰ sbi_system_reset_device sunxi_wdt_reset = {
	.name = "sunxi-wdt-reset",
	.system_reset_check = sunxi_wdt_system_reset_check,
	.system_reset = sunxi_wdt_system_reset,
};

ਸਥਿਰ ਪੂਰਨ sunxi_wdt_reset_init(ਅਚਲ ਖਾਲੀ *fdt, ਪੂਰਨ nodeoff,
				ਅਚਲ ਬਣਤਰ fdt_ਮੇਲ *ਮੇਲ)
{
	uint64_t reg_addr;
	ਪੂਰਨ rc;

	rc = fdt_get_ਗੰਢ_addr_ਮਾਪ(fdt, nodeoff, 0, &reg_addr, ਨਿਰਾ);
	ਜੇ (rc < 0 || !reg_addr)
		ਵਾਪਸ SBI_ENODEV;

	sunxi_wdt_ਅਧਾਰ = (ਅਸਥਿਰ ਅੱਖਰ *)(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)reg_addr;

	sbi_system_reset_ਜੋੜੋ_device(&sunxi_wdt_reset);

	ਵਾਪਸ 0;
}

ਸਥਿਰ ਅਚਲ ਬਣਤਰ fdt_ਮੇਲ sunxi_wdt_reset_ਮੇਲ[] = {
	{ .compatible = "allwinner,sun20i-d1-wdt-reset" },
	{ },
};

ਅਚਲ ਬਣਤਰ fdt_driver fdt_reset_sunxi_wdt = {
	.ਮੇਲ_ਸਾਰਣੀ = sunxi_wdt_reset_ਮੇਲ,
	.init = sunxi_wdt_reset_init,
};
