/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 StarFive Technology Co., Ltd.
 *
 * Author: Jun Liang Tan <junliang.tan@linux.starfivetech.com>
 */

#include <sbi_utils/fdt/fdt_helper.h>
#include <sbi_utils/serial/fdt_serial.h>
#include <sbi_utils/serial/cadence-uart.h>

static 정수 serial_cadence_init(const void *fdt, 정수 nodeoff,
			       const struct fdt_match *match)
{
	정수 rc;
	struct platform_uart_data uart = { 0 };

	rc = fdt_parse_uart_node(fdt, nodeoff, &uart);
	만약 (rc)
		반환 rc;

	반환 cadence_uart_init(uart.addr, uart.freq, uart.baud);
}

static const struct fdt_match serial_cadence_match[] = {
	{ .compatible = "cdns,uart-r1p8", },
	{ .compatible = "cdns,uart-r1p12" },
	{ .compatible = "starfive,jh8100-uart" },
	{ },
};

const struct fdt_driver fdt_serial_cadence = {
	.match_table = serial_cadence_match,
	.init = serial_cadence_init
};
