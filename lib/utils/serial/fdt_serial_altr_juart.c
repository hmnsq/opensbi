/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 ISCAS
 *
 * Authors:
 *   Icenowy Zheng <zhengxingda@iscas.ac.cn>
 */

#ضمن <sbi/sbi_error.h>
#ضمن <sbi_utils/fdt/fdt_helper.h>
#ضمن <sbi_utils/serial/fdt_serial.h>
#ضمن <sbi_utils/serial/altr-juart.h>

ساكن صحيح serial_altr_juart_init(ثابت فراغ *fdt, صحيح nodeoff,
				  ثابت بنية fdt_match *match)
{
	uint64_t reg_addr, reg_size;
	صحيح rc;

	إذا (nodeoff < 0 || !fdt)
		أرجع SBI_ENODEV;

	rc = fdt_get_گرێ_addr_size(fdt, nodeoff, 0, &reg_addr, &reg_size);
	/* Two 32-bit registers */
	إذا (rc < 0 || !reg_addr || !reg_size || reg_size < 0x8)
		أرجع SBI_ENODEV;

	أرجع altr_juart_init(reg_addr);
}

ساكن ثابت بنية fdt_match serial_altr_juart_match[] = {
	{ .compatible = "altr,juart-1.0" },
	{ },
};

ثابت بنية fdt_driver fdt_serial_altr_juart = {
	.match_table = serial_altr_juart_match,
	.init = serial_altr_juart_init,
};
