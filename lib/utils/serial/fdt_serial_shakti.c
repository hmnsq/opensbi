/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Vijai Kumar K <vijai@behindbytes.com>
 *
 */

#અન્તર્ભાવ <sbi_utils/fdt/fdt_helper.h>
#અન્તર્ભાવ <sbi_utils/serial/fdt_serial.h>
#અન્તર્ભાવ <sbi_utils/serial/shakti-uart.h>

સ્થિર પૂર્ણાંક serial_shakti_init(અચલ શૂન્ય *fdt, પૂર્ણાંક nodeoff,
			      અચલ સંરચના fdt_match *match)
{
	પૂર્ણાંક rc;
	સંરચના platform_uart_data uart = { 0 };

	rc = fdt_parse_shakti_uart_ગાંઠ(fdt, nodeoff, &uart);
	જો (rc)
		પાછા rc;

	પાછા shakti_uart_init(uart.addr, uart.freq, uart.baud);
}

સ્થિર અચલ સંરચના fdt_match serial_shakti_match[] = {
	{ .compatible = "shakti,uart0" },
	{ },
};

અચલ સંરચના fdt_driver fdt_serial_shakti = {
	.match_table = serial_shakti_match,
	.init = serial_shakti_init
};
