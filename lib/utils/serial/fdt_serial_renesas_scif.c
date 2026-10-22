// SPDX-License-Identifier: BSD-2-Clause
/*
 * Copyright (C) 2022 Renesas Electronics Corporation
 */

#অন্তর্ভাব <sbi_utils/fdt/fdt_helper.h>
#অন্তর্ভাব <sbi_utils/serial/fdt_serial.h>
#অন্তর্ভাব <sbi_utils/serial/renesas-scif.h>

স্থির পূর্ণসংখ্যা serial_renesas_scif_init(ধ্রুবক শূন্য *fdt, পূর্ণসংখ্যা nodeoff,
				    ধ্রুবক কাঠামো fdt_match *match)
{
	কাঠামো platform_uart_তথ্য uart = { 0 };
	পূর্ণসংখ্যা ret;

	ret = fdt_parse_renesas_scif_গ্রন্থি(fdt, nodeoff, &uart);
	যদি (ret)
		ফেরত ret;

	ফেরত renesas_scif_init(uart.addr, uart.freq, uart.baud);
}

স্থির ধ্রুবক কাঠামো fdt_match serial_renesas_scif_match[] = {
	{ .compatible = "renesas,scif-r9a07g043" },
	{ /* sentinel */ }
};

ধ্রুবক কাঠামো fdt_driver fdt_serial_renesas_scif = {
	.match_table = serial_renesas_scif_match,
	.init = serial_renesas_scif_init
};
