/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#include <sbi_utils/mpxy/fdt_mpxy.h>

/* List of FDT MPXY drivers generated at compile time */
extern const struct fdt_driver *const fdt_mpxy_drivers[];

정수 fdt_mpxy_init(const void *fdt)
{
	반환 fdt_driver_init_all(fdt, fdt_mpxy_drivers);
}
