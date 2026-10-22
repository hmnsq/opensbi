/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 StarFive Technology Co., Ltd.
 *
 * Author: Jun Liang Tan <junliang.tan@linux.starfivetech.com>
 */

#அந்தர்பாவ <sbi_utils/fdt/fdt_helper.h>
#அந்தர்பாவ <sbi_utils/serial/fdt_serial.h>
#அந்தர்பாவ <sbi_utils/serial/cadence-uart.h>

நிலையான முழு serial_cadence_init(மாறிலி வெறுமை *fdt, முழு nodeoff,
			       மாறிலி கட்டமைப்பு fdt_match *match)
{
	முழு rc;
	கட்டமைப்பு platform_uart_data uart = { 0 };

	rc = fdt_parse_uart_முனை(fdt, nodeoff, &uart);
	எனில் (rc)
		திரும்பு rc;

	திரும்பு cadence_uart_init(uart.addr, uart.freq, uart.baud);
}

நிலையான மாறிலி கட்டமைப்பு fdt_match serial_cadence_match[] = {
	{ .compatible = "cdns,uart-r1p8", },
	{ .compatible = "cdns,uart-r1p12" },
	{ .compatible = "starfive,jh8100-uart" },
	{ },
};

மாறிலி கட்டமைப்பு fdt_driver fdt_serial_cadence = {
	.match_table = serial_cadence_match,
	.init = serial_cadence_init
};
