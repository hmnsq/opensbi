/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#अन्तर्भाव <sbi/sbi_error.h>
#अन्तर्भाव <sbi/sbi_scratch.h>
#अन्तर्भाव <sbi_utils/fdt/fdt_helper.h>
#अन्तर्भाव <sbi_utils/timer/fdt_timer.h>

/* List of FDT timer drivers generated at compile time */
बाह्य ध्रुव संरचना fdt_driver *ध्रुव fdt_timer_drivers[];

पूर्णाङ्क fdt_timer_init(शून्य)
{
	/*
	 * Systems with Sstc might not provide any node in the FDT,
	 * so do not return a failure if no device is found.
	 */
	प्रत्यागम fdt_driver_init_all(fdt_get_address(), fdt_timer_drivers);
}
