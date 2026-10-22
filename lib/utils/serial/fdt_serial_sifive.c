/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#அந்தர்பாவ <sbi_utils/fdt/fdt_helper.h>
#அந்தர்பாவ <sbi_utils/serial/fdt_serial.h>
#அந்தர்பாவ <sbi_utils/serial/sifive-uart.h>

நிலையான முழு serial_sifive_init(மாறிலி வெறுமை *fdt, முழு nodeoff,
			      மாறிலி கட்டமைப்பு fdt_match *match)
{
	முழு rc;
	கட்டமைப்பு platform_uart_data uart = { 0 };

	rc = fdt_parse_sifive_uart_முனை(fdt, nodeoff, &uart);
	எனில் (rc)
		திரும்பு rc;

	திரும்பு sifive_uart_init(uart.addr, uart.freq, uart.baud);
}

நிலையான மாறிலி கட்டமைப்பு fdt_match serial_sifive_match[] = {
	{ .compatible = "sifive,fu540-c000-uart" },
	{ .compatible = "sifive,uart0" },
	{ },
};

மாறிலி கட்டமைப்பு fdt_driver fdt_serial_sifive = {
	.match_table = serial_sifive_match,
	.init = serial_sifive_init
};
