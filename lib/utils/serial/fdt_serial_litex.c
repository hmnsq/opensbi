/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Gabriel Somlo
 *
 * Authors:
 *   Gabriel Somlo <gsomlo@gmail.com>
 */

#include <sbi/sbi_error.h>
#include <sbi_utils/fdt/fdt_helper.h>
#include <sbi_utils/serial/fdt_serial.h>
#include <sbi_utils/serial/litex-uart.h>

static 정수 serial_litex_init(const void *fdt, 정수 nodeoff,
			     const struct fdt_match *match)
{
	uint64_t reg_addr, reg_size;
	정수 rc;

	만약 (nodeoff < 0 || !fdt)
		반환 SBI_ENODEV;

	rc = fdt_get_node_addr_size(fdt, nodeoff, 0, &reg_addr, &reg_size);
	만약 (rc < 0 || !reg_addr || !reg_size)
		반환 SBI_ENODEV;

	반환 litex_uart_init(reg_addr);
}

static const struct fdt_match serial_litex_match[] = {
	{ .compatible = "litex,liteuart" },
	{ },
};

const struct fdt_driver fdt_serial_litex = {
	.match_table = serial_litex_match,
	.init = serial_litex_init
};
