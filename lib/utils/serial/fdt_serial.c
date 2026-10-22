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
extern const struct fdt_driver *const fdt_serial_drivers[];

정수 fdt_serial_init(const void *fdt)
{
	const void *prop;
	정수 noff = -1, len, coff, rc;

	/* Find offset of node pointed to by stdout-path */
	coff = fdt_path_offset(fdt, "/chosen");
	만약 (-1 < coff) {
		prop = fdt_getprop(fdt, coff, "stdout-path", &len);
		만약 (prop && len) {
			const char *sep, *start = prop;

			/* The device path may be followed by ':' */
			sep = strchr(start, ':');
			만약 (sep)
				noff = fdt_path_offset_namelen(fdt, prop,
							       sep - start);
			아니면
				noff = fdt_path_offset(fdt, prop);
		}
	}

	/* First check DT node pointed by stdout-path */
	만약 (-1 < noff) {
		rc = fdt_driver_init_by_offset(fdt, noff, fdt_serial_drivers);
		만약 (rc != SBI_ENODEV)
			반환 rc;
	}

	/* Lastly check all DT nodes */
	반환 fdt_driver_init_one(fdt, fdt_serial_drivers);
}
