/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#অন্তর্ভাব <sbi_utils/mpxy/fdt_mpxy.h>

/* List of FDT MPXY drivers generated at compile time */
বাহ্য ধ্রুব সংরচনা fdt_driver *ধ্রুব fdt_mpxy_drivers[];

পূর্ণাঙ্ক fdt_mpxy_init(ধ্রুব শূন্য *fdt)
{
	প্রত্যাগম fdt_driver_init_all(fdt, fdt_mpxy_drivers);
}
