/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#அந்தர்பாவ <libfdt.h>
#அந்தர்பாவ <sbi/sbi_error.h>
#அந்தர்பாவ <sbi_utils/fdt/fdt_helper.h>
#அந்தர்பாவ <sbi_utils/serial/fdt_serial.h>

/* List of FDT serial drivers generated at compile time */
வெளி மாறிலி கட்டமைப்பு fdt_driver *மாறிலி fdt_serial_drivers[];

முழு fdt_serial_init(மாறிலி வெறுமை *fdt)
{
	மாறிலி வெறுமை *prop;
	முழு noff = -1, len, coff, rc;

	/* Find offset of node pointed to by stdout-path */
	coff = fdt_path_offset(fdt, "/chosen");
	எனில் (-1 < coff) {
		prop = fdt_getprop(fdt, coff, "stdout-path", &len);
		எனில் (prop && len) {
			மாறிலி எழுத்து *sep, *start = prop;

			/* The device path may be followed by ':' */
			sep = strchr(start, ':');
			எனில் (sep)
				noff = fdt_path_offset_namelen(fdt, prop,
							       sep - start);
			இல்லையெனில்
				noff = fdt_path_offset(fdt, prop);
		}
	}

	/* First check DT node pointed by stdout-path */
	எனில் (-1 < noff) {
		rc = fdt_driver_init_by_offset(fdt, noff, fdt_serial_drivers);
		எனில் (rc != SBI_ENODEV)
			திரும்பு rc;
	}

	/* Lastly check all DT nodes */
	திரும்பு fdt_driver_init_one(fdt, fdt_serial_drivers);
}
