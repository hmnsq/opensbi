/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#অন্তর্ভাৱ <sbi_utils/fdt/fdt_driver.h>
#অন্তর্ভাৱ <sbi_utils/fdt/fdt_helper.h>
#অন্তর্ভাৱ <sbi_utils/sys/htif.h>

স্থির পূর্ণাঙ্ক htif_reset_init(ধ্রুৱ শূন্য *fdt, পূর্ণাঙ্ক nodeoff,
			   ধ্রুৱ সংরচনা fdt_match *match)
{
	bool custom = false;
	uint64_t fromhost_addr = 0, tohost_addr = 0;

	যদি (!fdt_get_গ্ৰন্থি_addr_size(fdt, nodeoff, 0, &fromhost_addr, রিক্ত)) {
		custom = true;
		tohost_addr = fromhost_addr + পরিমাণ(uint64_t);
	}

	fdt_get_গ্ৰন্থি_addr_size(fdt, nodeoff, 1, &tohost_addr, রিক্ত);

	ঘূৰাই htif_system_reset_init(custom, fromhost_addr, tohost_addr);
}

স্থির ধ্রুৱ সংরচনা fdt_match htif_reset_match[] = {
	{ .compatible = "ucb,htif0" },
	{ },
};

ধ্রুৱ সংরচনা fdt_driver fdt_reset_htif = {
	.match_table = htif_reset_match,
	.init = htif_reset_init
};
