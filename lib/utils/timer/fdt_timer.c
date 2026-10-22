/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#అన్తర్భావ <sbi/sbi_error.h>
#అన్తర్భావ <sbi/sbi_scratch.h>
#అన్తర్భావ <sbi_utils/fdt/fdt_helper.h>
#అన్తర్భావ <sbi_utils/timer/fdt_timer.h>

/* List of FDT timer drivers generated at compile time */
బాహ్య స్థిరరాశి నిర్మాణం fdt_driver *స్థిరరాశి fdt_timer_drivers[];

పూర్ణాంకం fdt_timer_init(శూన్య)
{
	/*
	 * Systems with Sstc might not provide any node in the FDT,
	 * so do not return a failure if no device is found.
	 */
	తిరిగి fdt_driver_init_all(fdt_get_చిరునామా(), fdt_timer_drivers);
}
