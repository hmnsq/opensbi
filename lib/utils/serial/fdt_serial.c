/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#അന്തര്ഭാവ <libfdt.h>
#അന്തര്ഭാവ <sbi/sbi_error.h>
#അന്തര്ഭാവ <sbi_utils/fdt/fdt_helper.h>
#അന്തര്ഭാവ <sbi_utils/serial/fdt_serial.h>

/* List of FDT serial drivers generated at compile time */
ബാഹ്യ സ്ഥിരം ഘടന fdt_driver *സ്ഥിരം fdt_serial_drivers[];

പൂർണ്ണസംഖ്യ fdt_serial_init(സ്ഥിരം ശൂന്യം *fdt)
{
	സ്ഥിരം ശൂന്യം *prop;
	പൂർണ്ണസംഖ്യ noff = -1, len, coff, rc;

	/* Find offset of node pointed to by stdout-path */
	coff = fdt_path_offset(fdt, "/chosen");
	എങ്കിൽ (-1 < coff) {
		prop = fdt_getprop(fdt, coff, "stdout-path", &len);
		എങ്കിൽ (prop && len) {
			സ്ഥിരം അക്ഷരം *sep, *start = prop;

			/* The device path may be followed by ':' */
			sep = strchr(start, ':');
			എങ്കിൽ (sep)
				noff = fdt_path_offset_namelen(fdt, prop,
							       sep - start);
			അല്ലെങ്കിൽ
				noff = fdt_path_offset(fdt, prop);
		}
	}

	/* First check DT node pointed by stdout-path */
	എങ്കിൽ (-1 < noff) {
		rc = fdt_driver_init_by_offset(fdt, noff, fdt_serial_drivers);
		എങ്കിൽ (rc != SBI_ENODEV)
			തിരികെ rc;
	}

	/* Lastly check all DT nodes */
	തിരികെ fdt_driver_init_one(fdt, fdt_serial_drivers);
}
