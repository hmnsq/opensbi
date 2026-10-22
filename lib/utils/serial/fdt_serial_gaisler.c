/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Cobham Gaisler AB.
 *
 * Authors:
 *   Daniel Cederman <cederman@gaisler.com>
 */

#समावेश <sbi_utils/fdt/fdt_helper.h>
#समावेश <sbi_utils/serial/fdt_serial.h>
#समावेश <sbi_utils/serial/gaisler-uart.h>

अटल पूर्णांक serial_gaisler_init(स्थिर व्योम *fdt, पूर्णांक nodeoff,
			       स्थिर काष्ठा fdt_match *match)
{
	पूर्णांक rc;
	काष्ठा platform_uart_तथ्य uart = { 0 };

	rc = fdt_parse_gaisler_uart_गाँठ(fdt, nodeoff, &uart);
	अगर (rc)
		वापस rc;

	वापस gaisler_uart_init(uart.addr, uart.freq, uart.baud);
}

अटल स्थिर काष्ठा fdt_match serial_gaisler_match[] = {
	{ .compatible = "gaisler,apbuart" },
	{},
};

स्थिर काष्ठा fdt_driver fdt_serial_gaisler = {
	.match_table = serial_gaisler_match,
	.init = serial_gaisler_init
};
