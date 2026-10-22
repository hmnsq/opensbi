/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 Andes Technology Corporation
 */

#অন্তর্ভাৱ <libfdt.h>
#অন্তর্ভাৱ <sbi/riscv_io.h>
#অন্তর্ভাৱ <sbi/sbi_ecall_interface.h>
#অন্তর্ভাৱ <sbi/sbi_error.h>
#অন্তর্ভাৱ <sbi/sbi_hart.h>
#অন্তর্ভাৱ <sbi/sbi_system.h>
#অন্তর্ভাৱ <sbi_utils/fdt/fdt_driver.h>
#অন্তর্ভাৱ <sbi_utils/fdt/fdt_helper.h>
#অন্তর্ভাৱ <sbi_utils/hsm/fdt_hsm_andes_atcsmu.h>

#define ATCWDT200_WP_অঙ্ক 0x5aa5
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

স্থির চঞ্চল ৱর্ণ *wdt_addr = রিক্ত;

স্থির পূর্ণাঙ্ক ae350_system_reset_check(u32 type, u32 reason)
{
	ৱিকল্প (type) {
	পক্ষ SBI_SRST_RESET_TYPE_COLD_REBOOT:
		ঘূৰাই 1;
	পক্ষ SBI_SRST_RESET_TYPE_SHUTDOWN:
	পক্ষ SBI_SRST_RESET_TYPE_WARM_REBOOT:
	স্ৱভাৱ:
		ঘূৰাই 0;
	}
}

স্থির শূন্য ae350_system_reset(u32 type, u32 reason)
{
	sbi_for_each_hartindex(i)
		যদি (atcsmu_set_reset_vector(AE350_FLASH_BASE, i))
			গচ্ছ fail;

	/* Program WDT control register  */
	writew(ATCWDT200_WP_অঙ্ক, wdt_addr + WREN_REG);
	writel(INT_CLK_32768 | INT_EN | RST_CLK_128 | RST_EN | WDT_EN,
	       wdt_addr + CTRL_REG);

fail:
	sbi_hart_hang();
}

স্থির সংরচনা sbi_system_reset_device atcwdt200_reset = {
	.name		    = "atcwdt200",
	.system_reset_check = ae350_system_reset_check,
	.system_reset	    = ae350_system_reset,
};

স্থির পূর্ণাঙ্ক atcwdt200_reset_init(ধ্রুৱ শূন্য *fdt, পূর্ণাঙ্ক nodeoff,
				ধ্রুৱ সংরচনা fdt_match *match)
{
	uint64_t reg_addr;
	পূর্ণাঙ্ক rc;

	rc = fdt_get_গ্ৰন্থি_addr_size(fdt, nodeoff, 0, &reg_addr, রিক্ত);
	যদি (rc < 0 || !reg_addr)
		ঘূৰাই SBI_ENODEV;

	wdt_addr = (চঞ্চল ৱর্ণ *)(unsigned long)reg_addr;
	sbi_system_reset_add_device(&atcwdt200_reset);

	ঘূৰাই 0;
}

স্থির ধ্রুৱ সংরচনা fdt_match atcwdt200_reset_match[] = {
	{ .compatible = "andestech,atcwdt200" },
	{},
};

ধ্রুৱ সংরচনা fdt_driver fdt_reset_atcwdt200 = {
	.match_table = atcwdt200_reset_match,
	.init	     = atcwdt200_reset_init,
};
