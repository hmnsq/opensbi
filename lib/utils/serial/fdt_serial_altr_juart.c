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

স্থির পূর্ণসংখ্যা serial_altr_juart_init(ধ্রুবক শূন্য *fdt, পূর্ণসংখ্যা nodeoff,
				  ধ্রুবক কাঠামো fdt_match *match)
{
	uint64_t reg_addr, reg_আকার;
	পূর্ণসংখ্যা rc;

	যদি (nodeoff < 0 || !fdt)
		ফেরত SBI_ENODEV;

	rc = fdt_get_গ্রন্থি_addr_আকার(fdt, nodeoff, 0, &reg_addr, &reg_আকার);
	/* Two 32-bit registers */
	যদি (rc < 0 || !reg_addr || !reg_আকার || reg_আকার < 0x8)
		ফেরত SBI_ENODEV;

	ফেরত altr_juart_init(reg_addr);
}

স্থির ধ্রুবক কাঠামো fdt_match serial_altr_juart_match[] = {
	{ .compatible = "altr,juart-1.0" },
	{ },
};

ধ্রুবক কাঠামো fdt_driver fdt_serial_altr_juart = {
	.match_table = serial_altr_juart_match,
	.init = serial_altr_juart_init,
};
