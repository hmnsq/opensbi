/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Alistair Francis <alistair.francis@wdc.com>
 */

#అన్తర్భావ <sbi_utils/fdt/fdt_helper.h>
#అన్తర్భావ <sbi_utils/serial/fdt_serial.h>
#అన్తర్భావ <sbi_utils/serial/xlnx_uartlite.h>

స్థిర పూర్ణాంకం serial_xlnx_uartlite_init(స్థిరరాశి శూన్య *fdt, పూర్ణాంకం nodeoff,
				     స్థిరరాశి నిర్మాణం fdt_సరిపోలిక *సరిపోలిక)
{
	పూర్ణాంకం rc;
	నిర్మాణం platform_uart_దత్తాంశం uart = { 0 };

	rc = fdt_parse_xlnx_uartlite_కణుపు(fdt, nodeoff, &uart);
	ఒకవేళ (rc)
		తిరిగి rc;

	తిరిగి xlnx_uartlite_init(uart.addr);
}

స్థిర స్థిరరాశి నిర్మాణం fdt_సరిపోలిక serial_xlnx_uartlite_సరిపోలిక[] = {
	{ .compatible = "xlnx,xps-uartlite-1.00.a" },
	{ },
};

స్థిరరాశి నిర్మాణం fdt_driver fdt_serial_xlnx_uartlite = {
	.సరిపోలిక_పట్టిక = serial_xlnx_uartlite_సరిపోలిక,
	.init = serial_xlnx_uartlite_init,
};
