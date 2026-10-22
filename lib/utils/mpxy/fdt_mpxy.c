/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#ଅନ୍ତର୍ଭାଵ <sbi_utils/mpxy/fdt_mpxy.h>

/* List of FDT MPXY drivers generated at compile time */
ବାହ୍ୟ ଅଚଳ ସଂରଚନା fdt_driver *ଅଚଳ fdt_mpxy_drivers[];

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_mpxy_init(ଅଚଳ ଶୂନ୍ୟ *fdt)
{
	ଫେରନ୍ତୁ fdt_driver_init_all(fdt, fdt_mpxy_drivers);
}
