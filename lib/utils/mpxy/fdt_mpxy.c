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
ಬಾಹ್ಯ ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_driver *ಸ್ಥಿರಾಂಕ fdt_mpxy_drivers[];

ಪೂರ್ಣಾಂಕ fdt_mpxy_init(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt)
{
	ಹಿಂತಿರುಗಿ fdt_driver_init_all(fdt, fdt_mpxy_drivers);
}
