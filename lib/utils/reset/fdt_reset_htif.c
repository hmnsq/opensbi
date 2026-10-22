/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#شامل <sbi_utils/fdt/fdt_driver.h>
#شامل <sbi_utils/fdt/fdt_helper.h>
#شامل <sbi_utils/sys/htif.h>

ساکن صحیح htif_reset_init(ثابت خالی *fdt, صحیح nodeoff,
			   ثابت ساخت fdt_مطابقت *مطابقت)
{
	bool custom = false;
	uint64_t fromhost_addr = 0, tohost_addr = 0;

	اگر (!fdt_get_گرہ_addr_سائز(fdt, nodeoff, 0, &fromhost_addr, تہی)) {
		custom = true;
		tohost_addr = fromhost_addr + حجم(uint64_t);
	}

	fdt_get_گرہ_addr_سائز(fdt, nodeoff, 1, &tohost_addr, تہی);

	واپس htif_system_reset_init(custom, fromhost_addr, tohost_addr);
}

ساکن ثابت ساخت fdt_مطابقت htif_reset_مطابقت[] = {
	{ .compatible = "ucb,htif0" },
	{ },
};

ثابت ساخت fdt_driver fdt_reset_htif = {
	.مطابقت_جدول = htif_reset_مطابقت,
	.init = htif_reset_init
};
