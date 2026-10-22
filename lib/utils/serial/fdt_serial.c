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
	coff = fdt_مسیر_جابه_جایی(fdt, "/chosen");
	إذا (-1 < coff) {
		prop = fdt_getprop(fdt, coff, "stdout-path", &len);
		إذا (prop && len) {
			ثابت حرف *sep, *شروع = prop;

			/* The device path may be followed by ':' */
			sep = strchr(شروع, ':');
			إذا (sep)
				noff = fdt_مسیر_جابه_جایی_namelen(fdt, prop,
							       sep - شروع);
			وإلا
				noff = fdt_مسیر_جابه_جایی(fdt, prop);
		}
	}

	/* First check DT node pointed by stdout-path */
	إذا (-1 < noff) {
		rc = fdt_driver_init_by_جابه_جایی(fdt, noff, fdt_serial_drivers);
		إذا (rc != SBI_ENODEV)
			أرجع rc;
	}

	/* Lastly check all DT nodes */
	أرجع fdt_driver_init_one(fdt, fdt_serial_drivers);
}
