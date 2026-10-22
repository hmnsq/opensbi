/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Alistair Francis <alistair.francis@wdc.com>
 */

#અન્તર્ભાવ <sbi_utils/fdt/fdt_helper.h>
#અન્તર્ભાવ <sbi_utils/serial/fdt_serial.h>
#અન્તર્ભાવ <sbi_utils/serial/xlnx_uartlite.h>

સ્થિર પૂર્ણાંક serial_xlnx_uartlite_init(અચલ શૂન્ય *fdt, પૂર્ણાંક nodeoff,
				     અચલ સંરચના fdt_match *match)
{
	પૂર્ણાંક rc;
	સંરચના platform_uart_data uart = { 0 };

	rc = fdt_parse_xlnx_uartlite_ગાંઠ(fdt, nodeoff, &uart);
	જો (rc)
		પાછા rc;

	પાછા xlnx_uartlite_init(uart.addr);
}

સ્થિર અચલ સંરચના fdt_match serial_xlnx_uartlite_match[] = {
	{ .compatible = "xlnx,xps-uartlite-1.00.a" },
	{ },
};

અચલ સંરચના fdt_driver fdt_serial_xlnx_uartlite = {
	.match_table = serial_xlnx_uartlite_match,
	.init = serial_xlnx_uartlite_init,
};
