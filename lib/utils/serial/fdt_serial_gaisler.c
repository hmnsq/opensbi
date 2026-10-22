/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Cobham Gaisler AB.
 *
 * Authors:
 *   Daniel Cederman <cederman@gaisler.com>
 */

#ଅନ୍ତର୍ଭାଵ <sbi_utils/fdt/fdt_helper.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/serial/fdt_serial.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/serial/gaisler-uart.h>

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା serial_gaisler_init(ଅଚଳ ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nodeoff,
			       ଅଚଳ ସଂରଚନା fdt_match *match)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;
	ସଂରଚନା platform_uart_data uart = { 0 };

	rc = fdt_parse_gaisler_uart_ଗଣ୍ଠି(fdt, nodeoff, &uart);
	ଯଦି (rc)
		ଫେରନ୍ତୁ rc;

	ଫେରନ୍ତୁ gaisler_uart_init(uart.addr, uart.freq, uart.baud);
}

ସ୍ଥିର ଅଚଳ ସଂରଚନା fdt_match serial_gaisler_match[] = {
	{ .compatible = "gaisler,apbuart" },
	{},
};

ଅଚଳ ସଂରଚନା fdt_driver fdt_serial_gaisler = {
	.match_table = serial_gaisler_match,
	.init = serial_gaisler_init
};
