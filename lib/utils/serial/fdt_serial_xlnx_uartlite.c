/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Alistair Francis <alistair.francis@wdc.com>
 */

#ಅನ್ತರ್ಭಾವ <sbi_utils/fdt/fdt_helper.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/serial/fdt_serial.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/serial/xlnx_uartlite.h>

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ serial_xlnx_uartlite_init(ಧ್ರುವ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ nodeoff,
				     ಧ್ರುವ ಸಂರಚನಾ fdt_match *match)
{
	ಪೂರ್ಣಾಙ್ಕ rc;
	ಸಂರಚನಾ platform_uart_data uart = { 0 };

	rc = fdt_parse_xlnx_uartlite_ಗಂಟು(fdt, nodeoff, &uart);
	ಯದಿ (rc)
		ಪ್ರತ್ಯಾಗಮ rc;

	ಪ್ರತ್ಯಾಗಮ xlnx_uartlite_init(uart.addr);
}

ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ fdt_match serial_xlnx_uartlite_match[] = {
	{ .compatible = "xlnx,xps-uartlite-1.00.a" },
	{ },
};

ಧ್ರುವ ಸಂರಚನಾ fdt_driver fdt_serial_xlnx_uartlite = {
	.match_table = serial_xlnx_uartlite_match,
	.init = serial_xlnx_uartlite_init,
};
