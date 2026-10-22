/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#अन्तर्भाव <sbi_utils/mpxy/fdt_mpxy.h>

/* List of FDT MPXY drivers generated at compile time */
बाह्य स्थिरांक संरचना fdt_driver *स्थिरांक fdt_mpxy_drivers[];

पूर्णांक fdt_mpxy_init(स्थिरांक शून्य *fdt)
{
	परत fdt_driver_init_all(fdt, fdt_mpxy_drivers);
}
