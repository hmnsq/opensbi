/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#شامل <libfdt.h>
#شامل <sbi/sbi_error.h>
#شامل <sbi_utils/fdt/fdt_helper.h>
#شامل <sbi_utils/serial/fdt_serial.h>

/* List of FDT serial drivers generated at compile time */
بیرونی ثابت ساختار fdt_driver *ثابت fdt_serial_drivers[];

صحیح fdt_serial_init(ثابت تهی *fdt)
{
	ثابت تهی *prop;
	صحیح noff = -1, len, coff, rc;

	/* Find offset of node pointed to by stdout-path */
	coff = fdt_path_offset(fdt, "/chosen");
	اگر (-1 < coff) {
		prop = fdt_getprop(fdt, coff, "stdout-path", &len);
		اگر (prop && len) {
			ثابت نویسه *sep, *start = prop;

			/* The device path may be followed by ':' */
			sep = strchr(start, ':');
			اگر (sep)
				noff = fdt_path_offset_namelen(fdt, prop,
							       sep - start);
			وگرنه
				noff = fdt_path_offset(fdt, prop);
		}
	}

	/* First check DT node pointed by stdout-path */
	اگر (-1 < noff) {
		rc = fdt_driver_init_by_offset(fdt, noff, fdt_serial_drivers);
		اگر (rc != SBI_ENODEV)
			بازگشت rc;
	}

	/* Lastly check all DT nodes */
	بازگشت fdt_driver_init_one(fdt, fdt_serial_drivers);
}
