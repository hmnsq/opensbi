/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 Andes Technology Corporation
 */

#ଅନ୍ତର୍ଭାଵ <libfdt.h>
#ଅନ୍ତର୍ଭାଵ <sbi/riscv_io.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_ecall_interface.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_error.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_hart.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_system.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/fdt/fdt_driver.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/fdt/fdt_helper.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/hsm/fdt_hsm_andes_atcsmu.h>

#define ATCWDT200_WP_ଅଙ୍କ 0x5aa5
#define WREN_REG 0x18
#define CTRL_REG 0x10
#define RST_TIME_OFF 8
#define RST_TIME_MSK (0x3 << RST_TIME_OFF)
#define RST_CLK_128 (0 << RST_TIME_OFF)
#define RST_CLK_256 (1 << RST_TIME_OFF)
#define RST_CLK_512 (2 << RST_TIME_OFF)
#define RST_CLK_1024 (3 << RST_TIME_OFF)
#define INT_TIME_OFF 4
#define INT_TIME_MSK (0xf << INT_TIME_OFF)
#define INT_CLK_64 (0 << INT_TIME_OFF)
#define INT_CLK_256 (1 << INT_TIME_OFF)
#define INT_CLK_1024 (2 << INT_TIME_OFF)
#define INT_CLK_2048 (3 << INT_TIME_OFF)
#define INT_CLK_4096 (4 << INT_TIME_OFF)
#define INT_CLK_8192 (5 << INT_TIME_OFF)
#define INT_CLK_16384 (6 << INT_TIME_OFF)
#define INT_CLK_32768 (7 << INT_TIME_OFF)
#define RST_EN (1 << 3)
#define INT_EN (1 << 2)
#define CLK_PCLK (1 << 1)
#define WDT_EN (1 << 0)

#define AE350_FLASH_BASE 0x80000000

ସ୍ଥିର ଅସ୍ଥିର ଅକ୍ଷର *wdt_addr = ଖାଲି;

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା ae350_system_reset_check(u32 type, u32 reason)
{
	ବାଛ (type) {
	ସ୍ଥିତି SBI_SRST_RESET_TYPE_COLD_REBOOT:
		ଫେରନ୍ତୁ 1;
	ସ୍ଥିତି SBI_SRST_RESET_TYPE_SHUTDOWN:
	ସ୍ଥିତି SBI_SRST_RESET_TYPE_WARM_REBOOT:
	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		ଫେରନ୍ତୁ 0;
	}
}

ସ୍ଥିର ଶୂନ୍ୟ ae350_system_reset(u32 type, u32 reason)
{
	sbi_for_each_hartindex(i)
		ଯଦି (atcsmu_set_reset_vector(AE350_FLASH_BASE, i))
			ଯାଅ fail;

	/* Program WDT control register  */
	writew(ATCWDT200_WP_ଅଙ୍କ, wdt_addr + WREN_REG);
	writel(INT_CLK_32768 | INT_EN | RST_CLK_128 | RST_EN | WDT_EN,
	       wdt_addr + CTRL_REG);

fail:
	sbi_hart_hang();
}

ସ୍ଥିର ସଂରଚନା sbi_system_reset_device atcwdt200_reset = {
	.name		    = "atcwdt200",
	.system_reset_check = ae350_system_reset_check,
	.system_reset	    = ae350_system_reset,
};

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା atcwdt200_reset_init(ଅଚଳ ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nodeoff,
				ଅଚଳ ସଂରଚନା fdt_match *match)
{
	uint64_t reg_addr;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;

	rc = fdt_get_ଗଣ୍ଠି_addr_size(fdt, nodeoff, 0, &reg_addr, ଖାଲି);
	ଯଦି (rc < 0 || !reg_addr)
		ଫେରନ୍ତୁ SBI_ENODEV;

	wdt_addr = (ଅସ୍ଥିର ଅକ୍ଷର *)(unsigned long)reg_addr;
	sbi_system_reset_add_device(&atcwdt200_reset);

	ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ଅଚଳ ସଂରଚନା fdt_match atcwdt200_reset_match[] = {
	{ .compatible = "andestech,atcwdt200" },
	{},
};

ଅଚଳ ସଂରଚନା fdt_driver fdt_reset_atcwdt200 = {
	.match_table = atcwdt200_reset_match,
	.init	     = atcwdt200_reset_init,
};
