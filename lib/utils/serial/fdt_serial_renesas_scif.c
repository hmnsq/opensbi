// SPDX-License-Identifier: BSD-2-Clause
/*
 * Copyright (C) 2022 Renesas Electronics Corporation
 */

#ଅନ୍ତର୍ଭାଵ <sbi_utils/fdt/fdt_helper.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/serial/fdt_serial.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/serial/renesas-scif.h>

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା serial_renesas_scif_init(ଅଚଳ ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nodeoff,
				    ଅଚଳ ସଂରଚନା fdt_match *match)
{
	ସଂରଚନା platform_uart_data uart = { 0 };
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret;

	ret = fdt_parse_renesas_scif_ଗଣ୍ଠି(fdt, nodeoff, &uart);
	ଯଦି (ret)
		ଫେରନ୍ତୁ ret;

	ଫେରନ୍ତୁ renesas_scif_init(uart.addr, uart.freq, uart.baud);
}

ସ୍ଥିର ଅଚଳ ସଂରଚନା fdt_match serial_renesas_scif_match[] = {
	{ .compatible = "renesas,scif-r9a07g043" },
	{ /* sentinel */ }
};

ଅଚଳ ସଂରଚନା fdt_driver fdt_serial_renesas_scif = {
	.match_table = serial_renesas_scif_match,
	.init = serial_renesas_scif_init
};
