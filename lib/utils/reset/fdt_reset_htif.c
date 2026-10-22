/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#അന്തര്ഭാവ <sbi_utils/fdt/fdt_driver.h>
#അന്തര്ഭാവ <sbi_utils/fdt/fdt_helper.h>
#അന്തര്ഭാവ <sbi_utils/sys/htif.h>

സ്ഥിതം പൂർണ്ണസംഖ്യ htif_reset_init(സ്ഥിരം ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ nodeoff,
			   സ്ഥിരം ഘടന fdt_match *match)
{
	bool custom = false;
	uint64_t fromhost_addr = 0, tohost_addr = 0;

	എങ്കിൽ (!fdt_get_കണ്ണി_addr_size(fdt, nodeoff, 0, &fromhost_addr, ശൂന്യ)) {
		custom = true;
		tohost_addr = fromhost_addr + വലുപ്പം(uint64_t);
	}

	fdt_get_കണ്ണി_addr_size(fdt, nodeoff, 1, &tohost_addr, ശൂന്യ);

	തിരികെ htif_system_reset_init(custom, fromhost_addr, tohost_addr);
}

സ്ഥിതം സ്ഥിരം ഘടന fdt_match htif_reset_match[] = {
	{ .compatible = "ucb,htif0" },
	{ },
};

സ്ഥിരം ഘടന fdt_driver fdt_reset_htif = {
	.match_table = htif_reset_match,
	.init = htif_reset_init
};
