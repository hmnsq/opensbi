/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ضمن <sbi_utils/fdt/fdt_driver.h>
#ضمن <sbi_utils/fdt/fdt_helper.h>
#ضمن <sbi_utils/sys/htif.h>

ساكن صحيح htif_reset_init(ثابت فراغ *fdt, صحيح nodeoff,
			   ثابت بنية fdt_match *match)
{
	bool custom = false;
	uint64_t fromhost_addr = 0, tohost_addr = 0;

	𐤀𐤌 (!fdt_get_node_addr_size(fdt, nodeoff, 0, &fromhost_addr, عدم)) {
		custom = true;
		tohost_addr = fromhost_addr + حجم(uint64_t);
	}

	fdt_get_node_addr_size(fdt, nodeoff, 1, &tohost_addr, عدم);

	أرجع htif_system_reset_init(custom, fromhost_addr, tohost_addr);
}

ساكن ثابت بنية fdt_match htif_reset_match[] = {
	{ .compatible = "ucb,htif0" },
	{ },
};

ثابت بنية fdt_driver fdt_reset_htif = {
	.match_table = htif_reset_match,
	.init = htif_reset_init
};
