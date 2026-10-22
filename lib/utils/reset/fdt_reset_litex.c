/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2026 Inochi Amaoto <inochiama@gmail.com>
 */

#అన్తర్భావ <libfdt.h>
#అన్తర్భావ <sbi/riscv_io.h>
#అన్తర్భావ <sbi/sbi_error.h>
#అన్తర్భావ <sbi/sbi_system.h>
#అన్తర్భావ <sbi_utils/fdt/fdt_driver.h>
#అన్తర్భావ <sbi_utils/fdt/fdt_helper.h>

#నిర్వచించు RESET_CTRL			0x0

స్థిర అస్థిర u32 *litex_soc_పునాది;

స్థిర పూర్ణాంకం litex_reset_check(u32 type, u32 reason)
{
	ఎంపిక (type) {
	స్థితి SBI_SRST_RESET_TYPE_COLD_REBOOT:
	స్థితి SBI_SRST_RESET_TYPE_WARM_REBOOT:
		తిరిగి 255;
	}

	తిరిగి 0;
}

స్థిర శూన్య litex_do_reset(u32 type, u32 reason)
{
	writel_relaxed(0x1, litex_soc_పునాది + RESET_CTRL);
}

స్థిర నిర్మాణం sbi_system_reset_device litex_reset = {
	.name = "litex-reset",
	.system_reset_check = litex_reset_check,
	.system_reset = litex_do_reset
};

స్థిర పూర్ణాంకం litex_reset_init(స్థిరరాశి శూన్య *fdt, పూర్ణాంకం nodeoff,
			    స్థిరరాశి నిర్మాణం fdt_సరిపోలిక *సరిపోలిక)
{
	uint64_t reg_addr;
	పూర్ణాంకం rc;

	rc = fdt_get_కణుపు_addr_కొలత(fdt, nodeoff, 0, &reg_addr, ఖాళీ);
	ఒకవేళ (rc < 0 || !reg_addr)
		తిరిగి SBI_ENODEV;


	litex_soc_పునాది = (అస్థిర u32 *)(గుర్తులేని దీర్ఘ)reg_addr;

	sbi_system_reset_కూడు_device(&litex_reset);

	తిరిగి 0;
}

స్థిర స్థిరరాశి నిర్మాణం fdt_సరిపోలిక litex_reset_సరిపోలిక[] = {
	{ .compatible = "litex,soc-controller" },
	{ },
};

స్థిరరాశి నిర్మాణం fdt_driver fdt_reset_litex = {
	.సరిపోలిక_పట్టిక = litex_reset_సరిపోలిక,
	.init = litex_reset_init,
};
