/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#ضمن <sbi_utils/mpxy/fdt_mpxy.h>

/* List of FDT MPXY drivers generated at compile time */
خارجي ثابت بنية fdt_driver *ثابت fdt_mpxy_drivers[];

صحيح fdt_mpxy_init(ثابت فراغ *fdt)
{
	أرجع fdt_driver_init_all(fdt, fdt_mpxy_drivers);
}
