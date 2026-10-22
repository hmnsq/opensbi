/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#অন্তর্ভাৱ <sbi/riscv_asm.h>
#অন্তর্ভাৱ <sbi/sbi_domain.h>
#অন্তর্ভাৱ <sbi_utils/fdt/fdt_helper.h>
#অন্তর্ভাৱ <sbi_utils/serial/fdt_serial.h>
#অন্তর্ভাৱ <sbi_utils/sys/htif.h>

স্থির ধ্রুৱ সংরচনা fdt_match serial_htif_match[] = {
	{ .compatible = "ucb,htif0" },
	{ },
};

স্থির পূর্ণাঙ্ক serial_htif_init(ধ্রুৱ শূন্য *fdt, পূর্ণাঙ্ক nodeoff,
			    ধ্রুৱ সংরচনা fdt_match *match)
{
	পূর্ণাঙ্ক rc;
	bool custom = false;
	uint64_t fromhost_addr = 0, tohost_addr = 0;

	যদি (!fdt_get_গ্ৰন্থি_addr_size(fdt, nodeoff, 0, &fromhost_addr, রিক্ত)) {
		custom = true;
		tohost_addr = fromhost_addr + পরিমাণ(uint64_t);
	}

	fdt_get_গ্ৰন্থি_addr_size(fdt, nodeoff, 1, &tohost_addr, রিক্ত);

	rc = sbi_domain_মূল_add_memrange(fromhost_addr, PAGE_SIZE, PAGE_SIZE,
					  (SBI_DOMAIN_MEMREGION_MMIO |
					   SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW));
	যদি (rc)
		ঘূৰাই rc;

	ঘূৰাই htif_serial_init(custom, fromhost_addr, tohost_addr);
}

ধ্রুৱ সংরচনা fdt_driver fdt_serial_htif = {
	.match_table = serial_htif_match,
	.init = serial_htif_init
};
