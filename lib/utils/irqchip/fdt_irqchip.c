/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#অন্তর্ভাব <sbi_utils/irqchip/fdt_irqchip.h>

/* List of FDT irqchip drivers generated at compile time */
বাহ্যিক ধ্রুবক কাঠামো fdt_driver *ধ্রুবক fdt_irqchip_drivers[];

পূর্ণসংখ্যা fdt_irqchip_init(শূন্য)
{
	ফেরত fdt_driver_init_all(fdt_get_address(), fdt_irqchip_drivers);
}
