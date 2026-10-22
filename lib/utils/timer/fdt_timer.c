/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#অন্তর্ভাৱ <sbi/sbi_error.h>
#অন্তর্ভাৱ <sbi/sbi_scratch.h>
#অন্তর্ভাৱ <sbi_utils/fdt/fdt_helper.h>
#অন্তর্ভাৱ <sbi_utils/timer/fdt_timer.h>

/* List of FDT timer drivers generated at compile time */
বাহ্য ধ্রুৱ সংরচনা fdt_driver *ধ্রুৱ fdt_timer_drivers[];

পূর্ণাঙ্ক fdt_timer_init(শূন্য)
{
	/*
	 * Systems with Sstc might not provide any node in the FDT,
	 * so do not return a failure if no device is found.
	 */
	ঘূৰাই fdt_driver_init_all(fdt_get_address(), fdt_timer_drivers);
}
