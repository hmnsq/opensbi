/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ଅନ୍ତର୍ଭାଵ <sbi_utils/irqchip/fdt_irqchip.h>

/* List of FDT irqchip drivers generated at compile time */
ବାହ୍ୟ ଅଚଳ ସଂରଚନା fdt_driver *ଅଚଳ fdt_irqchip_drivers[];

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_irqchip_init(ଶୂନ୍ୟ)
{
	ଫେରନ୍ତୁ fdt_driver_init_all(fdt_get_address(), fdt_irqchip_drivers);
}
