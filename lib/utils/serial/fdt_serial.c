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

ចំនួនគត់ fdt_serial_init(const void *fdt)
{
	const void *prop;
	ចំនួនគត់ noff = -1, len, coff, rc;

	/* Find offset of node pointed to by stdout-path */
	coff = fdt_path_offset(fdt, "/chosen");
	ប្រសិនបើ (-1 < coff) {
		prop = fdt_getprop(fdt, coff, "stdout-path", &len);
		ប្រសិនបើ (prop && len) {
			const var_na *sep, *start = prop;

			/* The device path may be followed by ':' */
			sep = strchr(start, ':');
			ប្រសិនបើ (sep)
				noff = fdt_path_offset_namelen(fdt, prop,
							       sep - start);
			បើមិនដូច្នេះ
				noff = fdt_path_offset(fdt, prop);
		}
	}

	/* First check DT node pointed by stdout-path */
	ប្រសិនបើ (-1 < noff) {
		rc = fdt_driver_init_by_offset(fdt, noff, fdt_serial_drivers);
		ប្រសិនបើ (rc != SBI_ENODEV)
			ត្រឡប់ rc;
	}

	/* Lastly check all DT nodes */
	ត្រឡប់ fdt_driver_init_one(fdt, fdt_serial_drivers);
}
