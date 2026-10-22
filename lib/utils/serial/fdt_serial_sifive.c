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

स्थिर पूर्णाङ्क serial_sifive_init(अचर शून्य *fdt, पूर्णाङ्क nodeoff,
			      अचर संरचना fdt_match *match)
{
	पूर्णाङ्क rc;
	संरचना platform_uart_data uart = { 0 };

	rc = fdt_parse_sifive_uart_गाँठो(fdt, nodeoff, &uart);
	यदि (rc)
		फर्काऊ rc;

	फर्काऊ sifive_uart_init(uart.addr, uart.freq, uart.baud);
}

स्थिर अचर संरचना fdt_match serial_sifive_match[] = {
	{ .compatible = "sifive,fu540-c000-uart" },
	{ .compatible = "sifive,uart0" },
	{ },
};

अचर संरचना fdt_driver fdt_serial_sifive = {
	.match_table = serial_sifive_match,
	.init = serial_sifive_init
};
