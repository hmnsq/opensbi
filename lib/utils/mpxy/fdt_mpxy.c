/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#അന്തര്ഭാവ <sbi_utils/mpxy/fdt_mpxy.h>

/* List of FDT MPXY drivers generated at compile time */
ബാഹ്യ സ്ഥിരം ഘടന fdt_driver *സ്ഥിരം fdt_mpxy_drivers[];

പൂർണ്ണസംഖ്യ fdt_mpxy_init(സ്ഥിരം ശൂന്യം *fdt)
{
	തിരികെ fdt_driver_init_all(fdt, fdt_mpxy_drivers);
}
