/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#समावेश <sbi_utils/irqchip/fdt_irqchip.h>

/* List of FDT irqchip drivers generated at compile time */
बाह्य स्थिर काष्ठा fdt_driver *स्थिर fdt_irqchip_drivers[];

पूर्णांक fdt_irqchip_init(व्योम)
{
	वापस fdt_driver_init_all(fdt_get_address(), fdt_irqchip_drivers);
}
