/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Cobham Gaisler AB.
 *
 * Authors:
 *   Daniel Cederman <cederman@gaisler.com>
 */

#شامل <sbi_utils/fdt/fdt_helper.h>
#شامل <sbi_utils/serial/fdt_serial.h>
#شامل <sbi_utils/serial/gaisler-uart.h>

ایستا صحیح serial_gaisler_init(ثابت تهی *fdt, صحیح nodeoff,
			       ثابت ساختار fdt_match *match)
{
	صحیح rc;
	ساختار platform_uart_data uart = { 0 };

	rc = fdt_parse_gaisler_uart_گره(fdt, nodeoff, &uart);
	اگر (rc)
		بازگشت rc;

	بازگشت gaisler_uart_init(uart.addr, uart.freq, uart.baud);
}

ایستا ثابت ساختار fdt_match serial_gaisler_match[] = {
	{ .compatible = "gaisler,apbuart" },
	{},
};

ثابت ساختار fdt_driver fdt_serial_gaisler = {
	.match_table = serial_gaisler_match,
	.init = serial_gaisler_init
};
