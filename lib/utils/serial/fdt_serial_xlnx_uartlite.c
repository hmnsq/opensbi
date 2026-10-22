/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Alistair Francis <alistair.francis@wdc.com>
 */

#समावेश <sbi_utils/fdt/fdt_helper.h>
#समावेश <sbi_utils/serial/fdt_serial.h>
#समावेश <sbi_utils/serial/xlnx_uartlite.h>

अटल पूर्णांक serial_xlnx_uartlite_init(स्थिर व्योम *fdt, पूर्णांक nodeoff,
				     स्थिर काष्ठा fdt_match *match)
{
	पूर्णांक rc;
	काष्ठा platform_uart_तथ्य uart = { 0 };

	rc = fdt_parse_xlnx_uartlite_गाँठ(fdt, nodeoff, &uart);
	अगर (rc)
		वापस rc;

	वापस xlnx_uartlite_init(uart.addr);
}

अटल स्थिर काष्ठा fdt_match serial_xlnx_uartlite_match[] = {
	{ .compatible = "xlnx,xps-uartlite-1.00.a" },
	{ },
};

स्थिर काष्ठा fdt_driver fdt_serial_xlnx_uartlite = {
	.match_table = serial_xlnx_uartlite_match,
	.init = serial_xlnx_uartlite_init,
};
