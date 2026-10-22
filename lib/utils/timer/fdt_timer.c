/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ଅନ୍ତର୍ଭାଵ <sbi/sbi_error.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_scratch.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/fdt/fdt_helper.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/timer/fdt_timer.h>

/* List of FDT timer drivers generated at compile time */
ବାହ୍ୟ ଅଚଳ ସଂରଚନା fdt_driver *ଅଚଳ fdt_timer_drivers[];

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_timer_init(ଶୂନ୍ୟ)
{
	/*
	 * Systems with Sstc might not provide any node in the FDT,
	 * so do not return a failure if no device is found.
	 */
	ଫେରନ୍ତୁ fdt_driver_init_all(fdt_get_address(), fdt_timer_drivers);
}
