/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 StarFive Technology Co., Ltd.
 *
 * Author: Jun Liang Tan <junliang.tan@linux.starfivetech.com>
 */

#అన్తర్భావ <sbi_utils/fdt/fdt_helper.h>
#అన్తర్భావ <sbi_utils/serial/fdt_serial.h>
#అన్తర్భావ <sbi_utils/serial/cadence-uart.h>

స్థిర పూర్ణాంకం serial_cadence_init(స్థిరరాశి శూన్య *fdt, పూర్ణాంకం nodeoff,
			       స్థిరరాశి నిర్మాణం fdt_సరిపోలిక *సరిపోలిక)
{
	పూర్ణాంకం rc;
	నిర్మాణం platform_uart_దత్తాంశం uart = { 0 };

	rc = fdt_parse_uart_కణుపు(fdt, nodeoff, &uart);
	ఒకవేళ (rc)
		తిరిగి rc;

	తిరిగి cadence_uart_init(uart.addr, uart.freq, uart.baud);
}

స్థిర స్థిరరాశి నిర్మాణం fdt_సరిపోలిక serial_cadence_సరిపోలిక[] = {
	{ .compatible = "cdns,uart-r1p8", },
	{ .compatible = "cdns,uart-r1p12" },
	{ .compatible = "starfive,jh8100-uart" },
	{ },
};

స్థిరరాశి నిర్మాణం fdt_driver fdt_serial_cadence = {
	.సరిపోలిక_పట్టిక = serial_cadence_సరిపోలిక,
	.init = serial_cadence_init
};
