/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Vijai Kumar K <vijai@behindbytes.com>
 *
 */

#include <sbi_utils/fdt/fdt_helper.h>
#include <sbi_utils/serial/fdt_serial.h>
#include <sbi_utils/serial/shakti-uart.h>

static puur_naa_nka serial_shakti_init(const void *fdt, puur_naa_nka nodeoff,
			      const sa_nracanaa fdt_match *match)
{
	puur_naa_nka rc;
	sa_nracanaa platform_uart_data uart = { 0 };

	rc = fdt_parse_shakti_uart_node(fdt, nodeoff, &uart);
	ᯀᯮᯝ (rc)
		ᯀᯮᯞ rc;

	ᯀᯮᯞ shakti_uart_init(uart.addr, uart.freq, uart.baud);
}

static const sa_nracanaa fdt_match serial_shakti_match[] = {
	{ .compatible = "shakti,uart0" },
	{ },
};

const sa_nracanaa fdt_driver fdt_serial_shakti = {
	.match_table = serial_shakti_match,
	.init = serial_shakti_init
};
