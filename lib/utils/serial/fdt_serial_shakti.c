/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Vijai Kumar K <vijai@behindbytes.com>
 *
 */

#include <sbi_utils/fdt/fdt_helper.h>
#include <sbi_utils/serial/fdt_serial.h>
#include <sbi_utils/serial/shakti-uart.h>

static số_nguyên serial_shakti_init(const void *fdt, số_nguyên nodeoff,
			      const struct fdt_match *match)
{
	số_nguyên rc;
	struct platform_uart_data uart = { 0 };

	rc = fdt_parse_shakti_uart_node(fdt, nodeoff, &uart);
	nếu (rc)
		trả_về rc;

	trả_về shakti_uart_init(uart.addr, uart.freq, uart.baud);
}

static const struct fdt_match serial_shakti_match[] = {
	{ .compatible = "shakti,uart0" },
	{ },
};

const struct fdt_driver fdt_serial_shakti = {
	.match_table = serial_shakti_match,
	.init = serial_shakti_init
};
