/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 SiFive Inc.
 */

#ಅನ್ತರ್ಭಾವ <libfdt.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_console.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_error.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_heap.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/cache/fdt_cache.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/fdt/fdt_driver.h>

/* List of FDT cache drivers generated at compile time */
ಬಾಹ್ಯ ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_driver *ಸ್ಥಿರಾಂಕ fdt_cache_drivers[];

ಪೂರ್ಣಾಂಕ fdt_cache_ಕೂಡಿಸು(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ noff, ರಚನೆ cache_device *dev)
{
	ಪೂರ್ಣಾಂಕ rc;

	dev->id = noff;
	sbi_strncpy(dev->name, fdt_get_name(fdt, noff, ಖಾಲಿ), ಗಾತ್ರ(dev->name) - 1);
	sbi_dprintf("%s: %s\n", __func__, dev->name);

	rc = fdt_ಮುಂದಿನ_cache_get(fdt, noff, &dev->ಮುಂದಿನ);
	ಆದರೆ (rc == SBI_ENOENT)
		dev->ಮುಂದಿನ = ಖಾಲಿ;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (rc)
		ಹಿಂತಿರುಗಿ rc;

	ಹಿಂತಿರುಗಿ cache_ಕೂಡಿಸು(dev);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ fdt_cache_ಕೂಡಿಸು_generic(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ noff)
{
	ರಚನೆ cache_device *dev;
	ಪೂರ್ಣಾಂಕ rc;

	dev = sbi_zalloc(ಗಾತ್ರ(*dev));
	ಆದರೆ (!dev)
		ಹಿಂತಿರುಗಿ SBI_ENOMEM;

	rc = fdt_cache_ಕೂಡಿಸು(fdt, noff, dev);
	ಆದರೆ (rc) {
		sbi_free(dev);
		ಹಿಂತಿರುಗಿ rc;
	}

	ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ fdt_cache_ಕಂಡುಹಿಡಿ(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ noff, ರಚನೆ cache_device **ಹೊರಹೋಗು_dev)
{
	ರಚನೆ cache_device *dev = cache_ಕಂಡುಹಿಡಿ(noff);
	ಪೂರ್ಣಾಂಕ rc;

	ಆದರೆ (!dev) {
		rc = fdt_driver_init_by_ಸ್ಥಾನಾಂತರ(fdt, noff, fdt_cache_drivers);
		ಆದರೆ (rc == SBI_ENODEV)
			rc = fdt_cache_ಕೂಡಿಸು_generic(fdt, noff);
		ಆದರೆ (rc)
			ಹಿಂತಿರುಗಿ rc;

		dev = cache_ಕಂಡುಹಿಡಿ(noff);
		ಆದರೆ (!dev)
			ಹಿಂತಿರುಗಿ SBI_EFAIL;
	}

	ಆದರೆ (ಹೊರಹೋಗು_dev)
		*ಹೊರಹೋಗು_dev = dev;

	ಹಿಂತಿರುಗಿ SBI_OK;
}

ಪೂರ್ಣಾಂಕ fdt_ಮುಂದಿನ_cache_get(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ noff, ರಚನೆ cache_device **ಹೊರಹೋಗು_dev)
{
	ಸ್ಥಿರಾಂಕ fdt32_t *val;
	ಪೂರ್ಣಾಂಕ len;

	val = fdt_getprop(fdt, noff, "next-level-cache", &len);
	ಆದರೆ (!val || len < ಗಾತ್ರ(*val))
		ಹಿಂತಿರುಗಿ SBI_ENOENT;

	noff = fdt_ಗಂಟು_ಸ್ಥಾನಾಂತರ_by_phandle(fdt, fdt32_ಗಮ್ಯ_cpu(val[0]));
	ಆದರೆ (noff < 0)
		ಹಿಂತಿರುಗಿ noff;

	ಹಿಂತಿರುಗಿ fdt_cache_ಕಂಡುಹಿಡಿ(fdt, noff, ಹೊರಹೋಗು_dev);
}
