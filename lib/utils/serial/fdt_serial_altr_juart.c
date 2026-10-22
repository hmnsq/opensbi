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
				  ثابت بنية fdt_سمون *سمون)
{
	uint64_t reg_addr, reg_اندازه;
	صحيح rc;

	إذا (nodeoff < 0 || !fdt)
		أرجع SBI_ENODEV;

	rc = fdt_get_غوټه_addr_اندازه(fdt, nodeoff, 0, &reg_addr, &reg_اندازه);
	/* Two 32-bit registers */
	إذا (rc < 0 || !reg_addr || !reg_اندازه || reg_اندازه < 0x8)
		أرجع SBI_ENODEV;

	أرجع altr_juart_init(reg_addr);
}

ساكن ثابت بنية fdt_سمون serial_altr_juart_سمون[] = {
	{ .compatible = "altr,juart-1.0" },
	{ },
};

ثابت بنية fdt_driver fdt_serial_altr_juart = {
	.سمون_جدول = serial_altr_juart_سمون,
	.init = serial_altr_juart_init,
};
