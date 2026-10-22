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

ایستا صحیح serial_altr_juart_init(ثابت تهی *fdt, صحیح nodeoff,
				  ثابت ساختار fdt_match *match)
{
	uint64_t reg_addr, reg_size;
	صحیح rc;

	اگر (nodeoff < 0 || !fdt)
		بازگشت SBI_ENODEV;

	rc = fdt_get_گره_addr_size(fdt, nodeoff, 0, &reg_addr, &reg_size);
	/* Two 32-bit registers */
	اگر (rc < 0 || !reg_addr || !reg_size || reg_size < 0x8)
		بازگشت SBI_ENODEV;

	بازگشت altr_juart_init(reg_addr);
}

ایستا ثابت ساختار fdt_match serial_altr_juart_match[] = {
	{ .compatible = "altr,juart-1.0" },
	{ },
};

ثابت ساختار fdt_driver fdt_serial_altr_juart = {
	.match_table = serial_altr_juart_match,
	.init = serial_altr_juart_init,
};
