/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#அந்தர்பாவ <sbi/riscv_asm.h>
#அந்தர்பாவ <sbi/sbi_domain.h>
#அந்தர்பாவ <sbi_utils/fdt/fdt_helper.h>
#அந்தர்பாவ <sbi_utils/serial/fdt_serial.h>
#அந்தர்பாவ <sbi_utils/sys/htif.h>

நிலையான மாறிலி கட்டமைப்பு fdt_match serial_htif_match[] = {
	{ .compatible = "ucb,htif0" },
	{ },
};

நிலையான முழு serial_htif_init(மாறிலி வெறுமை *fdt, முழு nodeoff,
			    மாறிலி கட்டமைப்பு fdt_match *match)
{
	முழு rc;
	bool custom = false;
	uint64_t fromhost_addr = 0, tohost_addr = 0;

	எனில் (!fdt_get_முனை_addr_size(fdt, nodeoff, 0, &fromhost_addr, வெற்று)) {
		custom = true;
		tohost_addr = fromhost_addr + அளவு(uint64_t);
	}

	fdt_get_முனை_addr_size(fdt, nodeoff, 1, &tohost_addr, வெற்று);

	rc = sbi_domain_வேர்_add_memrange(fromhost_addr, PAGE_SIZE, PAGE_SIZE,
					  (SBI_DOMAIN_MEMREGION_MMIO |
					   SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW));
	எனில் (rc)
		திரும்பு rc;

	திரும்பு htif_serial_init(custom, fromhost_addr, tohost_addr);
}

மாறிலி கட்டமைப்பு fdt_driver fdt_serial_htif = {
	.match_table = serial_htif_match,
	.init = serial_htif_init
};
