/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Alistair Francis <alistair.francis@wdc.com>
 */

#அந்தர்பாவ <sbi_utils/fdt/fdt_helper.h>
#அந்தர்பாவ <sbi_utils/serial/fdt_serial.h>
#அந்தர்பாவ <sbi_utils/serial/xlnx_uartlite.h>

நிலையான முழு serial_xlnx_uartlite_init(மாறிலி வெறுமை *fdt, முழு nodeoff,
				     மாறிலி கட்டமைப்பு fdt_match *match)
{
	முழு rc;
	கட்டமைப்பு platform_uart_data uart = { 0 };

	rc = fdt_parse_xlnx_uartlite_முனை(fdt, nodeoff, &uart);
	எனில் (rc)
		திரும்பு rc;

	திரும்பு xlnx_uartlite_init(uart.addr);
}

நிலையான மாறிலி கட்டமைப்பு fdt_match serial_xlnx_uartlite_match[] = {
	{ .compatible = "xlnx,xps-uartlite-1.00.a" },
	{ },
};

மாறிலி கட்டமைப்பு fdt_driver fdt_serial_xlnx_uartlite = {
	.match_table = serial_xlnx_uartlite_match,
	.init = serial_xlnx_uartlite_init,
};
