/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#અન્તર્ભાવ <sbi_utils/fdt/fdt_helper.h>
#અન્તર્ભાવ <sbi_utils/serial/fdt_serial.h>
#અન્તર્ભાવ <sbi_utils/serial/sifive-uart.h>

સ્થિર પૂર્ણાંક serial_sifive_init(અચલ શૂન્ય *fdt, પૂર્ણાંક nodeoff,
			      અચલ સંરચના fdt_match *match)
{
	પૂર્ણાંક rc;
	સંરચના platform_uart_data uart = { 0 };

	rc = fdt_parse_sifive_uart_ગાંઠ(fdt, nodeoff, &uart);
	જો (rc)
		પાછા rc;

	પાછા sifive_uart_init(uart.addr, uart.freq, uart.baud);
}

સ્થિર અચલ સંરચના fdt_match serial_sifive_match[] = {
	{ .compatible = "sifive,fu540-c000-uart" },
	{ .compatible = "sifive,uart0" },
	{ },
};

અચલ સંરચના fdt_driver fdt_serial_sifive = {
	.match_table = serial_sifive_match,
	.init = serial_sifive_init
};
