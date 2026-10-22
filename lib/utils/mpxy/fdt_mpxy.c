/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#ಅನ್ತರ್ಭಾವ <sbi_utils/mpxy/fdt_mpxy.h>

/* List of FDT MPXY drivers generated at compile time */
ಬಾಹ್ಯ ಧ್ರುವ ಸಂರಚನಾ fdt_driver *ಧ್ರುವ fdt_mpxy_drivers[];

ಪೂರ್ಣಾಙ್ಕ fdt_mpxy_init(ಧ್ರುವ ಶೂನ್ಯ *fdt)
{
	ಪ್ರತ್ಯಾಗಮ fdt_driver_init_all(fdt, fdt_mpxy_drivers);
}
