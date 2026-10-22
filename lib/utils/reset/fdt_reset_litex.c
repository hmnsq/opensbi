/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2026 Inochi Amaoto <inochiama@gmail.com>
 */

#অন্তর্ভাব <libfdt.h>
#অন্তর্ভাব <sbi/riscv_io.h>
#অন্তর্ভাব <sbi/sbi_error.h>
#অন্তর্ভাব <sbi/sbi_system.h>
#অন্তর্ভাব <sbi_utils/fdt/fdt_driver.h>
#অন্তর্ভাব <sbi_utils/fdt/fdt_helper.h>

#define RESET_CTRL			0x0

স্থির অস্থির u32 *litex_soc_base;

স্থির পূর্ণসংখ্যা litex_reset_check(u32 type, u32 reason)
{
	নির্বাচন (type) {
	ক্ষেত্র SBI_SRST_RESET_TYPE_COLD_REBOOT:
	ক্ষেত্র SBI_SRST_RESET_TYPE_WARM_REBOOT:
		ফেরত 255;
	}

	ফেরত 0;
}

স্থির শূন্য litex_do_reset(u32 type, u32 reason)
{
	writel_relaxed(0x1, litex_soc_base + RESET_CTRL);
}

স্থির কাঠামো sbi_system_reset_device litex_reset = {
	.name = "litex-reset",
	.system_reset_check = litex_reset_check,
	.system_reset = litex_do_reset
};

স্থির পূর্ণসংখ্যা litex_reset_init(ধ্রুবক শূন্য *fdt, পূর্ণসংখ্যা nodeoff,
			    ধ্রুবক কাঠামো fdt_match *match)
{
	uint64_t reg_addr;
	পূর্ণসংখ্যা rc;

	rc = fdt_get_গ্রন্থি_addr_আকার(fdt, nodeoff, 0, &reg_addr, ফাঁকা);
	যদি (rc < 0 || !reg_addr)
		ফেরত SBI_ENODEV;


	litex_soc_base = (অস্থির u32 *)(unsigned long)reg_addr;

	sbi_system_reset_add_device(&litex_reset);

	ফেরত 0;
}

স্থির ধ্রুবক কাঠামো fdt_match litex_reset_match[] = {
	{ .compatible = "litex,soc-controller" },
	{ },
};

ধ্রুবক কাঠামো fdt_driver fdt_reset_litex = {
	.match_table = litex_reset_match,
	.init = litex_reset_init,
};
