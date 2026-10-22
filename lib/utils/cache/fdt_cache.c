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
	གལ་སྲིད (rc == SBI_ENOENT)
		dev->next = NULL;
	དེ་མིན གལ་སྲིད (rc)
		ལོག rc;

	ལོག cache_add(dev);
}

static ཧྲིལ fdt_cache_add_generic(const void *fdt, ཧྲིལ noff)
{
	sa_nracanaa cache_device *dev;
	ཧྲིལ rc;

	dev = sbi_zalloc(parimaa_na(*dev));
	གལ་སྲིད (!dev)
		ལོག SBI_ENOMEM;

	rc = fdt_cache_add(fdt, noff, dev);
	གལ་སྲིད (rc) {
		sbi_free(dev);
		ལོག rc;
	}

	ལོག 0;
}

static ཧྲིལ fdt_cache_find(const void *fdt, ཧྲིལ noff, sa_nracanaa cache_device **out_dev)
{
	sa_nracanaa cache_device *dev = cache_find(noff);
	ཧྲིལ rc;

	གལ་སྲིད (!dev) {
		rc = fdt_driver_init_by_offset(fdt, noff, fdt_cache_drivers);
		གལ་སྲིད (rc == SBI_ENODEV)
			rc = fdt_cache_add_generic(fdt, noff);
		གལ་སྲིད (rc)
			ལོག rc;

		dev = cache_find(noff);
		གལ་སྲིད (!dev)
			ལོག SBI_EFAIL;
	}

	གལ་སྲིད (out_dev)
		*out_dev = dev;

	ལོག SBI_OK;
}

ཧྲིལ fdt_next_cache_get(const void *fdt, ཧྲིལ noff, sa_nracanaa cache_device **out_dev)
{
	const fdt32_t *val;
	ཧྲིལ len;

	val = fdt_getprop(fdt, noff, "next-level-cache", &len);
	གལ་སྲིད (!val || len < parimaa_na(*val))
		ལོག SBI_ENOENT;

	noff = fdt_node_offset_by_phandle(fdt, fdt32_to_cpu(val[0]));
	གལ་སྲིད (noff < 0)
		ལོག noff;

	ལོག fdt_cache_find(fdt, noff, out_dev);
}
