/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#অন্তর্ভাব <sbi_utils/fdt/fdt_helper.h>
#অন্তর্ভাব <sbi_utils/serial/fdt_serial.h>
#অন্তর্ভাব <sbi_utils/serial/uart8250.h>

স্থির পূর্ণাঙ্ক serial_uart8250_init(ধ্রুব শূন্য *fdt, পূর্ণাঙ্ক nodeoff,
				ধ্রুব সংরচনা fdt_match *match)
{
	সংরচনা platform_uart_data uart = { 0 };
	ulong caps = (ulong)match->data;
	পূর্ণাঙ্ক rc;

	rc = fdt_parse_uart_গ্রন্থি(fdt, nodeoff, &uart);
	যদি (rc)
		প্রত্যাগম rc;

	প্রত্যাগম uart8250_init(uart.addr, uart.freq, uart.baud,
			     uart.reg_shift, uart.reg_io_width,
			     uart.reg_offset, caps);
}

স্থির ধ্রুব সংরচনা fdt_match serial_uart8250_match[] = {
	{ .compatible = "ns16550" },
	{ .compatible = "ns16550a" },
	{ .compatible = "snps,dw-apb-uart" },
	{ .compatible = "intel,xscale-uart",
	  .data = (শূন্য *)UART_CAP_UUE },
	{ },
};

ধ্রুব সংরচনা fdt_driver fdt_serial_uart8250 = {
	.match_table = serial_uart8250_match,
	.init = serial_uart8250_init,
};
