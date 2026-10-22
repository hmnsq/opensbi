/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 StarFive Technology Co., Ltd.
 *
 * Author: Jun Liang Tan <junliang.tan@linux.starfivetech.com>
 */

#ضمن <sbi_utils/fdt/fdt_helper.h>
#ضمن <sbi_utils/serial/fdt_serial.h>
#ضمن <sbi_utils/serial/cadence-uart.h>

ساكن صحيح serial_cadence_init(ثابت فراغ *fdt, صحيح nodeoff,
			       ثابت بنية fdt_match *match)
{
	صحيح rc;
	بنية platform_uart_data uart = { 0 };

	rc = fdt_parse_uart_عقدة(fdt, nodeoff, &uart);
	إذا (rc)
		أرجع rc;

	أرجع cadence_uart_init(uart.addr, uart.freq, uart.baud);
}

ساكن ثابت بنية fdt_match serial_cadence_match[] = {
	{ .compatible = "cdns,uart-r1p8", },
	{ .compatible = "cdns,uart-r1p12" },
	{ .compatible = "starfive,jh8100-uart" },
	{ },
};

ثابت بنية fdt_driver fdt_serial_cadence = {
	.match_table = serial_cadence_match,
	.init = serial_cadence_init
};
