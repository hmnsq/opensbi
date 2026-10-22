/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Alistair Francis <alistair.francis@wdc.com>
 */

#അന്തര്ഭാവ <sbi_utils/fdt/fdt_helper.h>
#അന്തര്ഭാവ <sbi_utils/serial/fdt_serial.h>
#അന്തര്ഭാവ <sbi_utils/serial/xlnx_uartlite.h>

സ്ഥിതം പൂർണ്ണസംഖ്യ serial_xlnx_uartlite_init(സ്ഥിരം ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ nodeoff,
				     സ്ഥിരം ഘടന fdt_match *match)
{
	പൂർണ്ണസംഖ്യ rc;
	ഘടന platform_uart_data uart = { 0 };

	rc = fdt_parse_xlnx_uartlite_കണ്ണി(fdt, nodeoff, &uart);
	എങ്കിൽ (rc)
		തിരികെ rc;

	തിരികെ xlnx_uartlite_init(uart.addr);
}

സ്ഥിതം സ്ഥിരം ഘടന fdt_match serial_xlnx_uartlite_match[] = {
	{ .compatible = "xlnx,xps-uartlite-1.00.a" },
	{ },
};

സ്ഥിരം ഘടന fdt_driver fdt_serial_xlnx_uartlite = {
	.match_table = serial_xlnx_uartlite_match,
	.init = serial_xlnx_uartlite_init,
};
