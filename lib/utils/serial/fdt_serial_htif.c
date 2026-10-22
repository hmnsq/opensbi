/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#અન્તર્ભાવ <sbi/riscv_asm.h>
#અન્તર્ભાવ <sbi/sbi_domain.h>
#અન્તર્ભાવ <sbi_utils/fdt/fdt_helper.h>
#અન્તર્ભાવ <sbi_utils/serial/fdt_serial.h>
#અન્તર્ભાવ <sbi_utils/sys/htif.h>

સ્થિર અચલ સંરચના fdt_match serial_htif_match[] = {
	{ .compatible = "ucb,htif0" },
	{ },
};

સ્થિર પૂર્ણાંક serial_htif_init(અચલ શૂન્ય *fdt, પૂર્ણાંક nodeoff,
			    અચલ સંરચના fdt_match *match)
{
	પૂર્ણાંક rc;
	bool custom = false;
	uint64_t fromhost_addr = 0, tohost_addr = 0;

	જો (!fdt_get_ગાંઠ_addr_size(fdt, nodeoff, 0, &fromhost_addr, ખાલી)) {
		custom = true;
		tohost_addr = fromhost_addr + કદ(uint64_t);
	}

	fdt_get_ગાંઠ_addr_size(fdt, nodeoff, 1, &tohost_addr, ખાલી);

	rc = sbi_domain_મૂળ_add_memrange(fromhost_addr, PAGE_SIZE, PAGE_SIZE,
					  (SBI_DOMAIN_MEMREGION_MMIO |
					   SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW));
	જો (rc)
		પાછા rc;

	પાછા htif_serial_init(custom, fromhost_addr, tohost_addr);
}

અચલ સંરચના fdt_driver fdt_serial_htif = {
	.match_table = serial_htif_match,
	.init = serial_htif_init
};
