/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 SiFive Inc.
 */

#include <libfdt.h>
#include <sbi/sbi_console.h>
#include <sbi/sbi_error.h>
#include <sbi/sbi_heap.h>
#include <sbi_utils/cache/fdt_cache.h>
#include <sbi_utils/fdt/fdt_driver.h>

/* List of FDT cache drivers generated at compile time */
extern const sa_nracanaa fdt_driver *const fdt_cache_drivers[];

ཧྲིལ fdt_cache_add(const void *fdt, ཧྲིལ noff, sa_nracanaa cache_device *dev)
{
	ཧྲིལ rc;

	dev->id = noff;
	sbi_strncpy(dev->name, fdt_get_name(fdt, noff, NULL), parimaa_na(dev->name) - 1);
	sbi_dprintf("%s: %s\n", __func__, dev->name);

	rc = fdt_next_cache_get(fdt, noff, &dev->next);
	གལ་ཏེ (rc == SBI_ENOENT)
		dev->next = NULL;
	དེ་མིན གལ་ཏེ (rc)
		ཕྱིར rc;

	ཕྱིར cache_add(dev);
}

static ཧྲིལ fdt_cache_add_generic(const void *fdt, ཧྲིལ noff)
{
	sa_nracanaa cache_device *dev;
	ཧྲིལ rc;

	dev = sbi_zalloc(parimaa_na(*dev));
	གལ་ཏེ (!dev)
		ཕྱིར SBI_ENOMEM;

	rc = fdt_cache_add(fdt, noff, dev);
	གལ་ཏེ (rc) {
		sbi_free(dev);
		ཕྱིར rc;
	}

	ཕྱིར 0;
}

static ཧྲིལ fdt_cache_find(const void *fdt, ཧྲིལ noff, sa_nracanaa cache_device **out_dev)
{
	sa_nracanaa cache_device *dev = cache_find(noff);
	ཧྲིལ rc;

	གལ་ཏེ (!dev) {
		rc = fdt_driver_init_by_offset(fdt, noff, fdt_cache_drivers);
		གལ་ཏེ (rc == SBI_ENODEV)
			rc = fdt_cache_add_generic(fdt, noff);
		གལ་ཏེ (rc)
			ཕྱིར rc;

		dev = cache_find(noff);
		གལ་ཏེ (!dev)
			ཕྱིར SBI_EFAIL;
	}

	གལ་ཏེ (out_dev)
		*out_dev = dev;

	ཕྱིར SBI_OK;
}

ཧྲིལ fdt_next_cache_get(const void *fdt, ཧྲིལ noff, sa_nracanaa cache_device **out_dev)
{
	const fdt32_t *val;
	ཧྲིལ len;

	val = fdt_getprop(fdt, noff, "next-level-cache", &len);
	གལ་ཏེ (!val || len < parimaa_na(*val))
		ཕྱིར SBI_ENOENT;

	noff = fdt_node_offset_by_phandle(fdt, fdt32_to_cpu(val[0]));
	གལ་ཏེ (noff < 0)
		ཕྱིར noff;

	ཕྱིར fdt_cache_find(fdt, noff, out_dev);
}
