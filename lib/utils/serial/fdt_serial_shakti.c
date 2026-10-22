/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Vijai Kumar K <vijai@behindbytes.com>
 *
 */

#अन्तर्भाव <sbi_utils/fdt/fdt_helper.h>
#अन्तर्भाव <sbi_utils/serial/fdt_serial.h>
#अन्तर्भाव <sbi_utils/serial/shakti-uart.h>

स्थिर पूर्णाङ्क serial_shakti_init(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoff,
			      ध्रुव संरचना fdt_मेलनम् *मेलनम्)
{
	पूर्णाङ्क rc;
	संरचना platform_uart_दत्त uart = { 0 };

	rc = fdt_parse_shakti_uart_ग्रन्थिः(fdt, nodeoff, &uart);
	यदि (rc)
		प्रत्यागम rc;

	प्रत्यागम shakti_uart_init(uart.addr, uart.freq, uart.baud);
}

स्थिर ध्रुव संरचना fdt_मेलनम् serial_shakti_मेलनम्[] = {
	{ .compatible = "shakti,uart0" },
	{ },
};

ध्रुव संरचना fdt_driver fdt_serial_shakti = {
	.मेलनम्_सारणी = serial_shakti_मेलनम्,
	.init = serial_shakti_init
};
