// SPDX-License-Identifier: BSD-2-Clause
/*
 * Copyright (C) 2022 Renesas Electronics Corporation
 */

#شامل <sbi_utils/fdt/fdt_helper.h>
#شامل <sbi_utils/serial/fdt_serial.h>
#شامل <sbi_utils/serial/renesas-scif.h>

ساکن صحیح serial_renesas_scif_init(ثابت خالی *fdt, صحیح nodeoff,
				    ثابت ساخت fdt_مطابقت *مطابقت)
{
	ساخت platform_uart_ڈیٹا uart = { 0 };
	صحیح ret;

	ret = fdt_parse_renesas_scif_گرہ(fdt, nodeoff, &uart);
	اگر (ret)
		واپس ret;

	واپس renesas_scif_init(uart.addr, uart.freq, uart.baud);
}

ساکن ثابت ساخت fdt_مطابقت serial_renesas_scif_مطابقت[] = {
	{ .compatible = "renesas,scif-r9a07g043" },
	{ /* sentinel */ }
};

ثابت ساخت fdt_driver fdt_serial_renesas_scif = {
	.مطابقت_جدول = serial_renesas_scif_مطابقت,
	.init = serial_renesas_scif_init
};
