/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#અન્તર્ભાવ <sbi_utils/mpxy/fdt_mpxy.h>

/* List of FDT MPXY drivers generated at compile time */
બાહ્ય અચલ સંરચના fdt_driver *અચલ fdt_mpxy_drivers[];

પૂર્ણાંક fdt_mpxy_init(અચલ શૂન્ય *fdt)
{
	પાછા fdt_driver_init_all(fdt, fdt_mpxy_drivers);
}
