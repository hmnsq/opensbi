/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ضمن <sbi_utils/fdt/fdt_helper.h>
#ضمن <sbi_utils/serial/fdt_serial.h>
#ضمن <sbi_utils/serial/sifive-uart.h>

ساكن صحيح serial_sifive_init(ثابت فراغ *fdt, صحيح nodeoff,
			      ثابت بنية fdt_match *match)
{
	صحيح rc;
	بنية platform_uart_data uart = { 0 };

	rc = fdt_parse_sifive_uart_node(fdt, nodeoff, &uart);
	𐤀𐤌 (rc)
		أرجع rc;

	أرجع sifive_uart_init(uart.addr, uart.freq, uart.baud);
}

ساكن ثابت بنية fdt_match serial_sifive_match[] = {
	{ .compatible = "sifive,fu540-c000-uart" },
	{ .compatible = "sifive,uart0" },
	{ },
};

ثابت بنية fdt_driver fdt_serial_sifive = {
	.match_table = serial_sifive_match,
	.init = serial_sifive_init
};
