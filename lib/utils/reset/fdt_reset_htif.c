/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#અન્તર્ભાવ <sbi_utils/fdt/fdt_driver.h>
#અન્તર્ભાવ <sbi_utils/fdt/fdt_helper.h>
#અન્તર્ભાવ <sbi_utils/sys/htif.h>

સ્થિર પૂર્ણાંક htif_reset_init(અચલ શૂન્ય *fdt, પૂર્ણાંક nodeoff,
			   અચલ સંરચના fdt_match *match)
{
	bool custom = false;
	uint64_t fromhost_addr = 0, tohost_addr = 0;

	જો (!fdt_get_ગાંઠ_addr_size(fdt, nodeoff, 0, &fromhost_addr, ખાલી)) {
		custom = true;
		tohost_addr = fromhost_addr + કદ(uint64_t);
	}

	fdt_get_ગાંઠ_addr_size(fdt, nodeoff, 1, &tohost_addr, ખાલી);

	પાછા htif_system_reset_init(custom, fromhost_addr, tohost_addr);
}

સ્થિર અચલ સંરચના fdt_match htif_reset_match[] = {
	{ .compatible = "ucb,htif0" },
	{ },
};

અચલ સંરચના fdt_driver fdt_reset_htif = {
	.match_table = htif_reset_match,
	.init = htif_reset_init
};
