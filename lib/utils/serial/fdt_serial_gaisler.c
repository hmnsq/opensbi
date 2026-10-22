/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Cobham Gaisler AB.
 *
 * Authors:
 *   Daniel Cederman <cederman@gaisler.com>
 */

#അന്തര്ഭാവ <sbi_utils/fdt/fdt_helper.h>
#അന്തര്ഭാവ <sbi_utils/serial/fdt_serial.h>
#അന്തര്ഭാവ <sbi_utils/serial/gaisler-uart.h>

സ്ഥിതം പൂർണ്ണസംഖ്യ serial_gaisler_init(സ്ഥിരം ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ nodeoff,
			       സ്ഥിരം ഘടന fdt_match *match)
{
	പൂർണ്ണസംഖ്യ rc;
	ഘടന platform_uart_data uart = { 0 };

	rc = fdt_parse_gaisler_uart_കണ്ണി(fdt, nodeoff, &uart);
	എങ്കിൽ (rc)
		തിരികെ rc;

	തിരികെ gaisler_uart_init(uart.addr, uart.freq, uart.baud);
}

സ്ഥിതം സ്ഥിരം ഘടന fdt_match serial_gaisler_match[] = {
	{ .compatible = "gaisler,apbuart" },
	{},
};

സ്ഥിരം ഘടന fdt_driver fdt_serial_gaisler = {
	.match_table = serial_gaisler_match,
	.init = serial_gaisler_init
};
