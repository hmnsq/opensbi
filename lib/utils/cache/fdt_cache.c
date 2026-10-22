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
ಬಾಹ್ಯ ಧ್ರುವ ಸಂರಚನಾ fdt_driver *ಧ್ರುವ fdt_cache_drivers[];

ಪೂರ್ಣಾಙ್ಕ fdt_cache_add(ಧ್ರುವ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ noff, ಸಂರಚನಾ cache_device *dev)
{
	ಪೂರ್ಣಾಙ್ಕ rc;

	dev->id = noff;
	sbi_strncpy(dev->name, fdt_get_name(fdt, noff, ರಿಕ್ತ), ಪರಿಮಾಣ(dev->name) - 1);
	sbi_dprintf("%s: %s\n", __func__, dev->name);

	rc = fdt_next_cache_get(fdt, noff, &dev->next);
	ಯದಿ (rc == SBI_ENOENT)
		dev->next = ರಿಕ್ತ;
	ಅನ್ಯಥಾ ಯದಿ (rc)
		ಪ್ರತ್ಯಾಗಮ rc;

	ಪ್ರತ್ಯಾಗಮ cache_add(dev);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ fdt_cache_add_generic(ಧ್ರುವ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ noff)
{
	ಸಂರಚನಾ cache_device *dev;
	ಪೂರ್ಣಾಙ್ಕ rc;

	dev = sbi_zalloc(ಪರಿಮಾಣ(*dev));
	ಯದಿ (!dev)
		ಪ್ರತ್ಯಾಗಮ SBI_ENOMEM;

	rc = fdt_cache_add(fdt, noff, dev);
	ಯದಿ (rc) {
		sbi_free(dev);
		ಪ್ರತ್ಯಾಗಮ rc;
	}

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ fdt_cache_find(ಧ್ರುವ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ noff, ಸಂರಚನಾ cache_device **out_dev)
{
	ಸಂರಚನಾ cache_device *dev = cache_find(noff);
	ಪೂರ್ಣಾಙ್ಕ rc;

	ಯದಿ (!dev) {
		rc = fdt_driver_init_by_offset(fdt, noff, fdt_cache_drivers);
		ಯದಿ (rc == SBI_ENODEV)
			rc = fdt_cache_add_generic(fdt, noff);
		ಯದಿ (rc)
			ಪ್ರತ್ಯಾಗಮ rc;

		dev = cache_find(noff);
		ಯದಿ (!dev)
			ಪ್ರತ್ಯಾಗಮ SBI_EFAIL;
	}

	ಯದಿ (out_dev)
		*out_dev = dev;

	ಪ್ರತ್ಯಾಗಮ SBI_OK;
}

ಪೂರ್ಣಾಙ್ಕ fdt_next_cache_get(ಧ್ರುವ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ noff, ಸಂರಚನಾ cache_device **out_dev)
{
	ಧ್ರುವ fdt32_t *val;
	ಪೂರ್ಣಾಙ್ಕ len;

	val = fdt_getprop(fdt, noff, "next-level-cache", &len);
	ಯದಿ (!val || len < ಪರಿಮಾಣ(*val))
		ಪ್ರತ್ಯಾಗಮ SBI_ENOENT;

	noff = fdt_ಗಂಟು_offset_by_phandle(fdt, fdt32_to_cpu(val[0]));
	ಯದಿ (noff < 0)
		ಪ್ರತ್ಯಾಗಮ noff;

	ಪ್ರತ್ಯಾಗಮ fdt_cache_find(fdt, noff, out_dev);
}
