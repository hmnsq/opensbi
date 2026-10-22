/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Vijai Kumar K <vijai@behindbytes.com>
 *
 */

#ଅନ୍ତର୍ଭାଵ <sbi_utils/fdt/fdt_helper.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/serial/fdt_serial.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/serial/shakti-uart.h>

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା serial_shakti_init(ଅଚଳ ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nodeoff,
			      ଅଚଳ ସଂରଚନା fdt_match *match)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;
	ସଂରଚନା platform_uart_data uart = { 0 };

	rc = fdt_parse_shakti_uart_ଗଣ୍ଠି(fdt, nodeoff, &uart);
	ଯଦି (rc)
		ଫେରନ୍ତୁ rc;

	ଫେରନ୍ତୁ shakti_uart_init(uart.addr, uart.freq, uart.baud);
}

ସ୍ଥିର ଅଚଳ ସଂରଚନା fdt_match serial_shakti_match[] = {
	{ .compatible = "shakti,uart0" },
	{ },
};

ଅଚଳ ସଂରଚନା fdt_driver fdt_serial_shakti = {
	.match_table = serial_shakti_match,
	.init = serial_shakti_init
};
