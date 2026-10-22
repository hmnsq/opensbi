/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Gabriel Somlo
 *
 * Authors:
 *   Gabriel Somlo <gsomlo@gmail.com>
 */

#অন্তর্ভাৱ <sbi/sbi_error.h>
#অন্তর্ভাৱ <sbi_utils/fdt/fdt_helper.h>
#অন্তর্ভাৱ <sbi_utils/serial/fdt_serial.h>
#অন্তর্ভাৱ <sbi_utils/serial/litex-uart.h>

স্থির পূর্ণাঙ্ক serial_litex_init(ধ্রুৱ শূন্য *fdt, পূর্ণাঙ্ক nodeoff,
			     ধ্রুৱ সংরচনা fdt_match *match)
{
	uint64_t reg_addr, reg_size;
	পূর্ণাঙ্ক rc;

	যদি (nodeoff < 0 || !fdt)
		ঘূৰাই SBI_ENODEV;

	rc = fdt_get_গ্ৰন্থি_addr_size(fdt, nodeoff, 0, &reg_addr, &reg_size);
	যদি (rc < 0 || !reg_addr || !reg_size)
		ঘূৰাই SBI_ENODEV;

	ঘূৰাই litex_uart_init(reg_addr);
}

স্থির ধ্রুৱ সংরচনা fdt_match serial_litex_match[] = {
	{ .compatible = "litex,liteuart" },
	{ },
};

ধ্রুৱ সংরচনা fdt_driver fdt_serial_litex = {
	.match_table = serial_litex_match,
	.init = serial_litex_init
};
