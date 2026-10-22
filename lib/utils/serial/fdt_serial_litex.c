/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Gabriel Somlo
 *
 * Authors:
 *   Gabriel Somlo <gsomlo@gmail.com>
 */

#అన్తర్భావ <sbi/sbi_error.h>
#అన్తర్భావ <sbi_utils/fdt/fdt_helper.h>
#అన్తర్భావ <sbi_utils/serial/fdt_serial.h>
#అన్తర్భావ <sbi_utils/serial/litex-uart.h>

స్థిర పూర్ణాంకం serial_litex_init(స్థిరరాశి శూన్య *fdt, పూర్ణాంకం nodeoff,
			     స్థిరరాశి నిర్మాణం fdt_సరిపోలిక *సరిపోలిక)
{
	uint64_t reg_addr, reg_కొలత;
	పూర్ణాంకం rc;

	ఒకవేళ (nodeoff < 0 || !fdt)
		తిరిగి SBI_ENODEV;

	rc = fdt_get_కణుపు_addr_కొలత(fdt, nodeoff, 0, &reg_addr, &reg_కొలత);
	ఒకవేళ (rc < 0 || !reg_addr || !reg_కొలత)
		తిరిగి SBI_ENODEV;

	తిరిగి litex_uart_init(reg_addr);
}

స్థిర స్థిరరాశి నిర్మాణం fdt_సరిపోలిక serial_litex_సరిపోలిక[] = {
	{ .compatible = "litex,liteuart" },
	{ },
};

స్థిరరాశి నిర్మాణం fdt_driver fdt_serial_litex = {
	.సరిపోలిక_పట్టిక = serial_litex_సరిపోలిక,
	.init = serial_litex_init
};
