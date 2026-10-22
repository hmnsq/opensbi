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

static 整数 serial_altr_juart_init(const void *fdt, 整数 nodeoff,
				  const struct fdt_match *match)
{
	uint64_t reg_addr, reg_size;
	整数 rc;

	如果 (nodeoff < 0 || !fdt)
		返回 SBI_ENODEV;

	rc = fdt_get_node_addr_size(fdt, nodeoff, 0, &reg_addr, &reg_size);
	/* Two 32-bit registers */
	如果 (rc < 0 || !reg_addr || !reg_size || reg_size < 0x8)
		返回 SBI_ENODEV;

	返回 altr_juart_init(reg_addr);
}

static const struct fdt_match serial_altr_juart_match[] = {
	{ .compatible = "altr,juart-1.0" },
	{ },
};

const struct fdt_driver fdt_serial_altr_juart = {
	.match_table = serial_altr_juart_match,
	.init = serial_altr_juart_init,
};
