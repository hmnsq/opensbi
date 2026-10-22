/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#অন্তর্ভাব <sbi/riscv_asm.h>
#অন্তর্ভাব <sbi/sbi_domain.h>
#অন্তর্ভাব <sbi_utils/fdt/fdt_helper.h>
#অন্তর্ভাব <sbi_utils/serial/fdt_serial.h>
#অন্তর্ভাব <sbi_utils/sys/htif.h>

স্থির ধ্রুবক কাঠামো fdt_match serial_htif_match[] = {
	{ .compatible = "ucb,htif0" },
	{ },
};

স্থির পূর্ণসংখ্যা serial_htif_init(ধ্রুবক শূন্য *fdt, পূর্ণসংখ্যা nodeoff,
			    ধ্রুবক কাঠামো fdt_match *match)
{
	পূর্ণসংখ্যা rc;
	bool custom = false;
	uint64_t fromhost_addr = 0, tohost_addr = 0;

	যদি (!fdt_get_গ্রন্থি_addr_আকার(fdt, nodeoff, 0, &fromhost_addr, ফাঁকা)) {
		custom = true;
		tohost_addr = fromhost_addr + আকার(uint64_t);
	}

	fdt_get_গ্রন্থি_addr_আকার(fdt, nodeoff, 1, &tohost_addr, ফাঁকা);

	rc = sbi_domain_মূল_add_memrange(fromhost_addr, PAGE_আকার, PAGE_আকার,
					  (SBI_DOMAIN_MEMREGION_MMIO |
					   SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW));
	যদি (rc)
		ফেরত rc;

	ফেরত htif_serial_init(custom, fromhost_addr, tohost_addr);
}

ধ্রুবক কাঠামো fdt_driver fdt_serial_htif = {
	.match_table = serial_htif_match,
	.init = serial_htif_init
};
