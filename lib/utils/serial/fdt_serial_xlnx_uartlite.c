/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Alistair Francis <alistair.francis@wdc.com>
 */

#ضمن <sbi_utils/fdt/fdt_helper.h>
#ضمن <sbi_utils/serial/fdt_serial.h>
#ضمن <sbi_utils/serial/xlnx_uartlite.h>

ساكن صحيح serial_xlnx_uartlite_init(ثابت فراغ *fdt, صحيح nodeoff,
				     ثابت بنية fdt_match *match)
{
	صحيح rc;
	بنية platform_uart_data uart = { 0 };

	rc = fdt_parse_xlnx_uartlite_عقدة(fdt, nodeoff, &uart);
	إذا (rc)
		أرجع rc;

	أرجع xlnx_uartlite_init(uart.addr);
}

ساكن ثابت بنية fdt_match serial_xlnx_uartlite_match[] = {
	{ .compatible = "xlnx,xps-uartlite-1.00.a" },
	{ },
};

ثابت بنية fdt_driver fdt_serial_xlnx_uartlite = {
	.match_table = serial_xlnx_uartlite_match,
	.init = serial_xlnx_uartlite_init,
};
