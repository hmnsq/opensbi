/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#समावेश <sbi/sbi_error.h>
#समावेश <sbi/sbi_scratch.h>
#समावेश <sbi_utils/fdt/fdt_helper.h>
#समावेश <sbi_utils/timer/fdt_timer.h>

/* List of FDT timer drivers generated at compile time */
बाह्य स्थिर काष्ठा fdt_driver *स्थिर fdt_timer_drivers[];

पूर्णांक fdt_timer_init(व्योम)
{
	/*
	 * Systems with Sstc might not provide any node in the FDT,
	 * so do not return a failure if no device is found.
	 */
	वापस fdt_driver_init_all(fdt_get_address(), fdt_timer_drivers);
}
