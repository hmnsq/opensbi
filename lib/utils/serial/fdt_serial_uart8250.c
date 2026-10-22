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
#અન્તર્ભાવ <sbi_utils/serial/uart8250.h>

સ્થિર પૂર્ણાંક serial_uart8250_init(અચલ શૂન્ય *fdt, પૂર્ણાંક nodeoff,
				અચલ સંરચના fdt_match *match)
{
	સંરચના platform_uart_data uart = { 0 };
	ulong caps = (ulong)match->data;
	પૂર્ણાંક rc;

	rc = fdt_parse_uart_ગાંઠ(fdt, nodeoff, &uart);
	જો (rc)
		પાછા rc;

	પાછા uart8250_init(uart.addr, uart.freq, uart.baud,
			     uart.reg_shift, uart.reg_io_width,
			     uart.reg_offset, caps);
}

સ્થિર અચલ સંરચના fdt_match serial_uart8250_match[] = {
	{ .compatible = "ns16550" },
	{ .compatible = "ns16550a" },
	{ .compatible = "snps,dw-apb-uart" },
	{ .compatible = "intel,xscale-uart",
	  .data = (શૂન્ય *)UART_CAP_UUE },
	{ },
};

અચલ સંરચના fdt_driver fdt_serial_uart8250 = {
	.match_table = serial_uart8250_match,
	.init = serial_uart8250_init,
};
