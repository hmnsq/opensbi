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

ایستا صحیح serial_cadence_init(ثابت تهی *fdt, صحیح nodeoff,
			       ثابت ساختار fdt_match *match)
{
	صحیح rc;
	ساختار platform_uart_data uart = { 0 };

	rc = fdt_parse_uart_گره(fdt, nodeoff, &uart);
	اگر (rc)
		بازگشت rc;

	بازگشت cadence_uart_init(uart.addr, uart.freq, uart.baud);
}

ایستا ثابت ساختار fdt_match serial_cadence_match[] = {
	{ .compatible = "cdns,uart-r1p8", },
	{ .compatible = "cdns,uart-r1p12" },
	{ .compatible = "starfive,jh8100-uart" },
	{ },
};

ثابت ساختار fdt_driver fdt_serial_cadence = {
	.match_table = serial_cadence_match,
	.init = serial_cadence_init
};
