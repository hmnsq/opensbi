/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Vijai Kumar K <vijai@behindbytes.com>
 *
 */

#include <sbi_utils/fdt/fdt_helper.h>
#include <sbi_utils/serial/fdt_serial.h>
#include <sbi_utils/serial/shakti-uart.h>

static целое serial_shakti_init(const void *fdt, целое nodeoff,
			      const struct fdt_match *match)
{
	целое rc;
	struct platform_uart_data uart = { 0 };

	rc = fdt_parse_shakti_uart_node(fdt, nodeoff, &uart);
	если (rc)
		вернуть rc;

	вернуть shakti_uart_init(uart.addr, uart.freq, uart.baud);
}

static const struct fdt_match serial_shakti_match[] = {
	{ .compatible = "shakti,uart0" },
	{ },
};

const struct fdt_driver fdt_serial_shakti = {
	.match_table = serial_shakti_match,
	.init = serial_shakti_init
};
