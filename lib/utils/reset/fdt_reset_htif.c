/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ଅନ୍ତର୍ଭାଵ <sbi_utils/fdt/fdt_driver.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/fdt/fdt_helper.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/sys/htif.h>

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା htif_reset_init(ଅଚଳ ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nodeoff,
			   ଅଚଳ ସଂରଚନା fdt_match *match)
{
	bool custom = false;
	uint64_t fromhost_addr = 0, tohost_addr = 0;

	ଯଦି (!fdt_get_ଗଣ୍ଠି_addr_size(fdt, nodeoff, 0, &fromhost_addr, ଖାଲି)) {
		custom = true;
		tohost_addr = fromhost_addr + ଆକାର(uint64_t);
	}

	fdt_get_ଗଣ୍ଠି_addr_size(fdt, nodeoff, 1, &tohost_addr, ଖାଲି);

	ଫେରନ୍ତୁ htif_system_reset_init(custom, fromhost_addr, tohost_addr);
}

ସ୍ଥିର ଅଚଳ ସଂରଚନା fdt_match htif_reset_match[] = {
	{ .compatible = "ucb,htif0" },
	{ },
};

ଅଚଳ ସଂରଚନା fdt_driver fdt_reset_htif = {
	.match_table = htif_reset_match,
	.init = htif_reset_init
};
