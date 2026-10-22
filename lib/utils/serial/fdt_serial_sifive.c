/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#include <sbi_utils/fdt/fdt_helper.h>
#include <sbi_utils/serial/fdt_serial.h>
#include <sbi_utils/serial/sifive-uart.h>

static целое serial_sifive_init(const void *fdt, целое nodeoff,
			      const struct fdt_match *match)
{
	целое rc;
	struct platform_uart_data uart = { 0 };

	rc = fdt_parse_sifive_uart_node(fdt, nodeoff, &uart);
	если (rc)
		вернуть rc;

	вернуть sifive_uart_init(uart.addr, uart.freq, uart.baud);
}

static const struct fdt_match serial_sifive_match[] = {
	{ .compatible = "sifive,fu540-c000-uart" },
	{ .compatible = "sifive,uart0" },
	{ },
};

const struct fdt_driver fdt_serial_sifive = {
	.match_table = serial_sifive_match,
	.init = serial_sifive_init
};
