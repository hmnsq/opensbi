/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ಅನ್ತರ್ಭಾವ <sbi_utils/irqchip/fdt_irqchip.h>

/* List of FDT irqchip drivers generated at compile time */
ಬಾಹ್ಯ ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_driver *ಸ್ಥಿರಾಂಕ fdt_irqchip_drivers[];

ಪೂರ್ಣಾಂಕ fdt_irqchip_init(ಶೂನ್ಯ)
{
	ಹಿಂತಿರುಗಿ fdt_driver_init_all(fdt_get_ವಿಳಾಸ(), fdt_irqchip_drivers);
}
