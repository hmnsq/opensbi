/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 ISCAS
 *
 * Authors:
 *   Icenowy Zheng <zhengxingda@iscas.ac.cn>
 */

#شامل <sbi/sbi_error.h>
#شامل <sbi_utils/fdt/fdt_helper.h>
#شامل <sbi_utils/serial/fdt_serial.h>
#شامل <sbi_utils/serial/altr-juart.h>

ساکن صحیح serial_altr_juart_init(ثابت خالی *fdt, صحیح nodeoff,
				  ثابت ساخت fdt_مطابقت *مطابقت)
{
	uint64_t reg_addr, reg_سائز;
	صحیح rc;

	اگر (nodeoff < 0 || !fdt)
		واپس SBI_ENODEV;

	rc = fdt_get_گرہ_addr_سائز(fdt, nodeoff, 0, &reg_addr, &reg_سائز);
	/* Two 32-bit registers */
	اگر (rc < 0 || !reg_addr || !reg_سائز || reg_سائز < 0x8)
		واپس SBI_ENODEV;

	واپس altr_juart_init(reg_addr);
}

ساکن ثابت ساخت fdt_مطابقت serial_altr_juart_مطابقت[] = {
	{ .compatible = "altr,juart-1.0" },
	{ },
};

ثابت ساخت fdt_driver fdt_serial_altr_juart = {
	.مطابقت_جدول = serial_altr_juart_مطابقت,
	.init = serial_altr_juart_init,
};
