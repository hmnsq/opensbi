/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Vijai Kumar K <vijai@behindbytes.com>
 *
 */

#அந்தர்பாவ <sbi_utils/fdt/fdt_helper.h>
#அந்தர்பாவ <sbi_utils/serial/fdt_serial.h>
#அந்தர்பாவ <sbi_utils/serial/shakti-uart.h>

நிலையான முழு serial_shakti_init(மாறிலி வெறுமை *fdt, முழு nodeoff,
			      மாறிலி கட்டமைப்பு fdt_match *match)
{
	முழு rc;
	கட்டமைப்பு platform_uart_data uart = { 0 };

	rc = fdt_parse_shakti_uart_முனை(fdt, nodeoff, &uart);
	எனில் (rc)
		திரும்பு rc;

	திரும்பு shakti_uart_init(uart.addr, uart.freq, uart.baud);
}

நிலையான மாறிலி கட்டமைப்பு fdt_match serial_shakti_match[] = {
	{ .compatible = "shakti,uart0" },
	{ },
};

மாறிலி கட்டமைப்பு fdt_driver fdt_serial_shakti = {
	.match_table = serial_shakti_match,
	.init = serial_shakti_init
};
