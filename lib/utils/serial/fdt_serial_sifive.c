/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#അന്തര്ഭാവ <sbi_utils/fdt/fdt_helper.h>
#അന്തര്ഭാവ <sbi_utils/serial/fdt_serial.h>
#അന്തര്ഭാവ <sbi_utils/serial/sifive-uart.h>

സ്ഥിതം പൂർണ്ണസംഖ്യ serial_sifive_init(സ്ഥിരം ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ nodeoff,
			      സ്ഥിരം ഘടന fdt_match *match)
{
	പൂർണ്ണസംഖ്യ rc;
	ഘടന platform_uart_data uart = { 0 };

	rc = fdt_parse_sifive_uart_കണ്ണി(fdt, nodeoff, &uart);
	എങ്കിൽ (rc)
		തിരികെ rc;

	തിരികെ sifive_uart_init(uart.addr, uart.freq, uart.baud);
}

സ്ഥിതം സ്ഥിരം ഘടന fdt_match serial_sifive_match[] = {
	{ .compatible = "sifive,fu540-c000-uart" },
	{ .compatible = "sifive,uart0" },
	{ },
};

സ്ഥിരം ഘടന fdt_driver fdt_serial_sifive = {
	.match_table = serial_sifive_match,
	.init = serial_sifive_init
};
