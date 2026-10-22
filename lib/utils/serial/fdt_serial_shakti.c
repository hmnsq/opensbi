/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Vijai Kumar K <vijai@behindbytes.com>
 *
 */

#অন্তর্ভাৱ <sbi_utils/fdt/fdt_helper.h>
#অন্তর্ভাৱ <sbi_utils/serial/fdt_serial.h>
#অন্তর্ভাৱ <sbi_utils/serial/shakti-uart.h>

স্থির পূর্ণাঙ্ক serial_shakti_init(ধ্রুৱ শূন্য *fdt, পূর্ণাঙ্ক nodeoff,
			      ধ্রুৱ সংরচনা fdt_match *match)
{
	পূর্ণাঙ্ক rc;
	সংরচনা platform_uart_data uart = { 0 };

	rc = fdt_parse_shakti_uart_গ্ৰন্থি(fdt, nodeoff, &uart);
	যদি (rc)
		ঘূৰাই rc;

	ঘূৰাই shakti_uart_init(uart.addr, uart.freq, uart.baud);
}

স্থির ধ্রুৱ সংরচনা fdt_match serial_shakti_match[] = {
	{ .compatible = "shakti,uart0" },
	{ },
};

ধ্রুৱ সংরচনা fdt_driver fdt_serial_shakti = {
	.match_table = serial_shakti_match,
	.init = serial_shakti_init
};
