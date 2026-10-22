/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 StarFive Technology Co., Ltd.
 *
 * Author: Jun Liang Tan <junliang.tan@linux.starfivetech.com>
 */

#अन्तर्भाव <sbi_utils/fdt/fdt_helper.h>
#अन्तर्भाव <sbi_utils/serial/fdt_serial.h>
#अन्तर्भाव <sbi_utils/serial/cadence-uart.h>

स्थिर पूर्णाङ्क serial_cadence_init(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoff,
			       ध्रुव संरचना fdt_match *match)
{
	पूर्णाङ्क rc;
	संरचना platform_uart_data uart = { 0 };

	rc = fdt_parse_uart_गाँठ(fdt, nodeoff, &uart);
	यदि (rc)
		प्रत्यागम rc;

	प्रत्यागम cadence_uart_init(uart.addr, uart.freq, uart.baud);
}

स्थिर ध्रुव संरचना fdt_match serial_cadence_match[] = {
	{ .compatible = "cdns,uart-r1p8", },
	{ .compatible = "cdns,uart-r1p12" },
	{ .compatible = "starfive,jh8100-uart" },
	{ },
};

ध्रुव संरचना fdt_driver fdt_serial_cadence = {
	.match_table = serial_cadence_match,
	.init = serial_cadence_init
};
