/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 StarFive Technology Co., Ltd.
 *
 * Author: Jun Liang Tan <junliang.tan@linux.starfivetech.com>
 */

#অন্তর্ভাব <sbi_utils/fdt/fdt_helper.h>
#অন্তর্ভাব <sbi_utils/serial/fdt_serial.h>
#অন্তর্ভাব <sbi_utils/serial/cadence-uart.h>

স্থির পূর্ণাঙ্ক serial_cadence_init(ধ্রুব শূন্য *fdt, পূর্ণাঙ্ক nodeoff,
			       ধ্রুব সংরচনা fdt_match *match)
{
	পূর্ণাঙ্ক rc;
	সংরচনা platform_uart_data uart = { 0 };

	rc = fdt_parse_uart_গ্রন্থি(fdt, nodeoff, &uart);
	যদি (rc)
		প্রত্যাগম rc;

	প্রত্যাগম cadence_uart_init(uart.addr, uart.freq, uart.baud);
}

স্থির ধ্রুব সংরচনা fdt_match serial_cadence_match[] = {
	{ .compatible = "cdns,uart-r1p8", },
	{ .compatible = "cdns,uart-r1p12" },
	{ .compatible = "starfive,jh8100-uart" },
	{ },
};

ধ্রুব সংরচনা fdt_driver fdt_serial_cadence = {
	.match_table = serial_cadence_match,
	.init = serial_cadence_init
};
