/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Vijai Kumar K <vijai@behindbytes.com>
 *
 */

#समावेश <sbi_utils/fdt/fdt_helper.h>
#समावेश <sbi_utils/serial/fdt_serial.h>
#समावेश <sbi_utils/serial/shakti-uart.h>

अटल पूर्णांक serial_shakti_init(स्थिर व्योम *fdt, पूर्णांक nodeoff,
			      स्थिर काष्ठा fdt_match *match)
{
	पूर्णांक rc;
	काष्ठा platform_uart_तथ्य uart = { 0 };

	rc = fdt_parse_shakti_uart_गाँठ(fdt, nodeoff, &uart);
	अगर (rc)
		वापस rc;

	वापस shakti_uart_init(uart.addr, uart.freq, uart.baud);
}

अटल स्थिर काष्ठा fdt_match serial_shakti_match[] = {
	{ .compatible = "shakti,uart0" },
	{ },
};

स्थिर काष्ठा fdt_driver fdt_serial_shakti = {
	.match_table = serial_shakti_match,
	.init = serial_shakti_init
};
