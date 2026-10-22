/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 StarFive Technology Co., Ltd.
 *
 * Author: Jun Liang Tan <junliang.tan@linux.starfivetech.com>
 */

#समावेश <sbi_utils/fdt/fdt_helper.h>
#समावेश <sbi_utils/serial/fdt_serial.h>
#समावेश <sbi_utils/serial/cadence-uart.h>

अटल पूर्णांक serial_cadence_init(स्थिर व्योम *fdt, पूर्णांक nodeoff,
			       स्थिर काष्ठा fdt_match *match)
{
	पूर्णांक rc;
	काष्ठा platform_uart_तथ्य uart = { 0 };

	rc = fdt_parse_uart_गाँठ(fdt, nodeoff, &uart);
	अगर (rc)
		वापस rc;

	वापस cadence_uart_init(uart.addr, uart.freq, uart.baud);
}

अटल स्थिर काष्ठा fdt_match serial_cadence_match[] = {
	{ .compatible = "cdns,uart-r1p8", },
	{ .compatible = "cdns,uart-r1p12" },
	{ .compatible = "starfive,jh8100-uart" },
	{ },
};

स्थिर काष्ठा fdt_driver fdt_serial_cadence = {
	.match_table = serial_cadence_match,
	.init = serial_cadence_init
};
