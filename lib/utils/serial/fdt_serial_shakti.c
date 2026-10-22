/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Vijai Kumar K <vijai@behindbytes.com>
 *
 */

#അന്തര്ഭാവ <sbi_utils/fdt/fdt_helper.h>
#അന്തര്ഭാവ <sbi_utils/serial/fdt_serial.h>
#അന്തര്ഭാവ <sbi_utils/serial/shakti-uart.h>

സ്ഥിതം പൂർണ്ണസംഖ്യ serial_shakti_init(സ്ഥിരം ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ nodeoff,
			      സ്ഥിരം ഘടന fdt_match *match)
{
	പൂർണ്ണസംഖ്യ rc;
	ഘടന platform_uart_data uart = { 0 };

	rc = fdt_parse_shakti_uart_കണ്ണി(fdt, nodeoff, &uart);
	എങ്കിൽ (rc)
		തിരികെ rc;

	തിരികെ shakti_uart_init(uart.addr, uart.freq, uart.baud);
}

സ്ഥിതം സ്ഥിരം ഘടന fdt_match serial_shakti_match[] = {
	{ .compatible = "shakti,uart0" },
	{ },
};

സ്ഥിരം ഘടന fdt_driver fdt_serial_shakti = {
	.match_table = serial_shakti_match,
	.init = serial_shakti_init
};
