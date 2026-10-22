/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ଅନ୍ତର୍ଭାଵ <libfdt.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_error.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/fdt/fdt_helper.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/serial/fdt_serial.h>

/* List of FDT serial drivers generated at compile time */
ବାହ୍ୟ ଅଚଳ ସଂରଚନା fdt_driver *ଅଚଳ fdt_serial_drivers[];

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_serial_init(ଅଚଳ ଶୂନ୍ୟ *fdt)
{
	ଅଚଳ ଶୂନ୍ୟ *prop;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା noff = -1, len, coff, rc;

	/* Find offset of node pointed to by stdout-path */
	coff = fdt_path_offset(fdt, "/chosen");
	ଯଦି (-1 < coff) {
		prop = fdt_getprop(fdt, coff, "stdout-path", &len);
		ଯଦି (prop && len) {
			ଅଚଳ ଅକ୍ଷର *sep, *start = prop;

			/* The device path may be followed by ':' */
			sep = strchr(start, ':');
			ଯଦି (sep)
				noff = fdt_path_offset_namelen(fdt, prop,
							       sep - start);
			ଅନ୍ୟଥା
				noff = fdt_path_offset(fdt, prop);
		}
	}

	/* First check DT node pointed by stdout-path */
	ଯଦି (-1 < noff) {
		rc = fdt_driver_init_by_offset(fdt, noff, fdt_serial_drivers);
		ଯଦି (rc != SBI_ENODEV)
			ଫେରନ୍ତୁ rc;
	}

	/* Lastly check all DT nodes */
	ଫେରନ୍ତୁ fdt_driver_init_one(fdt, fdt_serial_drivers);
}
