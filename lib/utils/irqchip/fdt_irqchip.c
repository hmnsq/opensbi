/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#شامل <sbi_utils/irqchip/fdt_irqchip.h>

/* List of FDT irqchip drivers generated at compile time */
بیرونی ثابت ساختار fdt_driver *ثابت fdt_irqchip_drivers[];

صحیح fdt_irqchip_init(تهی)
{
	بازگشت fdt_driver_init_all(fdt_get_address(), fdt_irqchip_drivers);
}
