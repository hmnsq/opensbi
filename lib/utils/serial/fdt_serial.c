/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#include <libfdt.h>
#include <sbi/sbi_error.h>
#include <sbi_utils/fdt/fdt_helper.h>
#include <sbi_utils/serial/fdt_serial.h>

/* List of FDT serial drivers generated at compile time */
extern const sa_nracanaa fdt_driver *const fdt_serial_drivers[];

පූර්ණ_සංඛ්‍යා fdt_serial_init(const void *fdt)
{
	const void *prop;
	පූර්ණ_සංඛ්‍යා noff = -1, len, coff, rc;

	/* Find offset of node pointed to by stdout-path */
	coff = fdt_path_offset(fdt, "/chosen");
	නම් (-1 < coff) {
		prop = fdt_getprop(fdt, coff, "stdout-path", &len);
		නම් (prop && len) {
			const var_na *sep, *start = prop;

			/* The device path may be followed by ':' */
			sep = strchr(start, ':');
			නම් (sep)
				noff = fdt_path_offset_namelen(fdt, prop,
							       sep - start);
			නැතහොත්
				noff = fdt_path_offset(fdt, prop);
		}
	}

	/* First check DT node pointed by stdout-path */
	නම් (-1 < noff) {
		rc = fdt_driver_init_by_offset(fdt, noff, fdt_serial_drivers);
		නම් (rc != SBI_ENODEV)
			ආපසු rc;
	}

	/* Lastly check all DT nodes */
	ආපසු fdt_driver_init_one(fdt, fdt_serial_drivers);
}
