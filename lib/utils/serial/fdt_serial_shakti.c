/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Vijai Kumar K <vijai@behindbytes.com>
 *
 */

#अन्तर्भाव <sbi_utils/fdt/fdt_helper.h>
#अन्तर्भाव <sbi_utils/serial/fdt_serial.h>
#अन्तर्भाव <sbi_utils/serial/shakti-uart.h>

स्थिर पूर्णाङ्क serial_shakti_init(अचर शून्य *fdt, पूर्णाङ्क nodeoff,
			      अचर संरचना fdt_match *match)
{
	पूर्णाङ्क rc;
	संरचना platform_uart_data uart = { 0 };

	rc = fdt_parse_shakti_uart_गाँठो(fdt, nodeoff, &uart);
	यदि (rc)
		फर्काऊ rc;

	फर्काऊ shakti_uart_init(uart.addr, uart.freq, uart.baud);
}

स्थिर अचर संरचना fdt_match serial_shakti_match[] = {
	{ .compatible = "shakti,uart0" },
	{ },
};

अचर संरचना fdt_driver fdt_serial_shakti = {
	.match_table = serial_shakti_match,
	.init = serial_shakti_init
};
