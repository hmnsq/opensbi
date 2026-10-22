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

ایستا صحیح serial_xlnx_uartlite_init(ثابت تهی *fdt, صحیح nodeoff,
				     ثابت ساختار fdt_match *match)
{
	صحیح rc;
	ساختار platform_uart_data uart = { 0 };

	rc = fdt_parse_xlnx_uartlite_گره(fdt, nodeoff, &uart);
	اگر (rc)
		بازگشت rc;

	بازگشت xlnx_uartlite_init(uart.addr);
}

ایستا ثابت ساختار fdt_match serial_xlnx_uartlite_match[] = {
	{ .compatible = "xlnx,xps-uartlite-1.00.a" },
	{ },
};

ثابت ساختار fdt_driver fdt_serial_xlnx_uartlite = {
	.match_table = serial_xlnx_uartlite_match,
	.init = serial_xlnx_uartlite_init,
};
