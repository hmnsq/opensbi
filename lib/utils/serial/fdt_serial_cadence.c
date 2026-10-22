/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 StarFive Technology Co., Ltd.
 *
 * Author: Jun Liang Tan <junliang.tan@linux.starfivetech.com>
 */

#અન્તર્ભાવ <sbi_utils/fdt/fdt_helper.h>
#અન્તર્ભાવ <sbi_utils/serial/fdt_serial.h>
#અન્તર્ભાવ <sbi_utils/serial/cadence-uart.h>

સ્થિર પૂર્ણાંક serial_cadence_init(અચલ શૂન્ય *fdt, પૂર્ણાંક nodeoff,
			       અચલ સંરચના fdt_match *match)
{
	પૂર્ણાંક rc;
	સંરચના platform_uart_data uart = { 0 };

	rc = fdt_parse_uart_ગાંઠ(fdt, nodeoff, &uart);
	જો (rc)
		પાછા rc;

	પાછા cadence_uart_init(uart.addr, uart.freq, uart.baud);
}

સ્થિર અચલ સંરચના fdt_match serial_cadence_match[] = {
	{ .compatible = "cdns,uart-r1p8", },
	{ .compatible = "cdns,uart-r1p12" },
	{ .compatible = "starfive,jh8100-uart" },
	{ },
};

અચલ સંરચના fdt_driver fdt_serial_cadence = {
	.match_table = serial_cadence_match,
	.init = serial_cadence_init
};
