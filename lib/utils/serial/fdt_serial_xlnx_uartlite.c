/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Alistair Francis <alistair.francis@wdc.com>
 */

#अन्तर्भाव <sbi_utils/fdt/fdt_helper.h>
#अन्तर्भाव <sbi_utils/serial/fdt_serial.h>
#अन्तर्भाव <sbi_utils/serial/xlnx_uartlite.h>

स्थिर पूर्णाङ्क serial_xlnx_uartlite_init(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoff,
				     ध्रुव संरचना fdt_match *match)
{
	पूर्णाङ्क rc;
	संरचना platform_uart_data uart = { 0 };

	rc = fdt_parse_xlnx_uartlite_गाँठ(fdt, nodeoff, &uart);
	यदि (rc)
		प्रत्यागम rc;

	प्रत्यागम xlnx_uartlite_init(uart.addr);
}

स्थिर ध्रुव संरचना fdt_match serial_xlnx_uartlite_match[] = {
	{ .compatible = "xlnx,xps-uartlite-1.00.a" },
	{ },
};

ध्रुव संरचना fdt_driver fdt_serial_xlnx_uartlite = {
	.match_table = serial_xlnx_uartlite_match,
	.init = serial_xlnx_uartlite_init,
};
