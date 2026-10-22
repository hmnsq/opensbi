/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#અન્તર્ભાવ <libfdt.h>
#અન્તર્ભાવ <sbi/sbi_error.h>
#અન્તર્ભાવ <sbi_utils/fdt/fdt_helper.h>
#અન્તર્ભાવ <sbi_utils/serial/fdt_serial.h>

/* List of FDT serial drivers generated at compile time */
બાહ્ય અચલ સંરચના fdt_driver *અચલ fdt_serial_drivers[];

પૂર્ણાંક fdt_serial_init(અચલ શૂન્ય *fdt)
{
	અચલ શૂન્ય *prop;
	પૂર્ણાંક noff = -1, len, coff, rc;

	/* Find offset of node pointed to by stdout-path */
	coff = fdt_path_offset(fdt, "/chosen");
	જો (-1 < coff) {
		prop = fdt_getprop(fdt, coff, "stdout-path", &len);
		જો (prop && len) {
			અચલ અક્ષર *sep, *start = prop;

			/* The device path may be followed by ':' */
			sep = strchr(start, ':');
			જો (sep)
				noff = fdt_path_offset_namelen(fdt, prop,
							       sep - start);
			નહીંતર
				noff = fdt_path_offset(fdt, prop);
		}
	}

	/* First check DT node pointed by stdout-path */
	જો (-1 < noff) {
		rc = fdt_driver_init_by_offset(fdt, noff, fdt_serial_drivers);
		જો (rc != SBI_ENODEV)
			પાછા rc;
	}

	/* Lastly check all DT nodes */
	પાછા fdt_driver_init_one(fdt, fdt_serial_drivers);
}
