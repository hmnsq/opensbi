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

स्थिर पूर्णाङ्क serial_cadence_init(अचर शून्य *fdt, पूर्णाङ्क nodeoff,
			       अचर संरचना fdt_match *match)
{
	पूर्णाङ्क rc;
	संरचना platform_uart_data uart = { 0 };

	rc = fdt_parse_uart_गाँठो(fdt, nodeoff, &uart);
	यदि (rc)
		फर्काऊ rc;

	फर्काऊ cadence_uart_init(uart.addr, uart.freq, uart.baud);
}

स्थिर अचर संरचना fdt_match serial_cadence_match[] = {
	{ .compatible = "cdns,uart-r1p8", },
	{ .compatible = "cdns,uart-r1p12" },
	{ .compatible = "starfive,jh8100-uart" },
	{ },
};

अचर संरचना fdt_driver fdt_serial_cadence = {
	.match_table = serial_cadence_match,
	.init = serial_cadence_init
};
