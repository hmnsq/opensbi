// SPDX-License-Identifier: BSD-2-Clause
/*
 * Copyright (C) 2022 Renesas Electronics Corporation
 */

#شامل <sbi_utils/fdt/fdt_helper.h>
#شامل <sbi_utils/serial/fdt_serial.h>
#شامل <sbi_utils/serial/renesas-scif.h>

ایستا صحیح serial_renesas_scif_init(ثابت تهی *fdt, صحیح nodeoff,
				    ثابت ساختار fdt_match *match)
{
	ساختار platform_uart_data uart = { 0 };
	صحیح ret;

	ret = fdt_parse_renesas_scif_گره(fdt, nodeoff, &uart);
	اگر (ret)
		بازگشت ret;

	بازگشت renesas_scif_init(uart.addr, uart.freq, uart.baud);
}

ایستا ثابت ساختار fdt_match serial_renesas_scif_match[] = {
	{ .compatible = "renesas,scif-r9a07g043" },
	{ /* sentinel */ }
};

ثابت ساختار fdt_driver fdt_serial_renesas_scif = {
	.match_table = serial_renesas_scif_match,
	.init = serial_renesas_scif_init
};
