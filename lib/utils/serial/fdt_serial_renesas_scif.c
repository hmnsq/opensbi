// SPDX-License-Identifier: BSD-2-Clause
/*
 * Copyright (C) 2022 Renesas Electronics Corporation
 */

#অন্তর্ভাৱ <sbi_utils/fdt/fdt_helper.h>
#অন্তর্ভাৱ <sbi_utils/serial/fdt_serial.h>
#অন্তর্ভাৱ <sbi_utils/serial/renesas-scif.h>

স্থির পূর্ণাঙ্ক serial_renesas_scif_init(ধ্রুৱ শূন্য *fdt, পূর্ণাঙ্ক nodeoff,
				    ধ্রুৱ সংরচনা fdt_match *match)
{
	সংরচনা platform_uart_data uart = { 0 };
	পূর্ণাঙ্ক ret;

	ret = fdt_parse_renesas_scif_গ্ৰন্থি(fdt, nodeoff, &uart);
	যদি (ret)
		ঘূৰাই ret;

	ঘূৰাই renesas_scif_init(uart.addr, uart.freq, uart.baud);
}

স্থির ধ্রুৱ সংরচনা fdt_match serial_renesas_scif_match[] = {
	{ .compatible = "renesas,scif-r9a07g043" },
	{ /* sentinel */ }
};

ধ্রুৱ সংরচনা fdt_driver fdt_serial_renesas_scif = {
	.match_table = serial_renesas_scif_match,
	.init = serial_renesas_scif_init
};
