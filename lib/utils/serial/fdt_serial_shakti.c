/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Vijai Kumar K <vijai@behindbytes.com>
 *
 */

#شامل <sbi_utils/fdt/fdt_helper.h>
#شامل <sbi_utils/serial/fdt_serial.h>
#شامل <sbi_utils/serial/shakti-uart.h>

ساکن صحیح serial_shakti_init(ثابت خالی *fdt, صحیح nodeoff,
			      ثابت ساخت fdt_مطابقت *مطابقت)
{
	صحیح rc;
	ساخت platform_uart_ڈیٹا uart = { 0 };

	rc = fdt_parse_shakti_uart_گرہ(fdt, nodeoff, &uart);
	اگر (rc)
		واپس rc;

	واپس shakti_uart_init(uart.addr, uart.freq, uart.baud);
}

ساکن ثابت ساخت fdt_مطابقت serial_shakti_مطابقت[] = {
	{ .compatible = "shakti,uart0" },
	{ },
};

ثابت ساخت fdt_driver fdt_serial_shakti = {
	.مطابقت_جدول = serial_shakti_مطابقت,
	.init = serial_shakti_init
};
