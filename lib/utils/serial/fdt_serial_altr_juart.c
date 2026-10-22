/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 ISCAS
 *
 * Authors:
 *   Icenowy Zheng <zhengxingda@iscas.ac.cn>
 */

#ଅନ୍ତର୍ଭାଵ <sbi/sbi_error.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/fdt/fdt_helper.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/serial/fdt_serial.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/serial/altr-juart.h>

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା serial_altr_juart_init(ଅଚଳ ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nodeoff,
				  ଅଚଳ ସଂରଚନା fdt_match *match)
{
	uint64_t reg_addr, reg_size;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;

	ଯଦି (nodeoff < 0 || !fdt)
		ଫେରନ୍ତୁ SBI_ENODEV;

	rc = fdt_get_ଗଣ୍ଠି_addr_size(fdt, nodeoff, 0, &reg_addr, &reg_size);
	/* Two 32-bit registers */
	ଯଦି (rc < 0 || !reg_addr || !reg_size || reg_size < 0x8)
		ଫେରନ୍ତୁ SBI_ENODEV;

	ଫେରନ୍ତୁ altr_juart_init(reg_addr);
}

ସ୍ଥିର ଅଚଳ ସଂରଚନା fdt_match serial_altr_juart_match[] = {
	{ .compatible = "altr,juart-1.0" },
	{ },
};

ଅଚଳ ସଂରଚନା fdt_driver fdt_serial_altr_juart = {
	.match_table = serial_altr_juart_match,
	.init = serial_altr_juart_init,
};
