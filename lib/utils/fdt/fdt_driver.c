/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 SiFive
 */

#ಅನ್ತರ್ಭಾವ <libfdt.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_console.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_error.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/fdt/fdt_driver.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/fdt/fdt_helper.h>

ಪೂರ್ಣಾಙ್ಕ fdt_driver_init_by_offset(ಧ್ರುವ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ nodeoff,
			      ಧ್ರುವ ಸಂರಚನಾ fdt_driver *ಧ್ರುವ *drivers)
{
	ಧ್ರುವ ಸಂರಚನಾ fdt_driver *driver;
	ಧ್ರುವ ಸಂರಚನಾ fdt_match *match;
	ಪೂರ್ಣಾಙ್ಕ compat_len, prop_len, rc;
	ಧ್ರುವ ವರ್ಣ *compat_str;

	ಯದಿ (!fdt_ಗಂಟು_is_enabled(fdt, nodeoff))
		ಪ್ರತ್ಯಾಗಮ SBI_ENODEV;

	compat_str = fdt_getprop(fdt, nodeoff, "compatible", &prop_len);
	ಯದಿ (!compat_str)
		ಪ್ರತ್ಯಾಗಮ SBI_ENODEV;

	ಯಾವತ್ ((compat_len = strnlen(compat_str, prop_len) + 1) <= prop_len) {
		ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; (driver = drivers[i]); i++)
			ಪ್ರತಿ (match = driver->match_table; match->compatible; match++)
				ಯದಿ (!memcmp(match->compatible, compat_str, compat_len))
					ಗಚ್ಛ found;

		compat_str += compat_len;
		prop_len -= compat_len;
	}

	ಪ್ರತ್ಯಾಗಮ SBI_ENODEV;

found:
	ಯದಿ (driver->experimental)
		sbi_printf("WARNING: %s driver is experimental and may change\n",
			   match->compatible);

	rc = driver->init(fdt, nodeoff, match);
	ಯದಿ (rc < 0) {
		ಧ್ರುವ ವರ್ಣ *name;

		name = fdt_get_name(fdt, nodeoff, ರಿಕ್ತ);
		sbi_printf("%s: %s (%s) init failed: %d\n",
			   __func__, name, match->compatible, rc);
	}

	ಪ್ರತ್ಯಾಗಮ rc;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ fdt_driver_init_scan(ಧ್ರುವ ಶೂನ್ಯ *fdt,
				ಧ್ರುವ ಸಂರಚನಾ fdt_driver *ಧ್ರುವ *drivers,
				bool one)
{
	ಪೂರ್ಣಾಙ್ಕ nodeoff, rc;

	ಪ್ರತಿ (nodeoff = fdt_next_ಗಂಟು(fdt, -1, ರಿಕ್ತ);
	     nodeoff >= 0;
	     nodeoff = fdt_next_ಗಂಟು(fdt, nodeoff, ರಿಕ್ತ)) {
		rc = fdt_driver_init_by_offset(fdt, nodeoff, drivers);
		ಯದಿ (rc == SBI_ENODEV)
			ಅನುವರ್ತ;
		ಯದಿ (rc < 0)
			ಪ್ರತ್ಯಾಗಮ rc;
		ಯದಿ (one)
			ಪ್ರತ್ಯಾಗಮ 0;
	}

	ಪ್ರತ್ಯಾಗಮ one ? SBI_ENODEV : 0;
}

ಪೂರ್ಣಾಙ್ಕ fdt_driver_init_all(ಧ್ರುವ ಶೂನ್ಯ *fdt,
			ಧ್ರುವ ಸಂರಚನಾ fdt_driver *ಧ್ರುವ *drivers)
{
	ಪ್ರತ್ಯಾಗಮ fdt_driver_init_scan(fdt, drivers, false);
}

ಪೂರ್ಣಾಙ್ಕ fdt_driver_init_one(ಧ್ರುವ ಶೂನ್ಯ *fdt,
			ಧ್ರುವ ಸಂರಚನಾ fdt_driver *ಧ್ರುವ *drivers)
{
	ಪ್ರತ್ಯಾಗಮ fdt_driver_init_scan(fdt, drivers, true);
}
