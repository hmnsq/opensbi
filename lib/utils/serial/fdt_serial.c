/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ಅನ್ತರ್ಭಾವ <libfdt.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_error.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/fdt/fdt_helper.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/serial/fdt_serial.h>

/* List of FDT serial drivers generated at compile time */
ಬಾಹ್ಯ ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_driver *ಸ್ಥಿರಾಂಕ fdt_serial_drivers[];

ಪೂರ್ಣಾಂಕ fdt_serial_init(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt)
{
	ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *prop;
	ಪೂರ್ಣಾಂಕ noff = -1, len, coff, rc;

	/* Find offset of node pointed to by stdout-path */
	coff = fdt_ಮಾರ್ಗ_ಸ್ಥಾನಾಂತರ(fdt, "/chosen");
	ಆದರೆ (-1 < coff) {
		prop = fdt_getprop(fdt, coff, "stdout-path", &len);
		ಆದರೆ (prop && len) {
			ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *sep, *ಆರಂಭ = prop;

			/* The device path may be followed by ':' */
			sep = strchr(ಆರಂಭ, ':');
			ಆದರೆ (sep)
				noff = fdt_ಮಾರ್ಗ_ಸ್ಥಾನಾಂತರ_namelen(fdt, prop,
							       sep - ಆರಂಭ);
			ಇಲ್ಲದಿದ್ದರೆ
				noff = fdt_ಮಾರ್ಗ_ಸ್ಥಾನಾಂತರ(fdt, prop);
		}
	}

	/* First check DT node pointed by stdout-path */
	ಆದರೆ (-1 < noff) {
		rc = fdt_driver_init_by_ಸ್ಥಾನಾಂತರ(fdt, noff, fdt_serial_drivers);
		ಆದರೆ (rc != SBI_ENODEV)
			ಹಿಂತಿರುಗಿ rc;
	}

	/* Lastly check all DT nodes */
	ಹಿಂತಿರುಗಿ fdt_driver_init_one(fdt, fdt_serial_drivers);
}
