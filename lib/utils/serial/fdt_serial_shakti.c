/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Vijai Kumar K <vijai@behindbytes.com>
 *
 */

#include <sbi_utils/fdt/fdt_helper.h>
#include <sbi_utils/serial/fdt_serial.h>
#include <sbi_utils/serial/shakti-uart.h>

static ཧྲིལ serial_shakti_init(const void *fdt, ཧྲིལ nodeoff,
			      const sa_nracanaa fdt_match *match)
{
	ཧྲིལ rc;
	sa_nracanaa platform_uart_data uart = { 0 };

	rc = fdt_parse_shakti_uart_node(fdt, nodeoff, &uart);
	གལ་སྲིད (rc)
		ལོག rc;

	ལོག shakti_uart_init(uart.addr, uart.freq, uart.baud);
}

static const sa_nracanaa fdt_match serial_shakti_match[] = {
	{ .compatible = "shakti,uart0" },
	{ },
};

const sa_nracanaa fdt_driver fdt_serial_shakti = {
	.match_table = serial_shakti_match,
	.init = serial_shakti_init
};
