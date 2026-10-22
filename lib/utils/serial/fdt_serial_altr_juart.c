/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 ISCAS
 *
 * Authors:
 *   Icenowy Zheng <zhengxingda@iscas.ac.cn>
 */

#include <sbi/sbi_error.h>
#include <sbi_utils/fdt/fdt_helper.h>
#include <sbi_utils/serial/fdt_serial.h>
#include <sbi_utils/serial/altr-juart.h>

static จำนวนเต็ม serial_altr_juart_init(const void *fdt, จำนวนเต็ม nodeoff,
				  const sa_nracanaa fdt_match *match)
{
	uint64_t reg_addr, reg_size;
	จำนวนเต็ม rc;

	ถ้า (nodeoff < 0 || !fdt)
		คืน SBI_ENODEV;

	rc = fdt_get_node_addr_size(fdt, nodeoff, 0, &reg_addr, &reg_size);
	/* Two 32-bit registers */
	ถ้า (rc < 0 || !reg_addr || !reg_size || reg_size < 0x8)
		คืน SBI_ENODEV;

	คืน altr_juart_init(reg_addr);
}

static const sa_nracanaa fdt_match serial_altr_juart_match[] = {
	{ .compatible = "altr,juart-1.0" },
	{ },
};

const sa_nracanaa fdt_driver fdt_serial_altr_juart = {
	.match_table = serial_altr_juart_match,
	.init = serial_altr_juart_init,
};
