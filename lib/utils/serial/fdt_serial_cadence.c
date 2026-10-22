/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 StarFive Technology Co., Ltd.
 *
 * Author: Jun Liang Tan <junliang.tan@linux.starfivetech.com>
 */

#অন্তর্ভাব <sbi_utils/fdt/fdt_helper.h>
#অন্তর্ভাব <sbi_utils/serial/fdt_serial.h>
#অন্তর্ভাব <sbi_utils/serial/cadence-uart.h>

স্থির পূর্ণসংখ্যা serial_cadence_init(ধ্রুবক শূন্য *fdt, পূর্ণসংখ্যা nodeoff,
			       ধ্রুবক কাঠামো fdt_match *match)
{
	পূর্ণসংখ্যা rc;
	কাঠামো platform_uart_তথ্য uart = { 0 };

	rc = fdt_parse_uart_গ্রন্থি(fdt, nodeoff, &uart);
	যদি (rc)
		ফেরত rc;

	ফেরত cadence_uart_init(uart.addr, uart.freq, uart.baud);
}

স্থির ধ্রুবক কাঠামো fdt_match serial_cadence_match[] = {
	{ .compatible = "cdns,uart-r1p8", },
	{ .compatible = "cdns,uart-r1p12" },
	{ .compatible = "starfive,jh8100-uart" },
	{ },
};

ধ্রুবক কাঠামো fdt_driver fdt_serial_cadence = {
	.match_table = serial_cadence_match,
	.init = serial_cadence_init
};
