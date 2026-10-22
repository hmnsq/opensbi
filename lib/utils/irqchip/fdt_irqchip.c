/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#অন্তর্ভাৱ <sbi_utils/irqchip/fdt_irqchip.h>

/* List of FDT irqchip drivers generated at compile time */
বাহ্য ধ্রুৱ সংরচনা fdt_driver *ধ্রুৱ fdt_irqchip_drivers[];

পূর্ণাঙ্ক fdt_irqchip_init(শূন্য)
{
	ঘূৰাই fdt_driver_init_all(fdt_get_address(), fdt_irqchip_drivers);
}
