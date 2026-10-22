/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#अन्तर्भाव <sbi_utils/fdt/fdt_helper.h>
#अन्तर्भाव <sbi_utils/serial/fdt_serial.h>
#अन्तर्भाव <sbi_utils/serial/sifive-uart.h>

स्थिर पूर्णाङ्क serial_sifive_init(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoff,
			      ध्रुव संरचना fdt_मेलनम् *मेलनम्)
{
	पूर्णाङ्क rc;
	संरचना platform_uart_दत्त uart = { 0 };

	rc = fdt_parse_sifive_uart_ग्रन्थिः(fdt, nodeoff, &uart);
	यदि (rc)
		प्रत्यागम rc;

	प्रत्यागम sifive_uart_init(uart.addr, uart.freq, uart.baud);
}

स्थिर ध्रुव संरचना fdt_मेलनम् serial_sifive_मेलनम्[] = {
	{ .compatible = "sifive,fu540-c000-uart" },
	{ .compatible = "sifive,uart0" },
	{ },
};

ध्रुव संरचना fdt_driver fdt_serial_sifive = {
	.मेलनम्_सारणी = serial_sifive_मेलनम्,
	.init = serial_sifive_init
};
