/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Vijai Kumar K <vijai@behindbytes.com>
 *
 */

#অন্তর্ভাব <sbi_utils/fdt/fdt_helper.h>
#অন্তর্ভাব <sbi_utils/serial/fdt_serial.h>
#অন্তর্ভাব <sbi_utils/serial/shakti-uart.h>

স্থির পূর্ণসংখ্যা serial_shakti_init(ধ্রুবক শূন্য *fdt, পূর্ণসংখ্যা nodeoff,
			      ধ্রুবক কাঠামো fdt_match *match)
{
	পূর্ণসংখ্যা rc;
	কাঠামো platform_uart_তথ্য uart = { 0 };

	rc = fdt_parse_shakti_uart_গ্রন্থি(fdt, nodeoff, &uart);
	যদি (rc)
		ফেরত rc;

	ফেরত shakti_uart_init(uart.addr, uart.freq, uart.baud);
}

স্থির ধ্রুবক কাঠামো fdt_match serial_shakti_match[] = {
	{ .compatible = "shakti,uart0" },
	{ },
};

ধ্রুবক কাঠামো fdt_driver fdt_serial_shakti = {
	.match_table = serial_shakti_match,
	.init = serial_shakti_init
};
