/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Cobham Gaisler AB.
 *
 * Authors:
 *   Daniel Cederman <cederman@gaisler.com>
 */

#ಅನ್ತರ್ಭಾವ <sbi_utils/fdt/fdt_helper.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/serial/fdt_serial.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/serial/gaisler-uart.h>

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ serial_gaisler_init(ಧ್ರುವ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ nodeoff,
			       ಧ್ರುವ ಸಂರಚನಾ fdt_match *match)
{
	ಪೂರ್ಣಾಙ್ಕ rc;
	ಸಂರಚನಾ platform_uart_data uart = { 0 };

	rc = fdt_parse_gaisler_uart_ಗಂಟು(fdt, nodeoff, &uart);
	ಯದಿ (rc)
		ಪ್ರತ್ಯಾಗಮ rc;

	ಪ್ರತ್ಯಾಗಮ gaisler_uart_init(uart.addr, uart.freq, uart.baud);
}

ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ fdt_match serial_gaisler_match[] = {
	{ .compatible = "gaisler,apbuart" },
	{},
};

ಧ್ರುವ ಸಂರಚನಾ fdt_driver fdt_serial_gaisler = {
	.match_table = serial_gaisler_match,
	.init = serial_gaisler_init
};
