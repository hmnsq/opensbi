/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#समावेश <libfdt.h>
#समावेश <sbi/sbi_error.h>
#समावेश <sbi_utils/fdt/fdt_helper.h>
#समावेश <sbi_utils/serial/fdt_serial.h>

/* List of FDT serial drivers generated at compile time */
बाह्य स्थिर काष्ठा fdt_driver *स्थिर fdt_serial_drivers[];

पूर्णांक fdt_serial_init(स्थिर व्योम *fdt)
{
	स्थिर व्योम *prop;
	पूर्णांक noff = -1, len, coff, rc;

	/* Find offset of node pointed to by stdout-path */
	coff = fdt_path_offset(fdt, "/chosen");
	अगर (-1 < coff) {
		prop = fdt_getprop(fdt, coff, "stdout-path", &len);
		अगर (prop && len) {
			स्थिर अक्षर *sep, *start = prop;

			/* The device path may be followed by ':' */
			sep = म_अक्षर(start, ':');
			अगर (sep)
				noff = fdt_path_offset_namelen(fdt, prop,
							       sep - start);
			अन्यथा
				noff = fdt_path_offset(fdt, prop);
		}
	}

	/* First check DT node pointed by stdout-path */
	अगर (-1 < noff) {
		rc = fdt_driver_init_by_offset(fdt, noff, fdt_serial_drivers);
		अगर (rc != SBI_ENODEV)
			वापस rc;
	}

	/* Lastly check all DT nodes */
	वापस fdt_driver_init_one(fdt, fdt_serial_drivers);
}
