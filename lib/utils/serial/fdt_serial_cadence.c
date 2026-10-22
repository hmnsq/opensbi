/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 StarFive Technology Co., Ltd.
 *
 * Author: Jun Liang Tan <junliang.tan@linux.starfivetech.com>
 */

#شامل <sbi_utils/fdt/fdt_helper.h>
#شامل <sbi_utils/serial/fdt_serial.h>
#شامل <sbi_utils/serial/cadence-uart.h>

ساکن صحیح serial_cadence_init(ثابت خالی *fdt, صحیح nodeoff,
			       ثابت ساخت fdt_مطابقت *مطابقت)
{
	صحیح rc;
	ساخت platform_uart_ڈیٹا uart = { 0 };

	rc = fdt_parse_uart_گرہ(fdt, nodeoff, &uart);
	اگر (rc)
		واپس rc;

	واپس cadence_uart_init(uart.addr, uart.freq, uart.baud);
}

ساکن ثابت ساخت fdt_مطابقت serial_cadence_مطابقت[] = {
	{ .compatible = "cdns,uart-r1p8", },
	{ .compatible = "cdns,uart-r1p12" },
	{ .compatible = "starfive,jh8100-uart" },
	{ },
};

ثابت ساخت fdt_driver fdt_serial_cadence = {
	.مطابقت_جدول = serial_cadence_مطابقت,
	.init = serial_cadence_init
};
