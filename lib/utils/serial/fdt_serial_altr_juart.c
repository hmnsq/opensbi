/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 ISCAS
 *
 * Authors:
 *   Icenowy Zheng <zhengxingda@iscas.ac.cn>
 */

#অন্তর্ভাব <sbi/sbi_error.h>
#অন্তর্ভাব <sbi_utils/fdt/fdt_helper.h>
#অন্তর্ভাব <sbi_utils/serial/fdt_serial.h>
#অন্তর্ভাব <sbi_utils/serial/altr-juart.h>

স্থির পূর্ণাঙ্ক serial_altr_juart_init(ধ্রুব শূন্য *fdt, পূর্ণাঙ্ক nodeoff,
				  ধ্রুব সংরচনা fdt_match *match)
{
	uint64_t reg_addr, reg_size;
	পূর্ণাঙ্ক rc;

	যদি (nodeoff < 0 || !fdt)
		প্রত্যাগম SBI_ENODEV;

	rc = fdt_get_গ্রন্থি_addr_size(fdt, nodeoff, 0, &reg_addr, &reg_size);
	/* Two 32-bit registers */
	যদি (rc < 0 || !reg_addr || !reg_size || reg_size < 0x8)
		প্রত্যাগম SBI_ENODEV;

	প্রত্যাগম altr_juart_init(reg_addr);
}

স্থির ধ্রুব সংরচনা fdt_match serial_altr_juart_match[] = {
	{ .compatible = "altr,juart-1.0" },
	{ },
};

ধ্রুব সংরচনা fdt_driver fdt_serial_altr_juart = {
	.match_table = serial_altr_juart_match,
	.init = serial_altr_juart_init,
};
