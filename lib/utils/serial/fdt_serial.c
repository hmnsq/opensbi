/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#অন্তর্ভাৱ <libfdt.h>
#অন্তর্ভাৱ <sbi/sbi_error.h>
#অন্তর্ভাৱ <sbi_utils/fdt/fdt_helper.h>
#অন্তর্ভাৱ <sbi_utils/serial/fdt_serial.h>

/* List of FDT serial drivers generated at compile time */
বাহ্য ধ্রুৱ সংরচনা fdt_driver *ধ্রুৱ fdt_serial_drivers[];

পূর্ণাঙ্ক fdt_serial_init(ধ্রুৱ শূন্য *fdt)
{
	ধ্রুৱ শূন্য *prop;
	পূর্ণাঙ্ক noff = -1, len, coff, rc;

	/* Find offset of node pointed to by stdout-path */
	coff = fdt_path_offset(fdt, "/chosen");
	যদি (-1 < coff) {
		prop = fdt_getprop(fdt, coff, "stdout-path", &len);
		যদি (prop && len) {
			ধ্রুৱ ৱর্ণ *sep, *start = prop;

			/* The device path may be followed by ':' */
			sep = strchr(start, ':');
			যদি (sep)
				noff = fdt_path_offset_namelen(fdt, prop,
							       sep - start);
			অন্যথা
				noff = fdt_path_offset(fdt, prop);
		}
	}

	/* First check DT node pointed by stdout-path */
	যদি (-1 < noff) {
		rc = fdt_driver_init_by_offset(fdt, noff, fdt_serial_drivers);
		যদি (rc != SBI_ENODEV)
			ঘূৰাই rc;
	}

	/* Lastly check all DT nodes */
	ঘূৰাই fdt_driver_init_one(fdt, fdt_serial_drivers);
}
