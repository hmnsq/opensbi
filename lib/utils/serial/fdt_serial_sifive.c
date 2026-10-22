/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#شامل <sbi_utils/fdt/fdt_helper.h>
#شامل <sbi_utils/serial/fdt_serial.h>
#شامل <sbi_utils/serial/sifive-uart.h>

ساکن صحیح serial_sifive_init(ثابت خالی *fdt, صحیح nodeoff,
			      ثابت ساخت fdt_مطابقت *مطابقت)
{
	صحیح rc;
	ساخت platform_uart_ڈیٹا uart = { 0 };

	rc = fdt_parse_sifive_uart_گرہ(fdt, nodeoff, &uart);
	اگر (rc)
		واپس rc;

	واپس sifive_uart_init(uart.addr, uart.freq, uart.baud);
}

ساکن ثابت ساخت fdt_مطابقت serial_sifive_مطابقت[] = {
	{ .compatible = "sifive,fu540-c000-uart" },
	{ .compatible = "sifive,uart0" },
	{ },
};

ثابت ساخت fdt_driver fdt_serial_sifive = {
	.مطابقت_جدول = serial_sifive_مطابقت,
	.init = serial_sifive_init
};
