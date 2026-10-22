/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Alistair Francis <alistair.francis@wdc.com>
 */

#شامل <sbi_utils/fdt/fdt_helper.h>
#شامل <sbi_utils/serial/fdt_serial.h>
#شامل <sbi_utils/serial/xlnx_uartlite.h>

ساکن صحیح serial_xlnx_uartlite_init(ثابت خالی *fdt, صحیح nodeoff,
				     ثابت ساخت fdt_مطابقت *مطابقت)
{
	صحیح rc;
	ساخت platform_uart_ڈیٹا uart = { 0 };

	rc = fdt_parse_xlnx_uartlite_گرہ(fdt, nodeoff, &uart);
	اگر (rc)
		واپس rc;

	واپس xlnx_uartlite_init(uart.addr);
}

ساکن ثابت ساخت fdt_مطابقت serial_xlnx_uartlite_مطابقت[] = {
	{ .compatible = "xlnx,xps-uartlite-1.00.a" },
	{ },
};

ثابت ساخت fdt_driver fdt_serial_xlnx_uartlite = {
	.مطابقت_جدول = serial_xlnx_uartlite_مطابقت,
	.init = serial_xlnx_uartlite_init,
};
