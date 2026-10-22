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

ایستا صحیح htif_reset_init(ثابت تهی *fdt, صحیح nodeoff,
			   ثابت ساختار fdt_match *match)
{
	bool custom = false;
	uint64_t fromhost_addr = 0, tohost_addr = 0;

	اگر (!fdt_get_گره_addr_size(fdt, nodeoff, 0, &fromhost_addr, پوچ)) {
		custom = true;
		tohost_addr = fromhost_addr + اندازه(uint64_t);
	}

	fdt_get_گره_addr_size(fdt, nodeoff, 1, &tohost_addr, پوچ);

	بازگشت htif_system_reset_init(custom, fromhost_addr, tohost_addr);
}

ایستا ثابت ساختار fdt_match htif_reset_match[] = {
	{ .compatible = "ucb,htif0" },
	{ },
};

ثابت ساختار fdt_driver fdt_reset_htif = {
	.match_table = htif_reset_match,
	.init = htif_reset_init
};
