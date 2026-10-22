/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Gabriel Somlo
 *
 * Authors:
 *   Gabriel Somlo <gsomlo@gmail.com>
 */

#شامل <sbi/sbi_error.h>
#شامل <sbi_utils/fdt/fdt_helper.h>
#شامل <sbi_utils/serial/fdt_serial.h>
#شامل <sbi_utils/serial/litex-uart.h>

ساکن صحیح serial_litex_init(ثابت خالی *fdt, صحیح nodeoff,
			     ثابت ساخت fdt_مطابقت *مطابقت)
{
	uint64_t reg_addr, reg_سائز;
	صحیح rc;

	اگر (nodeoff < 0 || !fdt)
		واپس SBI_ENODEV;

	rc = fdt_get_گرہ_addr_سائز(fdt, nodeoff, 0, &reg_addr, &reg_سائز);
	اگر (rc < 0 || !reg_addr || !reg_سائز)
		واپس SBI_ENODEV;

	واپس litex_uart_init(reg_addr);
}

ساکن ثابت ساخت fdt_مطابقت serial_litex_مطابقت[] = {
	{ .compatible = "litex,liteuart" },
	{ },
};

ثابت ساخت fdt_driver fdt_serial_litex = {
	.مطابقت_جدول = serial_litex_مطابقت,
	.init = serial_litex_init
};
