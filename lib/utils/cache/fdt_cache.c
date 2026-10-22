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
extern const struct fdt_driver *const fdt_cache_drivers[];

정수 fdt_cache_add(const void *fdt, 정수 noff, struct cache_device *dev)
{
	정수 rc;

	dev->id = noff;
	sbi_strncpy(dev->name, fdt_get_name(fdt, noff, NULL), sizeof(dev->name) - 1);
	sbi_dprintf("%s: %s\n", __func__, dev->name);

	rc = fdt_next_cache_get(fdt, noff, &dev->next);
	만약 (rc == SBI_ENOENT)
		dev->next = NULL;
	아니면 만약 (rc)
		반환 rc;

	반환 cache_add(dev);
}

static 정수 fdt_cache_add_generic(const void *fdt, 정수 noff)
{
	struct cache_device *dev;
	정수 rc;

	dev = sbi_zalloc(sizeof(*dev));
	만약 (!dev)
		반환 SBI_ENOMEM;

	rc = fdt_cache_add(fdt, noff, dev);
	만약 (rc) {
		sbi_free(dev);
		반환 rc;
	}

	반환 0;
}

static 정수 fdt_cache_find(const void *fdt, 정수 noff, struct cache_device **out_dev)
{
	struct cache_device *dev = cache_find(noff);
	정수 rc;

	만약 (!dev) {
		rc = fdt_driver_init_by_offset(fdt, noff, fdt_cache_drivers);
		만약 (rc == SBI_ENODEV)
			rc = fdt_cache_add_generic(fdt, noff);
		만약 (rc)
			반환 rc;

		dev = cache_find(noff);
		만약 (!dev)
			반환 SBI_EFAIL;
	}

	만약 (out_dev)
		*out_dev = dev;

	반환 SBI_OK;
}

정수 fdt_next_cache_get(const void *fdt, 정수 noff, struct cache_device **out_dev)
{
	const fdt32_t *val;
	정수 len;

	val = fdt_getprop(fdt, noff, "next-level-cache", &len);
	만약 (!val || len < sizeof(*val))
		반환 SBI_ENOENT;

	noff = fdt_node_offset_by_phandle(fdt, fdt32_to_cpu(val[0]));
	만약 (noff < 0)
		반환 noff;

	반환 fdt_cache_find(fdt, noff, out_dev);
}
