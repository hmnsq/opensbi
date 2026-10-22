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
बाह्य ध्रुव संरचना fdt_driver *ध्रुव fdt_mpxy_drivers[];

पूर्णाङ्क fdt_mpxy_init(ध्रुव शून्य *fdt)
{
	प्रत्यागम fdt_driver_init_all(fdt, fdt_mpxy_drivers);
}
