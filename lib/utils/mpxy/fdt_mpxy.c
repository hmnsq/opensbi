/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/mpxy/fdt_mpxy.h>

/* List of FDT MPXY drivers generated at compile time */
ਬਾਹਰੀ ਅਚਲ ਬਣਤਰ fdt_driver *ਅਚਲ fdt_mpxy_drivers[];

ਪੂਰਨ fdt_mpxy_init(ਅਚਲ ਖਾਲੀ *fdt)
{
	ਵਾਪਸ fdt_driver_init_all(fdt, fdt_mpxy_drivers);
}
