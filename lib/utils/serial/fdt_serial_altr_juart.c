/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 ISCAS
 *
 * Authors:
 *   Icenowy Zheng <zhengxingda@iscas.ac.cn>
 */

#అన్తర్భావ <sbi/sbi_error.h>
#అన్తర్భావ <sbi_utils/fdt/fdt_helper.h>
#అన్తర్భావ <sbi_utils/serial/fdt_serial.h>
#అన్తర్భావ <sbi_utils/serial/altr-juart.h>

స్థిర పూర్ణాంకం serial_altr_juart_init(స్థిరరాశి శూన్య *fdt, పూర్ణాంకం nodeoff,
				  స్థిరరాశి నిర్మాణం fdt_సరిపోలిక *సరిపోలిక)
{
	uint64_t reg_addr, reg_కొలత;
	పూర్ణాంకం rc;

	ఒకవేళ (nodeoff < 0 || !fdt)
		తిరిగి SBI_ENODEV;

	rc = fdt_get_కణుపు_addr_కొలత(fdt, nodeoff, 0, &reg_addr, &reg_కొలత);
	/* Two 32-bit registers */
	ఒకవేళ (rc < 0 || !reg_addr || !reg_కొలత || reg_కొలత < 0x8)
		తిరిగి SBI_ENODEV;

	తిరిగి altr_juart_init(reg_addr);
}

స్థిర స్థిరరాశి నిర్మాణం fdt_సరిపోలిక serial_altr_juart_సరిపోలిక[] = {
	{ .compatible = "altr,juart-1.0" },
	{ },
};

స్థిరరాశి నిర్మాణం fdt_driver fdt_serial_altr_juart = {
	.సరిపోలిక_పట్టిక = serial_altr_juart_సరిపోలిక,
	.init = serial_altr_juart_init,
};
