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
बाह्य स्थिरांक संरचना fdt_driver *स्थिरांक fdt_serial_drivers[];

पूर्णांक fdt_serial_init(स्थिरांक शून्य *fdt)
{
	स्थिरांक शून्य *prop;
	पूर्णांक noff = -1, len, coff, rc;

	/* Find offset of node pointed to by stdout-path */
	coff = fdt_path_offset(fdt, "/chosen");
	जर (-1 < coff) {
		prop = fdt_getprop(fdt, coff, "stdout-path", &len);
		जर (prop && len) {
			स्थिरांक अक्षर *sep, *start = prop;

			/* The device path may be followed by ':' */
			sep = म_अक्षर(start, ':');
			जर (sep)
				noff = fdt_path_offset_namelen(fdt, prop,
							       sep - start);
			अन्यथा
				noff = fdt_path_offset(fdt, prop);
		}
	}

	/* First check DT node pointed by stdout-path */
	जर (-1 < noff) {
		rc = fdt_driver_init_by_offset(fdt, noff, fdt_serial_drivers);
		जर (rc != SBI_ENODEV)
			परत rc;
	}

	/* Lastly check all DT nodes */
	परत fdt_driver_init_one(fdt, fdt_serial_drivers);
}
