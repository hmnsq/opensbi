/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Gabriel Somlo
 *
 * Authors:
 *   Gabriel Somlo <gsomlo@gmail.com>
 */

#ଅନ୍ତର୍ଭାଵ <sbi/sbi_error.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/fdt/fdt_helper.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/serial/fdt_serial.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/serial/litex-uart.h>

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା serial_litex_init(ଅଚଳ ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nodeoff,
			     ଅଚଳ ସଂରଚନା fdt_match *match)
{
	uint64_t reg_addr, reg_size;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;

	ଯଦି (nodeoff < 0 || !fdt)
		ଫେରନ୍ତୁ SBI_ENODEV;

	rc = fdt_get_ଗଣ୍ଠି_addr_size(fdt, nodeoff, 0, &reg_addr, &reg_size);
	ଯଦି (rc < 0 || !reg_addr || !reg_size)
		ଫେରନ୍ତୁ SBI_ENODEV;

	ଫେରନ୍ତୁ litex_uart_init(reg_addr);
}

ସ୍ଥିର ଅଚଳ ସଂରଚନା fdt_match serial_litex_match[] = {
	{ .compatible = "litex,liteuart" },
	{ },
};

ଅଚଳ ସଂରଚନା fdt_driver fdt_serial_litex = {
	.match_table = serial_litex_match,
	.init = serial_litex_init
};
