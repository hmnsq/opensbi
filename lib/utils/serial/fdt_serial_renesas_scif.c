// SPDX-License-Identifier: BSD-2-Clause
/*
 * Copyright (C) 2022 Renesas Electronics Corporation
 */

#అన్తర్భావ <sbi_utils/fdt/fdt_helper.h>
#అన్తర్భావ <sbi_utils/serial/fdt_serial.h>
#అన్తర్భావ <sbi_utils/serial/renesas-scif.h>

స్థిర పూర్ణాంకం serial_renesas_scif_init(స్థిరరాశి శూన్య *fdt, పూర్ణాంకం nodeoff,
				    స్థిరరాశి నిర్మాణం fdt_సరిపోలిక *సరిపోలిక)
{
	నిర్మాణం platform_uart_దత్తాంశం uart = { 0 };
	పూర్ణాంకం ret;

	ret = fdt_parse_renesas_scif_కణుపు(fdt, nodeoff, &uart);
	ఒకవేళ (ret)
		తిరిగి ret;

	తిరిగి renesas_scif_init(uart.addr, uart.freq, uart.baud);
}

స్థిర స్థిరరాశి నిర్మాణం fdt_సరిపోలిక serial_renesas_scif_సరిపోలిక[] = {
	{ .compatible = "renesas,scif-r9a07g043" },
	{ /* sentinel */ }
};

స్థిరరాశి నిర్మాణం fdt_driver fdt_serial_renesas_scif = {
	.సరిపోలిక_పట్టిక = serial_renesas_scif_సరిపోలిక,
	.init = serial_renesas_scif_init
};
