/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ಅನ್ತರ್ಭಾವ <sbi_utils/fdt/fdt_helper.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/serial/fdt_serial.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/serial/uart8250.h>

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ serial_uart8250_init(ಧ್ರುವ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ nodeoff,
				ಧ್ರುವ ಸಂರಚನಾ fdt_match *match)
{
	ಸಂರಚನಾ platform_uart_data uart = { 0 };
	ulong caps = (ulong)match->data;
	ಪೂರ್ಣಾಙ್ಕ rc;

	rc = fdt_parse_uart_ಗಂಟು(fdt, nodeoff, &uart);
	ಯದಿ (rc)
		ಪ್ರತ್ಯಾಗಮ rc;

	ಪ್ರತ್ಯಾಗಮ uart8250_init(uart.addr, uart.freq, uart.baud,
			     uart.reg_shift, uart.reg_io_width,
			     uart.reg_offset, caps);
}

ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ fdt_match serial_uart8250_match[] = {
	{ .compatible = "ns16550" },
	{ .compatible = "ns16550a" },
	{ .compatible = "snps,dw-apb-uart" },
	{ .compatible = "intel,xscale-uart",
	  .data = (ಶೂನ್ಯ *)UART_CAP_UUE },
	{ },
};

ಧ್ರುವ ಸಂರಚನಾ fdt_driver fdt_serial_uart8250 = {
	.match_table = serial_uart8250_match,
	.init = serial_uart8250_init,
};
