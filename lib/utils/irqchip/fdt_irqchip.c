/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#அந்தர்பாவ <sbi_utils/irqchip/fdt_irqchip.h>

/* List of FDT irqchip drivers generated at compile time */
வெளி மாறிலி கட்டமைப்பு fdt_driver *மாறிலி fdt_irqchip_drivers[];

முழு fdt_irqchip_init(வெறுமை)
{
	திரும்பு fdt_driver_init_all(fdt_get_address(), fdt_irqchip_drivers);
}
