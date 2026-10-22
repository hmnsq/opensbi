/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ಅನ್ತರ್ಭಾವ <sbi_utils/fdt/fdt_driver.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/fdt/fdt_helper.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/sys/htif.h>

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ htif_reset_init(ಧ್ರುವ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ nodeoff,
			   ಧ್ರುವ ಸಂರಚನಾ fdt_match *match)
{
	bool custom = false;
	uint64_t fromhost_addr = 0, tohost_addr = 0;

	ಯದಿ (!fdt_get_ಗಂಟು_addr_size(fdt, nodeoff, 0, &fromhost_addr, ರಿಕ್ತ)) {
		custom = true;
		tohost_addr = fromhost_addr + ಪರಿಮಾಣ(uint64_t);
	}

	fdt_get_ಗಂಟು_addr_size(fdt, nodeoff, 1, &tohost_addr, ರಿಕ್ತ);

	ಪ್ರತ್ಯಾಗಮ htif_system_reset_init(custom, fromhost_addr, tohost_addr);
}

ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ fdt_match htif_reset_match[] = {
	{ .compatible = "ucb,htif0" },
	{ },
};

ಧ್ರುವ ಸಂರಚನಾ fdt_driver fdt_reset_htif = {
	.match_table = htif_reset_match,
	.init = htif_reset_init
};
