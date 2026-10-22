/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <libfdt.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_error.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/fdt/fdt_helper.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/serial/fdt_serial.h>

/* List of FDT serial drivers generated at compile time */
ਬਾਹਰੀ ਅਚਲ ਬਣਤਰ fdt_driver *ਅਚਲ fdt_serial_drivers[];

ਪੂਰਨ fdt_serial_init(ਅਚਲ ਖਾਲੀ *fdt)
{
	ਅਚਲ ਖਾਲੀ *prop;
	ਪੂਰਨ noff = -1, len, coff, rc;

	/* Find offset of node pointed to by stdout-path */
	coff = fdt_ਰਸਤਾ_ਵਿੱਥ(fdt, "/chosen");
	ਜੇ (-1 < coff) {
		prop = fdt_getprop(fdt, coff, "stdout-path", &len);
		ਜੇ (prop && len) {
			ਅਚਲ ਅੱਖਰ *sep, *ਸ਼ੁਰੂ = prop;

			/* The device path may be followed by ':' */
			sep = strchr(ਸ਼ੁਰੂ, ':');
			ਜੇ (sep)
				noff = fdt_ਰਸਤਾ_ਵਿੱਥ_namelen(fdt, prop,
							       sep - ਸ਼ੁਰੂ);
			ਨਹੀਂਤਾਂ
				noff = fdt_ਰਸਤਾ_ਵਿੱਥ(fdt, prop);
		}
	}

	/* First check DT node pointed by stdout-path */
	ਜੇ (-1 < noff) {
		rc = fdt_driver_init_by_ਵਿੱਥ(fdt, noff, fdt_serial_drivers);
		ਜੇ (rc != SBI_ENODEV)
			ਵਾਪਸ rc;
	}

	/* Lastly check all DT nodes */
	ਵਾਪਸ fdt_driver_init_one(fdt, fdt_serial_drivers);
}
