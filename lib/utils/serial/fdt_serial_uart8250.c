/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#অন্তর্ভাব <sbi_utils/fdt/fdt_helper.h>
#অন্তর্ভাব <sbi_utils/serial/fdt_serial.h>
#অন্তর্ভাব <sbi_utils/serial/uart8250.h>

স্থির পূর্ণসংখ্যা serial_uart8250_init(ধ্রুবক শূন্য *fdt, পূর্ণসংখ্যা nodeoff,
				ধ্রুবক কাঠামো fdt_match *match)
{
	কাঠামো platform_uart_তথ্য uart = { 0 };
	ulong caps = (ulong)match->তথ্য;
	পূর্ণসংখ্যা rc;

	rc = fdt_parse_uart_গ্রন্থি(fdt, nodeoff, &uart);
	যদি (rc)
		ফেরত rc;

	ফেরত uart8250_init(uart.addr, uart.freq, uart.baud,
			     uart.reg_shift, uart.reg_io_width,
			     uart.reg_offset, caps);
}

স্থির ধ্রুবক কাঠামো fdt_match serial_uart8250_match[] = {
	{ .compatible = "ns16550" },
	{ .compatible = "ns16550a" },
	{ .compatible = "snps,dw-apb-uart" },
	{ .compatible = "intel,xscale-uart",
	  .তথ্য = (শূন্য *)UART_CAP_UUE },
	{ },
};

ধ্রুবক কাঠামো fdt_driver fdt_serial_uart8250 = {
	.match_table = serial_uart8250_match,
	.init = serial_uart8250_init,
};
