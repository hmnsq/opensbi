/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Vijai Kumar K <vijai@behindbytes.com>
 *
 */

#అన్తర్భావ <sbi_utils/fdt/fdt_helper.h>
#అన్తర్భావ <sbi_utils/serial/fdt_serial.h>
#అన్తర్భావ <sbi_utils/serial/shakti-uart.h>

స్థిర పూర్ణాంకం serial_shakti_init(స్థిరరాశి శూన్య *fdt, పూర్ణాంకం nodeoff,
			      స్థిరరాశి నిర్మాణం fdt_సరిపోలిక *సరిపోలిక)
{
	పూర్ణాంకం rc;
	నిర్మాణం platform_uart_దత్తాంశం uart = { 0 };

	rc = fdt_parse_shakti_uart_కణుపు(fdt, nodeoff, &uart);
	ఒకవేళ (rc)
		తిరిగి rc;

	తిరిగి shakti_uart_init(uart.addr, uart.freq, uart.baud);
}

స్థిర స్థిరరాశి నిర్మాణం fdt_సరిపోలిక serial_shakti_సరిపోలిక[] = {
	{ .compatible = "shakti,uart0" },
	{ },
};

స్థిరరాశి నిర్మాణం fdt_driver fdt_serial_shakti = {
	.సరిపోలిక_పట్టిక = serial_shakti_సరిపోలిక,
	.init = serial_shakti_init
};
