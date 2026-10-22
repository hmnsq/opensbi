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

ಪೂರ್ಣಾಂಕ fdt_driver_init_by_ಸ್ಥಾನಾಂತರ(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ nodeoff,
			      ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_driver *ಸ್ಥಿರಾಂಕ *drivers)
{
	ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_driver *driver;
	ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_ಹೊಂದಿಕೆ *ಹೊಂದಿಕೆ;
	ಪೂರ್ಣಾಂಕ compat_len, prop_len, rc;
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *compat_str;

	ಆದರೆ (!fdt_ಗಂಟು_is_enabled(fdt, nodeoff))
		ಹಿಂತಿರುಗಿ SBI_ENODEV;

	compat_str = fdt_getprop(fdt, nodeoff, "compatible", &prop_len);
	ಆದರೆ (!compat_str)
		ಹಿಂತಿರುಗಿ SBI_ENODEV;

	ಆಗ ((compat_len = strnlen(compat_str, prop_len) + 1) <= prop_len) {
		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; (driver = drivers[i]); i++)
			ಗಾಗಿ (ಹೊಂದಿಕೆ = driver->ಹೊಂದಿಕೆ_ಕೋಷ್ಟಕ; ಹೊಂದಿಕೆ->compatible; ಹೊಂದಿಕೆ++)
				ಆದರೆ (!memcmp(ಹೊಂದಿಕೆ->compatible, compat_str, compat_len))
					ಹೋಗು ಸಿಕ್ಕಿತು;

		compat_str += compat_len;
		prop_len -= compat_len;
	}

	ಹಿಂತಿರುಗಿ SBI_ENODEV;

ಸಿಕ್ಕಿತು:
	ಆದರೆ (driver->experimental)
		sbi_printf("WARNING: %s driver is experimental and may change\n",
			   ಹೊಂದಿಕೆ->compatible);

	rc = driver->init(fdt, nodeoff, ಹೊಂದಿಕೆ);
	ಆದರೆ (rc < 0) {
		ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name;

		name = fdt_get_name(fdt, nodeoff, ಖಾಲಿ);
		sbi_printf("%s: %s (%s) init failed: %d\n",
			   __func__, name, ಹೊಂದಿಕೆ->compatible, rc);
	}

	ಹಿಂತಿರುಗಿ rc;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ fdt_driver_init_ಪರಿಶೀಲನೆ(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt,
				ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_driver *ಸ್ಥಿರಾಂಕ *drivers,
				bool one)
{
	ಪೂರ್ಣಾಂಕ nodeoff, rc;

	ಗಾಗಿ (nodeoff = fdt_ಮುಂದಿನ_ಗಂಟು(fdt, -1, ಖಾಲಿ);
	     nodeoff >= 0;
	     nodeoff = fdt_ಮುಂದಿನ_ಗಂಟು(fdt, nodeoff, ಖಾಲಿ)) {
		rc = fdt_driver_init_by_ಸ್ಥಾನಾಂತರ(fdt, nodeoff, drivers);
		ಆದರೆ (rc == SBI_ENODEV)
			ಮುಂದುವರಿ;
		ಆದರೆ (rc < 0)
			ಹಿಂತಿರುಗಿ rc;
		ಆದರೆ (one)
			ಹಿಂತಿರುಗಿ 0;
	}

	ಹಿಂತಿರುಗಿ one ? SBI_ENODEV : 0;
}

ಪೂರ್ಣಾಂಕ fdt_driver_init_all(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt,
			ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_driver *ಸ್ಥಿರಾಂಕ *drivers)
{
	ಹಿಂತಿರುಗಿ fdt_driver_init_ಪರಿಶೀಲನೆ(fdt, drivers, false);
}

ಪೂರ್ಣಾಂಕ fdt_driver_init_one(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt,
			ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_driver *ಸ್ಥಿರಾಂಕ *drivers)
{
	ಹಿಂತಿರುಗಿ fdt_driver_init_ಪರಿಶೀಲನೆ(fdt, drivers, true);
}
