/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#অন্তর্ভাৱ <sbi_utils/mpxy/fdt_mpxy.h>

/* List of FDT MPXY drivers generated at compile time */
বাহ্য ধ্রুৱ সংরচনা fdt_driver *ধ্রুৱ fdt_mpxy_drivers[];

পূর্ণাঙ্ক fdt_mpxy_init(ধ্রুৱ শূন্য *fdt)
{
	ঘূৰাই fdt_driver_init_all(fdt, fdt_mpxy_drivers);
}
