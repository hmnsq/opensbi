/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#അന്തര്ഭാവ <sbi/sbi_error.h>
#അന്തര്ഭാവ <sbi/sbi_scratch.h>
#അന്തര്ഭാവ <sbi_utils/fdt/fdt_helper.h>
#അന്തര്ഭാവ <sbi_utils/timer/fdt_timer.h>

/* List of FDT timer drivers generated at compile time */
ബാഹ്യ സ്ഥിരം ഘടന fdt_driver *സ്ഥിരം fdt_timer_drivers[];

പൂർണ്ണസംഖ്യ fdt_timer_init(ശൂന്യം)
{
	/*
	 * Systems with Sstc might not provide any node in the FDT,
	 * so do not return a failure if no device is found.
	 */
	തിരികെ fdt_driver_init_all(fdt_get_address(), fdt_timer_drivers);
}
