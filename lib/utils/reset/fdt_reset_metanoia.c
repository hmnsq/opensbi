/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2026 Metanoia Communications Inc.
 *
 */

#ଅନ୍ତର୍ଭାଵ <sbi/riscv_io.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_platform.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_system.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/fdt/fdt_helper.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/fdt/fdt_driver.h>

#define SWRSTREQ_CTRL_REG_OFFSET 0x00
#define SWRSTREQ_REG_OFFSET 0x04

ସଂରଚନା reset_metanoia_data {
	u8 *reg_base;
};

ସ୍ଥିର ସଂରଚନା reset_metanoia_data reset_data;

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା metanoia_system_reset_check(u32 type, u32 reason)
{
	ବାଛ (type) {
	ସ୍ଥିତି SBI_SRST_RESET_TYPE_WARM_REBOOT:
	ସ୍ଥିତି SBI_SRST_RESET_TYPE_COLD_REBOOT:
		ଫେରନ୍ତୁ 1;
	ସ୍ଥିତି SBI_SRST_RESET_TYPE_SHUTDOWN:
	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		ଫେରନ୍ତୁ 0;
	}
}

ସ୍ଥିର ଶୂନ୍ୟ metanoia_system_reset(u32 type, u32 reason)
{
	writew(0x1, reset_data.reg_base + SWRSTREQ_CTRL_REG_OFFSET);
	writew(0x1, reset_data.reg_base + SWRSTREQ_REG_OFFSET);
}

ସ୍ଥିର ସଂରଚନା sbi_system_reset_device metanoia_reset = {
	.name		    = "metanoia-mt2824-reboot",
	.system_reset_check = metanoia_system_reset_check,
	.system_reset	    = metanoia_system_reset,
};

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା metanoia_reset_init(ଅଚଳ ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nodeoff,
			       ଅଚଳ ସଂରଚନା fdt_match *match)
{
	u64 reg_addr;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;

	rc = fdt_get_ଗଣ୍ଠି_addr_size(fdt, nodeoff, 0, &reg_addr, ଖାଲି);
	ଯଦି (rc < 0 || !reg_addr)
		ଫେରନ୍ତୁ SBI_ENODEV;

	reset_data.reg_base = (u8 *)(ulong)reg_addr;

	sbi_system_reset_add_device(&metanoia_reset);

	ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ଅଚଳ ସଂରଚନା fdt_match metanoia_reset_match[] = {
	{ .compatible = "metanoia,mt2824-reboot" },
	{ /* sentinel */ }
};

ଅଚଳ ସଂରଚନା fdt_driver fdt_reset_metanoia = {
	.match_table = metanoia_reset_match,
	.init	     = metanoia_reset_init,
};
