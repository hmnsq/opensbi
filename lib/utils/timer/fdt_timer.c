/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#અન્તર્ભાવ <sbi/sbi_error.h>
#અન્તર્ભાવ <sbi/sbi_scratch.h>
#અન્તર્ભાવ <sbi_utils/fdt/fdt_helper.h>
#અન્તર્ભાવ <sbi_utils/timer/fdt_timer.h>

/* List of FDT timer drivers generated at compile time */
બાહ્ય અચલ સંરચના fdt_driver *અચલ fdt_timer_drivers[];

પૂર્ણાંક fdt_timer_init(શૂન્ય)
{
	/*
	 * Systems with Sstc might not provide any node in the FDT,
	 * so do not return a failure if no device is found.
	 */
	પાછા fdt_driver_init_all(fdt_get_address(), fdt_timer_drivers);
}
