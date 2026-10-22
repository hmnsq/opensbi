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
#అన్తర్భావ <sbi_utils/serial/sifive-uart.h>

స్థిర పూర్ణాంకం serial_sifive_init(స్థిరరాశి శూన్య *fdt, పూర్ణాంకం nodeoff,
			      స్థిరరాశి నిర్మాణం fdt_సరిపోలిక *సరిపోలిక)
{
	పూర్ణాంకం rc;
	నిర్మాణం platform_uart_దత్తాంశం uart = { 0 };

	rc = fdt_parse_sifive_uart_కణుపు(fdt, nodeoff, &uart);
	ఒకవేళ (rc)
		తిరిగి rc;

	తిరిగి sifive_uart_init(uart.addr, uart.freq, uart.baud);
}

స్థిర స్థిరరాశి నిర్మాణం fdt_సరిపోలిక serial_sifive_సరిపోలిక[] = {
	{ .compatible = "sifive,fu540-c000-uart" },
	{ .compatible = "sifive,uart0" },
	{ },
};

స్థిరరాశి నిర్మాణం fdt_driver fdt_serial_sifive = {
	.సరిపోలిక_పట్టిక = serial_sifive_సరిపోలిక,
	.init = serial_sifive_init
};
