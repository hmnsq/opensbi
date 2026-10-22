/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#అన్తర్భావ <sbi_utils/fdt/fdt_helper.h>
#అన్తర్భావ <sbi_utils/serial/fdt_serial.h>
#అన్తర్భావ <sbi_utils/serial/uart8250.h>

స్థిర పూర్ణాంకం serial_uart8250_init(స్థిరరాశి శూన్య *fdt, పూర్ణాంకం nodeoff,
				స్థిరరాశి నిర్మాణం fdt_సరిపోలిక *సరిపోలిక)
{
	నిర్మాణం platform_uart_దత్తాంశం uart = { 0 };
	ulong caps = (ulong)సరిపోలిక->దత్తాంశం;
	పూర్ణాంకం rc;

	rc = fdt_parse_uart_కణుపు(fdt, nodeoff, &uart);
	ఒకవేళ (rc)
		తిరిగి rc;

	తిరిగి uart8250_init(uart.addr, uart.freq, uart.baud,
			     uart.reg_జరుపు, uart.reg_io_వెడల్పు,
			     uart.reg_స్థానభ్రంశం, caps);
}

స్థిర స్థిరరాశి నిర్మాణం fdt_సరిపోలిక serial_uart8250_సరిపోలిక[] = {
	{ .compatible = "ns16550" },
	{ .compatible = "ns16550a" },
	{ .compatible = "snps,dw-apb-uart" },
	{ .compatible = "intel,xscale-uart",
	  .దత్తాంశం = (శూన్య *)UART_CAP_UUE },
	{ },
};

స్థిరరాశి నిర్మాణం fdt_driver fdt_serial_uart8250 = {
	.సరిపోలిక_పట్టిక = serial_uart8250_సరిపోలిక,
	.init = serial_uart8250_init,
};
