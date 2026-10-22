/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ଅନ୍ତର୍ଭାଵ <sbi_utils/fdt/fdt_helper.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/serial/fdt_serial.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/serial/sifive-uart.h>

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା serial_sifive_init(ଅଚଳ ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nodeoff,
			      ଅଚଳ ସଂରଚନା fdt_match *match)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;
	ସଂରଚନା platform_uart_data uart = { 0 };

	rc = fdt_parse_sifive_uart_ଗଣ୍ଠି(fdt, nodeoff, &uart);
	ଯଦି (rc)
		ଫେରନ୍ତୁ rc;

	ଫେରନ୍ତୁ sifive_uart_init(uart.addr, uart.freq, uart.baud);
}

ସ୍ଥିର ଅଚଳ ସଂରଚନା fdt_match serial_sifive_match[] = {
	{ .compatible = "sifive,fu540-c000-uart" },
	{ .compatible = "sifive,uart0" },
	{ },
};

ଅଚଳ ସଂରଚନା fdt_driver fdt_serial_sifive = {
	.match_table = serial_sifive_match,
	.init = serial_sifive_init
};
