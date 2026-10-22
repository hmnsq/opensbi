/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ಅನ್ತರ್ಭಾವ <libfdt.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_error.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/fdt/fdt_helper.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/serial/fdt_serial.h>

/* List of FDT serial drivers generated at compile time */
ಬಾಹ್ಯ ಧ್ರುವ ಸಂರಚನಾ fdt_driver *ಧ್ರುವ fdt_serial_drivers[];

ಪೂರ್ಣಾಙ್ಕ fdt_serial_init(ಧ್ರುವ ಶೂನ್ಯ *fdt)
{
	ಧ್ರುವ ಶೂನ್ಯ *prop;
	ಪೂರ್ಣಾಙ್ಕ noff = -1, len, coff, rc;

	/* Find offset of node pointed to by stdout-path */
	coff = fdt_path_offset(fdt, "/chosen");
	ಯದಿ (-1 < coff) {
		prop = fdt_getprop(fdt, coff, "stdout-path", &len);
		ಯದಿ (prop && len) {
			ಧ್ರುವ ವರ್ಣ *sep, *start = prop;

			/* The device path may be followed by ':' */
			sep = strchr(start, ':');
			ಯದಿ (sep)
				noff = fdt_path_offset_namelen(fdt, prop,
							       sep - start);
			ಅನ್ಯಥಾ
				noff = fdt_path_offset(fdt, prop);
		}
	}

	/* First check DT node pointed by stdout-path */
	ಯದಿ (-1 < noff) {
		rc = fdt_driver_init_by_offset(fdt, noff, fdt_serial_drivers);
		ಯದಿ (rc != SBI_ENODEV)
			ಪ್ರತ್ಯಾಗಮ rc;
	}

	/* Lastly check all DT nodes */
	ಪ್ರತ್ಯಾಗಮ fdt_driver_init_one(fdt, fdt_serial_drivers);
}
