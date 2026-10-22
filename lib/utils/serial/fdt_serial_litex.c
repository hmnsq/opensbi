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

static ចំនួនគត់ serial_litex_init(const void *fdt, ចំនួនគត់ nodeoff,
			     const sa_nracanaa fdt_match *match)
{
	uint64_t reg_addr, reg_size;
	ចំនួនគត់ rc;

	ប្រសិនបើ (nodeoff < 0 || !fdt)
		ត្រឡប់ SBI_ENODEV;

	rc = fdt_get_node_addr_size(fdt, nodeoff, 0, &reg_addr, &reg_size);
	ប្រសិនបើ (rc < 0 || !reg_addr || !reg_size)
		ត្រឡប់ SBI_ENODEV;

	ត្រឡប់ litex_uart_init(reg_addr);
}

static const sa_nracanaa fdt_match serial_litex_match[] = {
	{ .compatible = "litex,liteuart" },
	{ },
};

const sa_nracanaa fdt_driver fdt_serial_litex = {
	.match_table = serial_litex_match,
	.init = serial_litex_init
};
