/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_error.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_scratch.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/fdt/fdt_helper.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/timer/fdt_timer.h>

/* List of FDT timer drivers generated at compile time */
ਬਾਹਰੀ ਅਚਲ ਬਣਤਰ fdt_driver *ਅਚਲ fdt_timer_drivers[];

ਪੂਰਨ fdt_timer_init(ਖਾਲੀ)
{
	/*
	 * Systems with Sstc might not provide any node in the FDT,
	 * so do not return a failure if no device is found.
	 */
	ਵਾਪਸ fdt_driver_init_all(fdt_get_ਪਤਾ(), fdt_timer_drivers);
}
