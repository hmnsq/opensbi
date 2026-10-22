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

static පූර්ණ_සංඛ්‍යා serial_sifive_init(const void *fdt, පූර්ණ_සංඛ්‍යා nodeoff,
			      const sa_nracanaa fdt_match *match)
{
	පූර්ණ_සංඛ්‍යා rc;
	sa_nracanaa platform_uart_data uart = { 0 };

	rc = fdt_parse_sifive_uart_node(fdt, nodeoff, &uart);
	නම් (rc)
		ආපසු rc;

	ආපසු sifive_uart_init(uart.addr, uart.freq, uart.baud);
}

static const sa_nracanaa fdt_match serial_sifive_match[] = {
	{ .compatible = "sifive,fu540-c000-uart" },
	{ .compatible = "sifive,uart0" },
	{ },
};

const sa_nracanaa fdt_driver fdt_serial_sifive = {
	.match_table = serial_sifive_match,
	.init = serial_sifive_init
};
