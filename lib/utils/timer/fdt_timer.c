/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#شامل <sbi/sbi_error.h>
#شامل <sbi/sbi_scratch.h>
#شامل <sbi_utils/fdt/fdt_helper.h>
#شامل <sbi_utils/timer/fdt_timer.h>

/* List of FDT timer drivers generated at compile time */
خارجی ثابت ساخت fdt_driver *ثابت fdt_timer_drivers[];

صحیح fdt_timer_init(خالی)
{
	/*
	 * Systems with Sstc might not provide any node in the FDT,
	 * so do not return a failure if no device is found.
	 */
	واپس fdt_driver_init_all(fdt_get_پتہ(), fdt_timer_drivers);
}
