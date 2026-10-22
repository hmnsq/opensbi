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

ایستا صحیح serial_sifive_init(ثابت تهی *fdt, صحیح nodeoff,
			      ثابت ساختار fdt_match *match)
{
	صحیح rc;
	ساختار platform_uart_data uart = { 0 };

	rc = fdt_parse_sifive_uart_گره(fdt, nodeoff, &uart);
	اگر (rc)
		بازگشت rc;

	بازگشت sifive_uart_init(uart.addr, uart.freq, uart.baud);
}

ایستا ثابت ساختار fdt_match serial_sifive_match[] = {
	{ .compatible = "sifive,fu540-c000-uart" },
	{ .compatible = "sifive,uart0" },
	{ },
};

ثابت ساختار fdt_driver fdt_serial_sifive = {
	.match_table = serial_sifive_match,
	.init = serial_sifive_init
};
