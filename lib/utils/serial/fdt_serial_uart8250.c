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
#അന്തര്ഭാവ <sbi_utils/serial/uart8250.h>

സ്ഥിതം പൂർണ്ണസംഖ്യ serial_uart8250_init(സ്ഥിരം ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ nodeoff,
				സ്ഥിരം ഘടന fdt_match *match)
{
	ഘടന platform_uart_data uart = { 0 };
	ulong caps = (ulong)match->data;
	പൂർണ്ണസംഖ്യ rc;

	rc = fdt_parse_uart_കണ്ണി(fdt, nodeoff, &uart);
	എങ്കിൽ (rc)
		തിരികെ rc;

	തിരികെ uart8250_init(uart.addr, uart.freq, uart.baud,
			     uart.reg_shift, uart.reg_io_width,
			     uart.reg_offset, caps);
}

സ്ഥിതം സ്ഥിരം ഘടന fdt_match serial_uart8250_match[] = {
	{ .compatible = "ns16550" },
	{ .compatible = "ns16550a" },
	{ .compatible = "snps,dw-apb-uart" },
	{ .compatible = "intel,xscale-uart",
	  .data = (ശൂന്യം *)UART_CAP_UUE },
	{ },
};

സ്ഥിരം ഘടന fdt_driver fdt_serial_uart8250 = {
	.match_table = serial_uart8250_match,
	.init = serial_uart8250_init,
};
