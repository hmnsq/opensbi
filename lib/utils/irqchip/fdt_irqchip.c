/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#अन्तर्भाव <sbi_utils/irqchip/fdt_irqchip.h>

/* List of FDT irqchip drivers generated at compile time */
बाह्य ध्रुव संरचना fdt_driver *ध्रुव fdt_irqchip_drivers[];

पूर्णाङ्क fdt_irqchip_init(शून्य)
{
	प्रत्यागम fdt_driver_init_all(fdt_get_address(), fdt_irqchip_drivers);
}
