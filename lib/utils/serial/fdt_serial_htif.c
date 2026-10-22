/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ಅನ್ತರ್ಭಾವ <sbi/riscv_asm.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_domain.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/fdt/fdt_helper.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/serial/fdt_serial.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/sys/htif.h>

ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ fdt_match serial_htif_match[] = {
	{ .compatible = "ucb,htif0" },
	{ },
};

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ serial_htif_init(ಧ್ರುವ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ nodeoff,
			    ಧ್ರುವ ಸಂರಚನಾ fdt_match *match)
{
	ಪೂರ್ಣಾಙ್ಕ rc;
	bool custom = false;
	uint64_t fromhost_addr = 0, tohost_addr = 0;

	ಯದಿ (!fdt_get_ಗಂಟು_addr_size(fdt, nodeoff, 0, &fromhost_addr, ರಿಕ್ತ)) {
		custom = true;
		tohost_addr = fromhost_addr + ಪರಿಮಾಣ(uint64_t);
	}

	fdt_get_ಗಂಟು_addr_size(fdt, nodeoff, 1, &tohost_addr, ರಿಕ್ತ);

	rc = sbi_domain_ಬೇರು_add_memrange(fromhost_addr, PAGE_SIZE, PAGE_SIZE,
					  (SBI_DOMAIN_MEMREGION_MMIO |
					   SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW));
	ಯದಿ (rc)
		ಪ್ರತ್ಯಾಗಮ rc;

	ಪ್ರತ್ಯಾಗಮ htif_serial_init(custom, fromhost_addr, tohost_addr);
}

ಧ್ರುವ ಸಂರಚನಾ fdt_driver fdt_serial_htif = {
	.match_table = serial_htif_match,
	.init = serial_htif_init
};
