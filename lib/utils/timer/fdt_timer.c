/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ಅನ್ತರ್ಭಾವ <sbi/sbi_error.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_scratch.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/fdt/fdt_helper.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/timer/fdt_timer.h>

/* List of FDT timer drivers generated at compile time */
ಬಾಹ್ಯ ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_driver *ಸ್ಥಿರಾಂಕ fdt_timer_drivers[];

ಪೂರ್ಣಾಂಕ fdt_timer_init(ಶೂನ್ಯ)
{
	/*
	 * Systems with Sstc might not provide any node in the FDT,
	 * so do not return a failure if no device is found.
	 */
	ಹಿಂತಿರುಗಿ fdt_driver_init_all(fdt_get_ವಿಳಾಸ(), fdt_timer_drivers);
}
