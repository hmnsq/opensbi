/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Vijai Kumar K <vijai@behindbytes.com>
 *
 */

#ضمن <sbi_utils/fdt/fdt_helper.h>
#ضمن <sbi_utils/serial/fdt_serial.h>
#ضمن <sbi_utils/serial/shakti-uart.h>

ساكن صحيح serial_shakti_init(ثابت فراغ *fdt, صحيح nodeoff,
			      ثابت بنية fdt_سمون *سمون)
{
	صحيح rc;
	بنية platform_uart_معلومات uart = { 0 };

	rc = fdt_parse_shakti_uart_غوټه(fdt, nodeoff, &uart);
	إذا (rc)
		أرجع rc;

	أرجع shakti_uart_init(uart.addr, uart.freq, uart.baud);
}

ساكن ثابت بنية fdt_سمون serial_shakti_سمون[] = {
	{ .compatible = "shakti,uart0" },
	{ },
};

ثابت بنية fdt_driver fdt_serial_shakti = {
	.سمون_جدول = serial_shakti_سمون,
	.init = serial_shakti_init
};
