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

ساکن ثابت ساخت fdt_مطابقت serial_htif_مطابقت[] = {
	{ .compatible = "ucb,htif0" },
	{ },
};

ساکن صحیح serial_htif_init(ثابت خالی *fdt, صحیح nodeoff,
			    ثابت ساخت fdt_مطابقت *مطابقت)
{
	صحیح rc;
	bool custom = false;
	uint64_t fromhost_addr = 0, tohost_addr = 0;

	اگر (!fdt_get_گرہ_addr_سائز(fdt, nodeoff, 0, &fromhost_addr, تہی)) {
		custom = true;
		tohost_addr = fromhost_addr + حجم(uint64_t);
	}

	fdt_get_گرہ_addr_سائز(fdt, nodeoff, 1, &tohost_addr, تہی);

	rc = sbi_domain_جڑ_جوڑو_memrange(fromhost_addr, صفحہ_سائز, صفحہ_سائز,
					  (SBI_DOMAIN_MEMREGION_MMIO |
					   SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW));
	اگر (rc)
		واپس rc;

	واپس htif_serial_init(custom, fromhost_addr, tohost_addr);
}

ثابت ساخت fdt_driver fdt_serial_htif = {
	.مطابقت_جدول = serial_htif_مطابقت,
	.init = serial_htif_init
};
