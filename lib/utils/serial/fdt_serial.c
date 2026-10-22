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
خارجی ثابت ساخت fdt_driver *ثابت fdt_serial_drivers[];

صحیح fdt_serial_init(ثابت خالی *fdt)
{
	ثابت خالی *prop;
	صحیح noff = -1, len, coff, rc;

	/* Find offset of node pointed to by stdout-path */
	coff = fdt_راستہ_انحراف(fdt, "/chosen");
	اگر (-1 < coff) {
		prop = fdt_getprop(fdt, coff, "stdout-path", &len);
		اگر (prop && len) {
			ثابت حرف *sep, *آغاز = prop;

			/* The device path may be followed by ':' */
			sep = strchr(آغاز, ':');
			اگر (sep)
				noff = fdt_راستہ_انحراف_namelen(fdt, prop,
							       sep - آغاز);
			ورنہ
				noff = fdt_راستہ_انحراف(fdt, prop);
		}
	}

	/* First check DT node pointed by stdout-path */
	اگر (-1 < noff) {
		rc = fdt_driver_init_by_انحراف(fdt, noff, fdt_serial_drivers);
		اگر (rc != SBI_ENODEV)
			واپس rc;
	}

	/* Lastly check all DT nodes */
	واپس fdt_driver_init_one(fdt, fdt_serial_drivers);
}
