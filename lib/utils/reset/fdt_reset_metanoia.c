/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2026 Metanoia Communications Inc.
 *
 */

#అన్తర్భావ <sbi/riscv_io.h>
#అన్తర్భావ <sbi/sbi_platform.h>
#అన్తర్భావ <sbi/sbi_system.h>
#అన్తర్భావ <sbi_utils/fdt/fdt_helper.h>
#అన్తర్భావ <sbi_utils/fdt/fdt_driver.h>

#నిర్వచించు SWRSTREQ_CTRL_REG_స్థానభ్రంశం 0x00
#నిర్వచించు SWRSTREQ_REG_స్థానభ్రంశం 0x04

నిర్మాణం reset_metanoia_దత్తాంశం {
	u8 *reg_పునాది;
};

స్థిర నిర్మాణం reset_metanoia_దత్తాంశం reset_దత్తాంశం;

స్థిర పూర్ణాంకం metanoia_system_reset_check(u32 type, u32 reason)
{
	ఎంపిక (type) {
	స్థితి SBI_SRST_RESET_TYPE_WARM_REBOOT:
	స్థితి SBI_SRST_RESET_TYPE_COLD_REBOOT:
		తిరిగి 1;
	స్థితి SBI_SRST_RESET_TYPE_SHUTDOWN:
	అప్రమేయ:
		తిరిగి 0;
	}
}

స్థిర శూన్య metanoia_system_reset(u32 type, u32 reason)
{
	writew(0x1, reset_దత్తాంశం.reg_పునాది + SWRSTREQ_CTRL_REG_స్థానభ్రంశం);
	writew(0x1, reset_దత్తాంశం.reg_పునాది + SWRSTREQ_REG_స్థానభ్రంశం);
}

స్థిర నిర్మాణం sbi_system_reset_device metanoia_reset = {
	.name		    = "metanoia-mt2824-reboot",
	.system_reset_check = metanoia_system_reset_check,
	.system_reset	    = metanoia_system_reset,
};

స్థిర పూర్ణాంకం metanoia_reset_init(స్థిరరాశి శూన్య *fdt, పూర్ణాంకం nodeoff,
			       స్థిరరాశి నిర్మాణం fdt_సరిపోలిక *సరిపోలిక)
{
	u64 reg_addr;
	పూర్ణాంకం rc;

	rc = fdt_get_కణుపు_addr_కొలత(fdt, nodeoff, 0, &reg_addr, ఖాళీ);
	ఒకవేళ (rc < 0 || !reg_addr)
		తిరిగి SBI_ENODEV;

	reset_దత్తాంశం.reg_పునాది = (u8 *)(ulong)reg_addr;

	sbi_system_reset_కూడు_device(&metanoia_reset);

	తిరిగి 0;
}

స్థిర స్థిరరాశి నిర్మాణం fdt_సరిపోలిక metanoia_reset_సరిపోలిక[] = {
	{ .compatible = "metanoia,mt2824-reboot" },
	{ /* sentinel */ }
};

స్థిరరాశి నిర్మాణం fdt_driver fdt_reset_metanoia = {
	.సరిపోలిక_పట్టిక = metanoia_reset_సరిపోలిక,
	.init	     = metanoia_reset_init,
};
