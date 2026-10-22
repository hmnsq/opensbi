/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ضمن <libfdt.h>
#ضمن <sbi/sbi_error.h>
#ضمن <sbi_utils/fdt/fdt_helper.h>
#ضمن <sbi_utils/serial/fdt_serial.h>

/* List of FDT serial drivers generated at compile time */
خارجي ثابت بنية fdt_driver *ثابت fdt_serial_drivers[];

صحيح fdt_serial_init(ثابت فراغ *fdt)
{
	ثابت فراغ *prop;
	صحيح noff = -1, len, coff, rc;

	/* Find offset of node pointed to by stdout-path */
	coff = fdt_path_offset(fdt, "/chosen");
	אם (-1 < coff) {
		prop = fdt_getprop(fdt, coff, "stdout-path", &len);
		אם (prop && len) {
			ثابت حرف *sep, *start = prop;

			/* The device path may be followed by ':' */
			sep = strchr(start, ':');
			אם (sep)
				noff = fdt_path_offset_namelen(fdt, prop,
							       sep - start);
			אחרת
				noff = fdt_path_offset(fdt, prop);
		}
	}

	/* First check DT node pointed by stdout-path */
	אם (-1 < noff) {
		rc = fdt_driver_init_by_offset(fdt, noff, fdt_serial_drivers);
		אם (rc != SBI_ENODEV)
			החזר rc;
	}

	/* Lastly check all DT nodes */
	החזר fdt_driver_init_one(fdt, fdt_serial_drivers);
}
