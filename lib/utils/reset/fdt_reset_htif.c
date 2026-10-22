/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#অন্তর্ভাব <sbi_utils/fdt/fdt_driver.h>
#অন্তর্ভাব <sbi_utils/fdt/fdt_helper.h>
#অন্তর্ভাব <sbi_utils/sys/htif.h>

স্থির পূর্ণসংখ্যা htif_reset_init(ধ্রুবক শূন্য *fdt, পূর্ণসংখ্যা nodeoff,
			   ধ্রুবক কাঠামো fdt_match *match)
{
	bool custom = false;
	uint64_t fromhost_addr = 0, tohost_addr = 0;

	যদি (!fdt_get_গ্রন্থি_addr_আকার(fdt, nodeoff, 0, &fromhost_addr, ফাঁকা)) {
		custom = true;
		tohost_addr = fromhost_addr + আকার(uint64_t);
	}

	fdt_get_গ্রন্থি_addr_আকার(fdt, nodeoff, 1, &tohost_addr, ফাঁকা);

	ফেরত htif_system_reset_init(custom, fromhost_addr, tohost_addr);
}

স্থির ধ্রুবক কাঠামো fdt_match htif_reset_match[] = {
	{ .compatible = "ucb,htif0" },
	{ },
};

ধ্রুবক কাঠামো fdt_driver fdt_reset_htif = {
	.match_table = htif_reset_match,
	.init = htif_reset_init
};
