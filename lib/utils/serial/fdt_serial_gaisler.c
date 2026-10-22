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

ساکن صحیح serial_gaisler_init(ثابت خالی *fdt, صحیح nodeoff,
			       ثابت ساخت fdt_مطابقت *مطابقت)
{
	صحیح rc;
	ساخت platform_uart_ڈیٹا uart = { 0 };

	rc = fdt_parse_gaisler_uart_گرہ(fdt, nodeoff, &uart);
	اگر (rc)
		واپس rc;

	واپس gaisler_uart_init(uart.addr, uart.freq, uart.baud);
}

ساکن ثابت ساخت fdt_مطابقت serial_gaisler_مطابقت[] = {
	{ .compatible = "gaisler,apbuart" },
	{},
};

ثابت ساخت fdt_driver fdt_serial_gaisler = {
	.مطابقت_جدول = serial_gaisler_مطابقت,
	.init = serial_gaisler_init
};
