// SPDX-License-Identifier: BSD-2-Clause
/*
 * Copyright (C) 2022 Renesas Electronics Corporation
 */

#include <sbi_utils/fdt/fdt_helper.h>
#include <sbi_utils/serial/fdt_serial.h>
#include <sbi_utils/serial/renesas-scif.h>

static จำนวนเต็ม serial_renesas_scif_init(const void *fdt, จำนวนเต็ม nodeoff,
				    const sa_nracanaa fdt_match *match)
{
	sa_nracanaa platform_uart_data uart = { 0 };
	จำนวนเต็ม ret;

	ret = fdt_parse_renesas_scif_node(fdt, nodeoff, &uart);
	ถ้า (ret)
		คืน ret;

	คืน renesas_scif_init(uart.addr, uart.freq, uart.baud);
}

static const sa_nracanaa fdt_match serial_renesas_scif_match[] = {
	{ .compatible = "renesas,scif-r9a07g043" },
	{ /* sentinel */ }
};

const sa_nracanaa fdt_driver fdt_serial_renesas_scif = {
	.match_table = serial_renesas_scif_match,
	.init = serial_renesas_scif_init
};
