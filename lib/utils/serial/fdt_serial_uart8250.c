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
#समावेश <sbi_utils/serial/uart8250.h>

अटल पूर्णांक serial_uart8250_init(स्थिर व्योम *fdt, पूर्णांक nodeoff,
				स्थिर काष्ठा fdt_match *match)
{
	काष्ठा platform_uart_तथ्य uart = { 0 };
	ulong caps = (ulong)match->तथ्य;
	पूर्णांक rc;

	rc = fdt_parse_uart_गाँठ(fdt, nodeoff, &uart);
	अगर (rc)
		वापस rc;

	वापस uart8250_init(uart.addr, uart.freq, uart.baud,
			     uart.reg_shift, uart.reg_io_width,
			     uart.reg_offset, caps);
}

अटल स्थिर काष्ठा fdt_match serial_uart8250_match[] = {
	{ .compatible = "ns16550" },
	{ .compatible = "ns16550a" },
	{ .compatible = "snps,dw-apb-uart" },
	{ .compatible = "intel,xscale-uart",
	  .तथ्य = (व्योम *)UART_CAP_UUE },
	{ },
};

स्थिर काष्ठा fdt_driver fdt_serial_uart8250 = {
	.match_table = serial_uart8250_match,
	.init = serial_uart8250_init,
};
