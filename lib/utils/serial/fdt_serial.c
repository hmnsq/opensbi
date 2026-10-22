/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#अन्तर्भाव <libfdt.h>
#अन्तर्भाव <sbi/sbi_error.h>
#अन्तर्भाव <sbi_utils/fdt/fdt_helper.h>
#अन्तर्भाव <sbi_utils/serial/fdt_serial.h>

/* List of FDT serial drivers generated at compile time */
बाह्य ध्रुव संरचना fdt_driver *ध्रुव fdt_serial_drivers[];

पूर्णाङ्क fdt_serial_init(ध्रुव शून्य *fdt)
{
	ध्रुव शून्य *prop;
	पूर्णाङ्क noff = -1, len, coff, rc;

	/* Find offset of node pointed to by stdout-path */
	coff = fdt_path_offset(fdt, "/chosen");
	यदि (-1 < coff) {
		prop = fdt_getprop(fdt, coff, "stdout-path", &len);
		यदि (prop && len) {
			ध्रुव वर्ण *sep, *start = prop;

			/* The device path may be followed by ':' */
			sep = म_अक्षर(start, ':');
			यदि (sep)
				noff = fdt_path_offset_namelen(fdt, prop,
							       sep - start);
			अन्यथा
				noff = fdt_path_offset(fdt, prop);
		}
	}

	/* First check DT node pointed by stdout-path */
	यदि (-1 < noff) {
		rc = fdt_driver_init_by_offset(fdt, noff, fdt_serial_drivers);
		यदि (rc != SBI_ENODEV)
			प्रत्यागम rc;
	}

	/* Lastly check all DT nodes */
	प्रत्यागम fdt_driver_init_one(fdt, fdt_serial_drivers);
}
