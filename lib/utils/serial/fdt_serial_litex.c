/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Gabriel Somlo
 *
 * Authors:
 *   Gabriel Somlo <gsomlo@gmail.com>
 */

#অন্তর্ভাব <sbi/sbi_error.h>
#অন্তর্ভাব <sbi_utils/fdt/fdt_helper.h>
#অন্তর্ভাব <sbi_utils/serial/fdt_serial.h>
#অন্তর্ভাব <sbi_utils/serial/litex-uart.h>

স্থির পূর্ণসংখ্যা serial_litex_init(ধ্রুবক শূন্য *fdt, পূর্ণসংখ্যা nodeoff,
			     ধ্রুবক কাঠামো fdt_match *match)
{
	uint64_t reg_addr, reg_আকার;
	পূর্ণসংখ্যা rc;

	যদি (nodeoff < 0 || !fdt)
		ফেরত SBI_ENODEV;

	rc = fdt_get_গ্রন্থি_addr_আকার(fdt, nodeoff, 0, &reg_addr, &reg_আকার);
	যদি (rc < 0 || !reg_addr || !reg_আকার)
		ফেরত SBI_ENODEV;

	ফেরত litex_uart_init(reg_addr);
}

স্থির ধ্রুবক কাঠামো fdt_match serial_litex_match[] = {
	{ .compatible = "litex,liteuart" },
	{ },
};

ধ্রুবক কাঠামো fdt_driver fdt_serial_litex = {
	.match_table = serial_litex_match,
	.init = serial_litex_init
};
