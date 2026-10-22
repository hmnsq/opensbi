/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2026 Inochi Amaoto <inochiama@gmail.com>
 */

#ଅନ୍ତର୍ଭାଵ <libfdt.h>
#ଅନ୍ତର୍ଭାଵ <sbi/riscv_io.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_error.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_system.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/fdt/fdt_driver.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/fdt/fdt_helper.h>

#define RESET_CTRL			0x0

ସ୍ଥିର ଅସ୍ଥିର u32 *litex_soc_base;

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା litex_reset_check(u32 type, u32 reason)
{
	ବାଛ (type) {
	ସ୍ଥିତି SBI_SRST_RESET_TYPE_COLD_REBOOT:
	ସ୍ଥିତି SBI_SRST_RESET_TYPE_WARM_REBOOT:
		ଫେରନ୍ତୁ 255;
	}

	ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ଶୂନ୍ୟ litex_do_reset(u32 type, u32 reason)
{
	writel_relaxed(0x1, litex_soc_base + RESET_CTRL);
}

ସ୍ଥିର ସଂରଚନା sbi_system_reset_device litex_reset = {
	.name = "litex-reset",
	.system_reset_check = litex_reset_check,
	.system_reset = litex_do_reset
};

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା litex_reset_init(ଅଚଳ ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nodeoff,
			    ଅଚଳ ସଂରଚନା fdt_match *match)
{
	uint64_t reg_addr;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;

	rc = fdt_get_ଗଣ୍ଠି_addr_size(fdt, nodeoff, 0, &reg_addr, ଖାଲି);
	ଯଦି (rc < 0 || !reg_addr)
		ଫେରନ୍ତୁ SBI_ENODEV;


	litex_soc_base = (ଅସ୍ଥିର u32 *)(unsigned long)reg_addr;

	sbi_system_reset_add_device(&litex_reset);

	ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ଅଚଳ ସଂରଚନା fdt_match litex_reset_match[] = {
	{ .compatible = "litex,soc-controller" },
	{ },
};

ଅଚଳ ସଂରଚନା fdt_driver fdt_reset_litex = {
	.match_table = litex_reset_match,
	.init = litex_reset_init,
};
