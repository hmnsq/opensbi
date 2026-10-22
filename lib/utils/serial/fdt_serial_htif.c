/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#شامل <sbi/riscv_asm.h>
#شامل <sbi/sbi_domain.h>
#شامل <sbi_utils/fdt/fdt_helper.h>
#شامل <sbi_utils/serial/fdt_serial.h>
#شامل <sbi_utils/sys/htif.h>

ایستا ثابت ساختار fdt_match serial_htif_match[] = {
	{ .compatible = "ucb,htif0" },
	{ },
};

ایستا صحیح serial_htif_init(ثابت تهی *fdt, صحیح nodeoff,
			    ثابت ساختار fdt_match *match)
{
	صحیح rc;
	bool custom = false;
	uint64_t fromhost_addr = 0, tohost_addr = 0;

	اگر (!fdt_get_گره_addr_size(fdt, nodeoff, 0, &fromhost_addr, پوچ)) {
		custom = true;
		tohost_addr = fromhost_addr + اندازه(uint64_t);
	}

	fdt_get_گره_addr_size(fdt, nodeoff, 1, &tohost_addr, پوچ);

	rc = sbi_domain_ریشه_add_memrange(fromhost_addr, PAGE_SIZE, PAGE_SIZE,
					  (SBI_DOMAIN_MEMREGION_MMIO |
					   SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW));
	اگر (rc)
		بازگشت rc;

	بازگشت htif_serial_init(custom, fromhost_addr, tohost_addr);
}

ثابت ساختار fdt_driver fdt_serial_htif = {
	.match_table = serial_htif_match,
	.init = serial_htif_init
};
