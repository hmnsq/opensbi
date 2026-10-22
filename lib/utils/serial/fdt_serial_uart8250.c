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

ایستا صحیح serial_uart8250_init(ثابت تهی *fdt, صحیح nodeoff,
				ثابت ساختار fdt_match *match)
{
	ساختار platform_uart_data uart = { 0 };
	ulong caps = (ulong)match->data;
	صحیح rc;

	rc = fdt_parse_uart_گره(fdt, nodeoff, &uart);
	اگر (rc)
		بازگشت rc;

	بازگشت uart8250_init(uart.addr, uart.freq, uart.baud,
			     uart.reg_shift, uart.reg_io_width,
			     uart.reg_offset, caps);
}

ایستا ثابت ساختار fdt_match serial_uart8250_match[] = {
	{ .compatible = "ns16550" },
	{ .compatible = "ns16550a" },
	{ .compatible = "snps,dw-apb-uart" },
	{ .compatible = "intel,xscale-uart",
	  .data = (تهی *)UART_CAP_UUE },
	{ },
};

ثابت ساختار fdt_driver fdt_serial_uart8250 = {
	.match_table = serial_uart8250_match,
	.init = serial_uart8250_init,
};
