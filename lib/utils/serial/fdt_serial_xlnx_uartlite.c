/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Alistair Francis <alistair.francis@wdc.com>
 */

#অন্তর্ভাব <sbi_utils/fdt/fdt_helper.h>
#অন্তর্ভাব <sbi_utils/serial/fdt_serial.h>
#অন্তর্ভাব <sbi_utils/serial/xlnx_uartlite.h>

স্থির পূর্ণসংখ্যা serial_xlnx_uartlite_init(ধ্রুবক শূন্য *fdt, পূর্ণসংখ্যা nodeoff,
				     ধ্রুবক কাঠামো fdt_match *match)
{
	পূর্ণসংখ্যা rc;
	কাঠামো platform_uart_তথ্য uart = { 0 };

	rc = fdt_parse_xlnx_uartlite_গ্রন্থি(fdt, nodeoff, &uart);
	যদি (rc)
		ফেরত rc;

	ফেরত xlnx_uartlite_init(uart.addr);
}

স্থির ধ্রুবক কাঠামো fdt_match serial_xlnx_uartlite_match[] = {
	{ .compatible = "xlnx,xps-uartlite-1.00.a" },
	{ },
};

ধ্রুবক কাঠামো fdt_driver fdt_serial_xlnx_uartlite = {
	.match_table = serial_xlnx_uartlite_match,
	.init = serial_xlnx_uartlite_init,
};
