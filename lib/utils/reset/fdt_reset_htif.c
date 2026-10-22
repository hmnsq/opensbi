/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#அந்தர்பாவ <sbi_utils/fdt/fdt_driver.h>
#அந்தர்பாவ <sbi_utils/fdt/fdt_helper.h>
#அந்தர்பாவ <sbi_utils/sys/htif.h>

நிலையான முழு htif_reset_init(மாறிலி வெறுமை *fdt, முழு nodeoff,
			   மாறிலி கட்டமைப்பு fdt_match *match)
{
	bool custom = false;
	uint64_t fromhost_addr = 0, tohost_addr = 0;

	எனில் (!fdt_get_முனை_addr_size(fdt, nodeoff, 0, &fromhost_addr, வெற்று)) {
		custom = true;
		tohost_addr = fromhost_addr + அளவு(uint64_t);
	}

	fdt_get_முனை_addr_size(fdt, nodeoff, 1, &tohost_addr, வெற்று);

	திரும்பு htif_system_reset_init(custom, fromhost_addr, tohost_addr);
}

நிலையான மாறிலி கட்டமைப்பு fdt_match htif_reset_match[] = {
	{ .compatible = "ucb,htif0" },
	{ },
};

மாறிலி கட்டமைப்பு fdt_driver fdt_reset_htif = {
	.match_table = htif_reset_match,
	.init = htif_reset_init
};
