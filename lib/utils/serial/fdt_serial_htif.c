/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#അന്തര്ഭാവ <sbi/riscv_asm.h>
#അന്തര്ഭാവ <sbi/sbi_domain.h>
#അന്തര്ഭാവ <sbi_utils/fdt/fdt_helper.h>
#അന്തര്ഭാവ <sbi_utils/serial/fdt_serial.h>
#അന്തര്ഭാവ <sbi_utils/sys/htif.h>

സ്ഥിതം സ്ഥിരം ഘടന fdt_match serial_htif_match[] = {
	{ .compatible = "ucb,htif0" },
	{ },
};

സ്ഥിതം പൂർണ്ണസംഖ്യ serial_htif_init(സ്ഥിരം ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ nodeoff,
			    സ്ഥിരം ഘടന fdt_match *match)
{
	പൂർണ്ണസംഖ്യ rc;
	bool custom = false;
	uint64_t fromhost_addr = 0, tohost_addr = 0;

	എങ്കിൽ (!fdt_get_കണ്ണി_addr_size(fdt, nodeoff, 0, &fromhost_addr, ശൂന്യ)) {
		custom = true;
		tohost_addr = fromhost_addr + വലുപ്പം(uint64_t);
	}

	fdt_get_കണ്ണി_addr_size(fdt, nodeoff, 1, &tohost_addr, ശൂന്യ);

	rc = sbi_domain_വേര്_add_memrange(fromhost_addr, PAGE_SIZE, PAGE_SIZE,
					  (SBI_DOMAIN_MEMREGION_MMIO |
					   SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW));
	എങ്കിൽ (rc)
		തിരികെ rc;

	തിരികെ htif_serial_init(custom, fromhost_addr, tohost_addr);
}

സ്ഥിരം ഘടന fdt_driver fdt_serial_htif = {
	.match_table = serial_htif_match,
	.init = serial_htif_init
};
