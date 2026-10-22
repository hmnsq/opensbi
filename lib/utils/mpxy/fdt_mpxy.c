/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#అన్తర్భావ <sbi_utils/mpxy/fdt_mpxy.h>

/* List of FDT MPXY drivers generated at compile time */
బాహ్య స్థిరరాశి నిర్మాణం fdt_driver *స్థిరరాశి fdt_mpxy_drivers[];

పూర్ణాంకం fdt_mpxy_init(స్థిరరాశి శూన్య *fdt)
{
	తిరిగి fdt_driver_init_all(fdt, fdt_mpxy_drivers);
}
