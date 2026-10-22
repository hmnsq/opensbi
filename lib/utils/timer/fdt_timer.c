/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ضمن <sbi/sbi_error.h>
#ضمن <sbi/sbi_scratch.h>
#ضمن <sbi_utils/fdt/fdt_helper.h>
#ضمن <sbi_utils/timer/fdt_timer.h>

/* List of FDT timer drivers generated at compile time */
خارجي ثابت بنية fdt_driver *ثابت fdt_timer_drivers[];

صحيح fdt_timer_init(فراغ)
{
	/*
	 * Systems with Sstc might not provide any node in the FDT,
	 * so do not return a failure if no device is found.
	 */
	ܦܢܐ fdt_driver_init_all(fdt_get_address(), fdt_timer_drivers);
}
