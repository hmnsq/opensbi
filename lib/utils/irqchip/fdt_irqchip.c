/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#అన్తర్భావ <sbi_utils/irqchip/fdt_irqchip.h>

/* List of FDT irqchip drivers generated at compile time */
బాహ్య స్థిరరాశి నిర్మాణం fdt_driver *స్థిరరాశి fdt_irqchip_drivers[];

పూర్ణాంకం fdt_irqchip_init(శూన్య)
{
	తిరిగి fdt_driver_init_all(fdt_get_చిరునామా(), fdt_irqchip_drivers);
}
