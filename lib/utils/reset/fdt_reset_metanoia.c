/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2026 Metanoia Communications Inc.
 *
 */

#অন্তর্ভাৱ <sbi/riscv_io.h>
#অন্তর্ভাৱ <sbi/sbi_platform.h>
#অন্তর্ভাৱ <sbi/sbi_system.h>
#অন্তর্ভাৱ <sbi_utils/fdt/fdt_helper.h>
#অন্তর্ভাৱ <sbi_utils/fdt/fdt_driver.h>

#define SWRSTREQ_CTRL_REG_OFFSET 0x00
#define SWRSTREQ_REG_OFFSET 0x04

সংরচনা reset_metanoia_data {
	u8 *reg_base;
};

স্থির সংরচনা reset_metanoia_data reset_data;

স্থির পূর্ণাঙ্ক metanoia_system_reset_check(u32 type, u32 reason)
{
	ৱিকল্প (type) {
	পক্ষ SBI_SRST_RESET_TYPE_WARM_REBOOT:
	পক্ষ SBI_SRST_RESET_TYPE_COLD_REBOOT:
		ঘূৰাই 1;
	পক্ষ SBI_SRST_RESET_TYPE_SHUTDOWN:
	স্ৱভাৱ:
		ঘূৰাই 0;
	}
}

স্থির শূন্য metanoia_system_reset(u32 type, u32 reason)
{
	writew(0x1, reset_data.reg_base + SWRSTREQ_CTRL_REG_OFFSET);
	writew(0x1, reset_data.reg_base + SWRSTREQ_REG_OFFSET);
}

স্থির সংরচনা sbi_system_reset_device metanoia_reset = {
	.name		    = "metanoia-mt2824-reboot",
	.system_reset_check = metanoia_system_reset_check,
	.system_reset	    = metanoia_system_reset,
};

স্থির পূর্ণাঙ্ক metanoia_reset_init(ধ্রুৱ শূন্য *fdt, পূর্ণাঙ্ক nodeoff,
			       ধ্রুৱ সংরচনা fdt_match *match)
{
	u64 reg_addr;
	পূর্ণাঙ্ক rc;

	rc = fdt_get_গ্ৰন্থি_addr_size(fdt, nodeoff, 0, &reg_addr, রিক্ত);
	যদি (rc < 0 || !reg_addr)
		ঘূৰাই SBI_ENODEV;

	reset_data.reg_base = (u8 *)(ulong)reg_addr;

	sbi_system_reset_add_device(&metanoia_reset);

	ঘূৰাই 0;
}

স্থির ধ্রুৱ সংরচনা fdt_match metanoia_reset_match[] = {
	{ .compatible = "metanoia,mt2824-reboot" },
	{ /* sentinel */ }
};

ধ্রুৱ সংরচনা fdt_driver fdt_reset_metanoia = {
	.match_table = metanoia_reset_match,
	.init	     = metanoia_reset_init,
};
