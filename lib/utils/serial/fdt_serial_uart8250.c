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
#شامل <sbi_utils/serial/uart8250.h>

ساکن صحیح serial_uart8250_init(ثابت خالی *fdt, صحیح nodeoff,
				ثابت ساخت fdt_مطابقت *مطابقت)
{
	ساخت platform_uart_ڈیٹا uart = { 0 };
	ulong caps = (ulong)مطابقت->ڈیٹا;
	صحیح rc;

	rc = fdt_parse_uart_گرہ(fdt, nodeoff, &uart);
	اگر (rc)
		واپس rc;

	واپس uart8250_init(uart.addr, uart.freq, uart.baud,
			     uart.reg_سرکاؤ, uart.reg_io_چوڑائی,
			     uart.reg_انحراف, caps);
}

ساکن ثابت ساخت fdt_مطابقت serial_uart8250_مطابقت[] = {
	{ .compatible = "ns16550" },
	{ .compatible = "ns16550a" },
	{ .compatible = "snps,dw-apb-uart" },
	{ .compatible = "intel,xscale-uart",
	  .ڈیٹا = (خالی *)UART_CAP_UUE },
	{ },
};

ثابت ساخت fdt_driver fdt_serial_uart8250 = {
	.مطابقت_جدول = serial_uart8250_مطابقت,
	.init = serial_uart8250_init,
};
