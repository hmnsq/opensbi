/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#அந்தர்பாவ <sbi/sbi_error.h>
#அந்தர்பாவ <sbi/sbi_scratch.h>
#அந்தர்பாவ <sbi_utils/fdt/fdt_helper.h>
#அந்தர்பாவ <sbi_utils/timer/fdt_timer.h>

/* List of FDT timer drivers generated at compile time */
வெளி மாறிலி கட்டமைப்பு fdt_driver *மாறிலி fdt_timer_drivers[];

முழு fdt_timer_init(வெறுமை)
{
	/*
	 * Systems with Sstc might not provide any node in the FDT,
	 * so do not return a failure if no device is found.
	 */
	திரும்பு fdt_driver_init_all(fdt_get_address(), fdt_timer_drivers);
}
