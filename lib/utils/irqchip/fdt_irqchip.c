/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#അന്തര്ഭാവ <sbi_utils/irqchip/fdt_irqchip.h>

/* List of FDT irqchip drivers generated at compile time */
ബാഹ്യ സ്ഥിരം ഘടന fdt_driver *സ്ഥിരം fdt_irqchip_drivers[];

പൂർണ്ണസംഖ്യ fdt_irqchip_init(ശൂന്യം)
{
	തിരികെ fdt_driver_init_all(fdt_get_address(), fdt_irqchip_drivers);
}
