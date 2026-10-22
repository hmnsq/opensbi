/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#अन्तर्भाव <sbi/riscv_asm.h>
#अन्तर्भाव <sbi/sbi_domain.h>
#अन्तर्भाव <sbi_utils/fdt/fdt_helper.h>
#अन्तर्भाव <sbi_utils/serial/fdt_serial.h>
#अन्तर्भाव <sbi_utils/sys/htif.h>

स्थिर ध्रुव संरचना fdt_match serial_htif_match[] = {
	{ .compatible = "ucb,htif0" },
	{ },
};

स्थिर पूर्णाङ्क serial_htif_init(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoff,
			    ध्रुव संरचना fdt_match *match)
{
	पूर्णाङ्क rc;
	bool custom = false;
	uint64_t fromhost_addr = 0, tohost_addr = 0;

	यदि (!fdt_get_गाठ_addr_size(fdt, nodeoff, 0, &fromhost_addr, रिक्त)) {
		custom = true;
		tohost_addr = fromhost_addr + परिमाण(uint64_t);
	}

	fdt_get_गाठ_addr_size(fdt, nodeoff, 1, &tohost_addr, रिक्त);

	rc = sbi_domain_मूळ_add_memrange(fromhost_addr, PAGE_SIZE, PAGE_SIZE,
					  (SBI_DOMAIN_MEMREGION_MMIO |
					   SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW));
	यदि (rc)
		प्रत्यागम rc;

	प्रत्यागम htif_serial_init(custom, fromhost_addr, tohost_addr);
}

ध्रुव संरचना fdt_driver fdt_serial_htif = {
	.match_table = serial_htif_match,
	.init = serial_htif_init
};
