/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Samuel Holland <samuel@sholland.org>
 */

#అన్తర్భావ <libfdt.h>
#అన్తర్భావ <sbi/riscv_io.h>
#అన్తర్భావ <sbi/sbi_bitops.h>
#అన్తర్భావ <sbi/sbi_error.h>
#అన్తర్భావ <sbi/sbi_system.h>
#అన్తర్భావ <sbi_utils/fdt/fdt_driver.h>
#అన్తర్భావ <sbi_utils/fdt/fdt_helper.h>

#నిర్వచించు WDT_కీలకం_VAL			0x16aa0000

#నిర్వచించు WDT_SOFT_RST_REG		0x08
#నిర్వచించు WDT_SOFT_RST_EN			బిట్టు(0)

#నిర్వచించు WDT_బహుళకం_REG			0x18

స్థిర అస్థిర అక్షరం *sunxi_wdt_పునాది;

స్థిర పూర్ణాంకం sunxi_wdt_system_reset_check(u32 type, u32 reason)
{
	ఎంపిక (type) {
	స్థితి SBI_SRST_RESET_TYPE_COLD_REBOOT:
	స్థితి SBI_SRST_RESET_TYPE_WARM_REBOOT:
		తిరిగి 1;
	}

	తిరిగి 0;
}

స్థిర శూన్య sunxi_wdt_system_reset(u32 type, u32 reason)
{
	/* Disable the watchdog. */
	writel_relaxed(WDT_కీలకం_VAL,
		       sunxi_wdt_పునాది + WDT_బహుళకం_REG);

	/* Trigger soft reset. */
	writel_relaxed(WDT_కీలకం_VAL | WDT_SOFT_RST_EN,
		       sunxi_wdt_పునాది + WDT_SOFT_RST_REG);
}

స్థిర నిర్మాణం sbi_system_reset_device sunxi_wdt_reset = {
	.name = "sunxi-wdt-reset",
	.system_reset_check = sunxi_wdt_system_reset_check,
	.system_reset = sunxi_wdt_system_reset,
};

స్థిర పూర్ణాంకం sunxi_wdt_reset_init(స్థిరరాశి శూన్య *fdt, పూర్ణాంకం nodeoff,
				స్థిరరాశి నిర్మాణం fdt_సరిపోలిక *సరిపోలిక)
{
	uint64_t reg_addr;
	పూర్ణాంకం rc;

	rc = fdt_get_కణుపు_addr_కొలత(fdt, nodeoff, 0, &reg_addr, ఖాళీ);
	ఒకవేళ (rc < 0 || !reg_addr)
		తిరిగి SBI_ENODEV;

	sunxi_wdt_పునాది = (అస్థిర అక్షరం *)(గుర్తులేని దీర్ఘ)reg_addr;

	sbi_system_reset_కూడు_device(&sunxi_wdt_reset);

	తిరిగి 0;
}

స్థిర స్థిరరాశి నిర్మాణం fdt_సరిపోలిక sunxi_wdt_reset_సరిపోలిక[] = {
	{ .compatible = "allwinner,sun20i-d1-wdt-reset" },
	{ },
};

స్థిరరాశి నిర్మాణం fdt_driver fdt_reset_sunxi_wdt = {
	.సరిపోలిక_పట్టిక = sunxi_wdt_reset_సరిపోలిక,
	.init = sunxi_wdt_reset_init,
};
