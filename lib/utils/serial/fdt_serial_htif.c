/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ضمن <sbi/riscv_asm.h>
#ضمن <sbi/sbi_domain.h>
#ضمن <sbi_utils/fdt/fdt_helper.h>
#ضمن <sbi_utils/serial/fdt_serial.h>
#ضمن <sbi_utils/sys/htif.h>

ساكن ثابت بنية fdt_match serial_htif_match[] = {
	{ .compatible = "ucb,htif0" },
	{ },
};

ساكن صحيح serial_htif_init(ثابت فراغ *fdt, صحيح nodeoff,
			    ثابت بنية fdt_match *match)
{
	صحيح rc;
	bool custom = false;
	uint64_t fromhost_addr = 0, tohost_addr = 0;

	إذا (!fdt_get_تۈگۈن_addr_size(fdt, nodeoff, 0, &fromhost_addr, عدم)) {
		custom = true;
		tohost_addr = fromhost_addr + حجم(uint64_t);
	}

	fdt_get_تۈگۈن_addr_size(fdt, nodeoff, 1, &tohost_addr, عدم);

	rc = sbi_domain_يىلتىز_add_memrange(fromhost_addr, PAGE_SIZE, PAGE_SIZE,
					  (SBI_DOMAIN_MEMREGION_MMIO |
					   SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW));
	إذا (rc)
		أرجع rc;

	أرجع htif_serial_init(custom, fromhost_addr, tohost_addr);
}

ثابت بنية fdt_driver fdt_serial_htif = {
	.match_table = serial_htif_match,
	.init = serial_htif_init
};
