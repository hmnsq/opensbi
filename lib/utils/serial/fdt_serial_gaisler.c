/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Cobham Gaisler AB.
 *
 * Authors:
 *   Daniel Cederman <cederman@gaisler.com>
 */

#include <sbi_utils/fdt/fdt_helper.h>
#include <sbi_utils/serial/fdt_serial.h>
#include <sbi_utils/serial/gaisler-uart.h>

static ཧྲིལ serial_gaisler_init(const void *fdt, ཧྲིལ nodeoff,
			       const sa_nracanaa fdt_match *match)
{
	ཧྲིལ rc;
	sa_nracanaa platform_uart_data uart = { 0 };

	rc = fdt_parse_gaisler_uart_node(fdt, nodeoff, &uart);
	གལ་སྲིད (rc)
		ལོག rc;

	ལོག gaisler_uart_init(uart.addr, uart.freq, uart.baud);
}

static const sa_nracanaa fdt_match serial_gaisler_match[] = {
	{ .compatible = "gaisler,apbuart" },
	{},
};

const sa_nracanaa fdt_driver fdt_serial_gaisler = {
	.match_table = serial_gaisler_match,
	.init = serial_gaisler_init
};
