/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/irqchip/fdt_irqchip.h>

/* List of FDT irqchip drivers generated at compile time */
ਬਾਹਰੀ ਅਚਲ ਬਣਤਰ fdt_driver *ਅਚਲ fdt_irqchip_drivers[];

ਪੂਰਨ fdt_irqchip_init(ਖਾਲੀ)
{
	ਵਾਪਸ fdt_driver_init_all(fdt_get_ਪਤਾ(), fdt_irqchip_drivers);
}
