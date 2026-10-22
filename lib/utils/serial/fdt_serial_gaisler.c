/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Cobham Gaisler AB.
 *
 * Authors:
 *   Daniel Cederman <cederman@gaisler.com>
 */

#அந்தர்பாவ <sbi_utils/fdt/fdt_helper.h>
#அந்தர்பாவ <sbi_utils/serial/fdt_serial.h>
#அந்தர்பாவ <sbi_utils/serial/gaisler-uart.h>

நிலையான முழு serial_gaisler_init(மாறிலி வெறுமை *fdt, முழு nodeoff,
			       மாறிலி கட்டமைப்பு fdt_match *match)
{
	முழு rc;
	கட்டமைப்பு platform_uart_data uart = { 0 };

	rc = fdt_parse_gaisler_uart_முனை(fdt, nodeoff, &uart);
	எனில் (rc)
		திரும்பு rc;

	திரும்பு gaisler_uart_init(uart.addr, uart.freq, uart.baud);
}

நிலையான மாறிலி கட்டமைப்பு fdt_match serial_gaisler_match[] = {
	{ .compatible = "gaisler,apbuart" },
	{},
};

மாறிலி கட்டமைப்பு fdt_driver fdt_serial_gaisler = {
	.match_table = serial_gaisler_match,
	.init = serial_gaisler_init
};
