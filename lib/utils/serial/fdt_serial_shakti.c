/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Vijai Kumar K <vijai@behindbytes.com>
 *
 */

#شامل <sbi_utils/fdt/fdt_helper.h>
#شامل <sbi_utils/serial/fdt_serial.h>
#شامل <sbi_utils/serial/shakti-uart.h>

ایستا صحیح serial_shakti_init(ثابت تهی *fdt, صحیح nodeoff,
			      ثابت ساختار fdt_match *match)
{
	صحیح rc;
	ساختار platform_uart_data uart = { 0 };

	rc = fdt_parse_shakti_uart_گره(fdt, nodeoff, &uart);
	اگر (rc)
		بازگشت rc;

	بازگشت shakti_uart_init(uart.addr, uart.freq, uart.baud);
}

ایستا ثابت ساختار fdt_match serial_shakti_match[] = {
	{ .compatible = "shakti,uart0" },
	{ },
};

ثابت ساختار fdt_driver fdt_serial_shakti = {
	.match_table = serial_shakti_match,
	.init = serial_shakti_init
};
