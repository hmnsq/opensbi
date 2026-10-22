/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#அந்தர்பாவ <sbi_utils/mpxy/fdt_mpxy.h>

/* List of FDT MPXY drivers generated at compile time */
வெளி மாறிலி கட்டமைப்பு fdt_driver *மாறிலி fdt_mpxy_drivers[];

முழு fdt_mpxy_init(மாறிலி வெறுமை *fdt)
{
	திரும்பு fdt_driver_init_all(fdt, fdt_mpxy_drivers);
}
