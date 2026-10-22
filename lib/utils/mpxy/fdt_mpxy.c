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
বাহ্যিক ধ্রুবক কাঠামো fdt_driver *ধ্রুবক fdt_mpxy_drivers[];

পূর্ণসংখ্যা fdt_mpxy_init(ধ্রুবক শূন্য *fdt)
{
	ফেরত fdt_driver_init_all(fdt, fdt_mpxy_drivers);
}
