/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#شامل <sbi_utils/mpxy/fdt_mpxy.h>

/* List of FDT MPXY drivers generated at compile time */
خارجی ثابت ساخت fdt_driver *ثابت fdt_mpxy_drivers[];

صحیح fdt_mpxy_init(ثابت خالی *fdt)
{
	واپس fdt_driver_init_all(fdt, fdt_mpxy_drivers);
}
