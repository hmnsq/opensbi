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
ಬಾಹ್ಯ ಧ್ರುವ ಸಂರಚನಾ fdt_driver *ಧ್ರುವ fdt_irqchip_drivers[];

ಪೂರ್ಣಾಙ್ಕ fdt_irqchip_init(ಶೂನ್ಯ)
{
	ಪ್ರತ್ಯಾಗಮ fdt_driver_init_all(fdt_get_address(), fdt_irqchip_drivers);
}
