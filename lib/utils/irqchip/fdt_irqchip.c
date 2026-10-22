/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#અન્તર્ભાવ <sbi_utils/irqchip/fdt_irqchip.h>

/* List of FDT irqchip drivers generated at compile time */
બાહ્ય અચલ સંરચના fdt_driver *અચલ fdt_irqchip_drivers[];

પૂર્ણાંક fdt_irqchip_init(શૂન્ય)
{
	પાછા fdt_driver_init_all(fdt_get_address(), fdt_irqchip_drivers);
}
