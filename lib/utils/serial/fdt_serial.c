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
	coff = fdt_رستو_وٿي(fdt, "/chosen");
	إذا (-1 < coff) {
		prop = fdt_getprop(fdt, coff, "stdout-path", &len);
		إذا (prop && len) {
			ثابت حرف *sep, *شروعات = prop;

			/* The device path may be followed by ':' */
			sep = strchr(شروعات, ':');
			إذا (sep)
				noff = fdt_رستو_وٿي_namelen(fdt, prop,
							       sep - شروعات);
			وإلا
				noff = fdt_رستو_وٿي(fdt, prop);
		}
	}

	/* First check DT node pointed by stdout-path */
	إذا (-1 < noff) {
		rc = fdt_driver_init_by_وٿي(fdt, noff, fdt_serial_drivers);
		إذا (rc != SBI_ENODEV)
			أرجع rc;
	}

	/* Lastly check all DT nodes */
	أرجع fdt_driver_init_one(fdt, fdt_serial_drivers);
}
