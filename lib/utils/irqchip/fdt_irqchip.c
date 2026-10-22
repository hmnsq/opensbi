/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ضمن <sbi_utils/irqchip/fdt_irqchip.h>

/* List of FDT irqchip drivers generated at compile time */
خارجي ثابت بنية fdt_driver *ثابت fdt_irqchip_drivers[];

صحيح fdt_irqchip_init(فراغ)
{
	أرجع fdt_driver_init_all(fdt_get_address(), fdt_irqchip_drivers);
}
