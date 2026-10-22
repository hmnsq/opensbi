/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Cobham Gaisler AB.
 *
 * Authors:
 *   Daniel Cederman <cederman@gaisler.com>
 */

#અન્તર્ભાવ <sbi_utils/fdt/fdt_helper.h>
#અન્તર્ભાવ <sbi_utils/serial/fdt_serial.h>
#અન્તર્ભાવ <sbi_utils/serial/gaisler-uart.h>

સ્થિર પૂર્ણાંક serial_gaisler_init(અચલ શૂન્ય *fdt, પૂર્ણાંક nodeoff,
			       અચલ સંરચના fdt_match *match)
{
	પૂર્ણાંક rc;
	સંરચના platform_uart_data uart = { 0 };

	rc = fdt_parse_gaisler_uart_ગાંઠ(fdt, nodeoff, &uart);
	જો (rc)
		પાછા rc;

	પાછા gaisler_uart_init(uart.addr, uart.freq, uart.baud);
}

સ્થિર અચલ સંરચના fdt_match serial_gaisler_match[] = {
	{ .compatible = "gaisler,apbuart" },
	{},
};

અચલ સંરચના fdt_driver fdt_serial_gaisler = {
	.match_table = serial_gaisler_match,
	.init = serial_gaisler_init
};
