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
#அந்தர்பாவ <sbi_utils/serial/uart8250.h>

நிலையான முழு serial_uart8250_init(மாறிலி வெறுமை *fdt, முழு nodeoff,
				மாறிலி கட்டமைப்பு fdt_match *match)
{
	கட்டமைப்பு platform_uart_data uart = { 0 };
	ulong caps = (ulong)match->data;
	முழு rc;

	rc = fdt_parse_uart_முனை(fdt, nodeoff, &uart);
	எனில் (rc)
		திரும்பு rc;

	திரும்பு uart8250_init(uart.addr, uart.freq, uart.baud,
			     uart.reg_shift, uart.reg_io_width,
			     uart.reg_offset, caps);
}

நிலையான மாறிலி கட்டமைப்பு fdt_match serial_uart8250_match[] = {
	{ .compatible = "ns16550" },
	{ .compatible = "ns16550a" },
	{ .compatible = "snps,dw-apb-uart" },
	{ .compatible = "intel,xscale-uart",
	  .data = (வெறுமை *)UART_CAP_UUE },
	{ },
};

மாறிலி கட்டமைப்பு fdt_driver fdt_serial_uart8250 = {
	.match_table = serial_uart8250_match,
	.init = serial_uart8250_init,
};
