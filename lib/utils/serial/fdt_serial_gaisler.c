/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Cobham Gaisler AB.
 *
 * Authors:
 *   Daniel Cederman <cederman@gaisler.com>
 */

#ضمن <sbi_utils/fdt/fdt_helper.h>
#ضمن <sbi_utils/serial/fdt_serial.h>
#ضمن <sbi_utils/serial/gaisler-uart.h>

ساكن صحيح serial_gaisler_init(ثابت فراغ *fdt, صحيح nodeoff,
			       ثابت بنية fdt_سمون *سمون)
{
	صحيح rc;
	بنية platform_uart_معلومات uart = { 0 };

	rc = fdt_parse_gaisler_uart_غوټه(fdt, nodeoff, &uart);
	إذا (rc)
		أرجع rc;

	أرجع gaisler_uart_init(uart.addr, uart.freq, uart.baud);
}

ساكن ثابت بنية fdt_سمون serial_gaisler_سمون[] = {
	{ .compatible = "gaisler,apbuart" },
	{},
};

ثابت بنية fdt_driver fdt_serial_gaisler = {
	.سمون_جدول = serial_gaisler_سمون,
	.init = serial_gaisler_init
};
