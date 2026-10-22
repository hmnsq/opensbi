/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#समावेश <sbi_utils/mpxy/fdt_mpxy.h>

/* List of FDT MPXY drivers generated at compile time */
बाह्य स्थिर काष्ठा fdt_driver *स्थिर fdt_mpxy_drivers[];

पूर्णांक fdt_mpxy_init(स्थिर व्योम *fdt)
{
	वापस fdt_driver_init_all(fdt, fdt_mpxy_drivers);
}
