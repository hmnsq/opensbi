/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#समावेश <sbi_utils/fdt/fdt_helper.h>
#समावेश <sbi_utils/serial/fdt_serial.h>
#समावेश <sbi_utils/serial/sifive-uart.h>

अटल पूर्णांक serial_sifive_init(स्थिर व्योम *fdt, पूर्णांक nodeoff,
			      स्थिर काष्ठा fdt_match *match)
{
	पूर्णांक rc;
	काष्ठा platform_uart_तथ्य uart = { 0 };

	rc = fdt_parse_sifive_uart_गाँठ(fdt, nodeoff, &uart);
	अगर (rc)
		वापस rc;

	वापस sifive_uart_init(uart.addr, uart.freq, uart.baud);
}

अटल स्थिर काष्ठा fdt_match serial_sifive_match[] = {
	{ .compatible = "sifive,fu540-c000-uart" },
	{ .compatible = "sifive,uart0" },
	{ },
};

स्थिर काष्ठा fdt_driver fdt_serial_sifive = {
	.match_table = serial_sifive_match,
	.init = serial_sifive_init
};
