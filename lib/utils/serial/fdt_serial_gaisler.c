/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Cobham Gaisler AB.
 *
 * Authors:
 *   Daniel Cederman <cederman@gaisler.com>
 */

#अन्तर्भाव <sbi_utils/fdt/fdt_helper.h>
#अन्तर्भाव <sbi_utils/serial/fdt_serial.h>
#अन्तर्भाव <sbi_utils/serial/gaisler-uart.h>

स्थिर पूर्णाङ्क serial_gaisler_init(अचर शून्य *fdt, पूर्णाङ्क nodeoff,
			       अचर संरचना fdt_match *match)
{
	पूर्णाङ्क rc;
	संरचना platform_uart_data uart = { 0 };

	rc = fdt_parse_gaisler_uart_गाँठो(fdt, nodeoff, &uart);
	यदि (rc)
		फर्काऊ rc;

	फर्काऊ gaisler_uart_init(uart.addr, uart.freq, uart.baud);
}

स्थिर अचर संरचना fdt_match serial_gaisler_match[] = {
	{ .compatible = "gaisler,apbuart" },
	{},
};

अचर संरचना fdt_driver fdt_serial_gaisler = {
	.match_table = serial_gaisler_match,
	.init = serial_gaisler_init
};
