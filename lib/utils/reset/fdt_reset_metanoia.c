/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2026 Metanoia Communications Inc.
 *
 */

#অন্তর্ভাব <sbi/riscv_io.h>
#অন্তর্ভাব <sbi/sbi_platform.h>
#অন্তর্ভাব <sbi/sbi_system.h>
#অন্তর্ভাব <sbi_utils/fdt/fdt_helper.h>
#অন্তর্ভাব <sbi_utils/fdt/fdt_driver.h>

#define SWRSTREQ_CTRL_REG_OFFSET 0x00
#define SWRSTREQ_REG_OFFSET 0x04

কাঠামো reset_metanoia_তথ্য {
	u8 *reg_base;
};

স্থির কাঠামো reset_metanoia_তথ্য reset_তথ্য;

স্থির পূর্ণসংখ্যা metanoia_system_reset_check(u32 type, u32 reason)
{
	নির্বাচন (type) {
	ক্ষেত্র SBI_SRST_RESET_TYPE_WARM_REBOOT:
	ক্ষেত্র SBI_SRST_RESET_TYPE_COLD_REBOOT:
		ফেরত 1;
	ক্ষেত্র SBI_SRST_RESET_TYPE_SHUTDOWN:
	পূর্বনির্ধারিত:
		ফেরত 0;
	}
}

স্থির শূন্য metanoia_system_reset(u32 type, u32 reason)
{
	writew(0x1, reset_তথ্য.reg_base + SWRSTREQ_CTRL_REG_OFFSET);
	writew(0x1, reset_তথ্য.reg_base + SWRSTREQ_REG_OFFSET);
}

স্থির কাঠামো sbi_system_reset_device metanoia_reset = {
	.name		    = "metanoia-mt2824-reboot",
	.system_reset_check = metanoia_system_reset_check,
	.system_reset	    = metanoia_system_reset,
};

স্থির পূর্ণসংখ্যা metanoia_reset_init(ধ্রুবক শূন্য *fdt, পূর্ণসংখ্যা nodeoff,
			       ধ্রুবক কাঠামো fdt_match *match)
{
	u64 reg_addr;
	পূর্ণসংখ্যা rc;

	rc = fdt_get_গ্রন্থি_addr_আকার(fdt, nodeoff, 0, &reg_addr, ফাঁকা);
	যদি (rc < 0 || !reg_addr)
		ফেরত SBI_ENODEV;

	reset_তথ্য.reg_base = (u8 *)(ulong)reg_addr;

	sbi_system_reset_add_device(&metanoia_reset);

	ফেরত 0;
}

স্থির ধ্রুবক কাঠামো fdt_match metanoia_reset_match[] = {
	{ .compatible = "metanoia,mt2824-reboot" },
	{ /* sentinel */ }
};

ধ্রুবক কাঠামো fdt_driver fdt_reset_metanoia = {
	.match_table = metanoia_reset_match,
	.init	     = metanoia_reset_init,
};
