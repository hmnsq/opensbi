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

整数 fdt_cache_add(const void *fdt, 整数 noff, struct cache_device *dev)
{
	整数 rc;

	dev->id = noff;
	sbi_strncpy(dev->name, fdt_get_name(fdt, noff, NULL), sizeof(dev->name) - 1);
	sbi_dprintf("%s: %s\n", __func__, dev->name);

	rc = fdt_next_cache_get(fdt, noff, &dev->next);
	如果 (rc == SBI_ENOENT)
		dev->next = NULL;
	否则 如果 (rc)
		返回 rc;

	返回 cache_add(dev);
}

static 整数 fdt_cache_add_generic(const void *fdt, 整数 noff)
{
	struct cache_device *dev;
	整数 rc;

	dev = sbi_zalloc(sizeof(*dev));
	如果 (!dev)
		返回 SBI_ENOMEM;

	rc = fdt_cache_add(fdt, noff, dev);
	如果 (rc) {
		sbi_free(dev);
		返回 rc;
	}

	返回 0;
}

static 整数 fdt_cache_find(const void *fdt, 整数 noff, struct cache_device **out_dev)
{
	struct cache_device *dev = cache_find(noff);
	整数 rc;

	如果 (!dev) {
		rc = fdt_driver_init_by_offset(fdt, noff, fdt_cache_drivers);
		如果 (rc == SBI_ENODEV)
			rc = fdt_cache_add_generic(fdt, noff);
		如果 (rc)
			返回 rc;

		dev = cache_find(noff);
		如果 (!dev)
			返回 SBI_EFAIL;
	}

	如果 (out_dev)
		*out_dev = dev;

	返回 SBI_OK;
}

整数 fdt_next_cache_get(const void *fdt, 整数 noff, struct cache_device **out_dev)
{
	const fdt32_t *val;
	整数 len;

	val = fdt_getprop(fdt, noff, "next-level-cache", &len);
	如果 (!val || len < sizeof(*val))
		返回 SBI_ENOENT;

	noff = fdt_node_offset_by_phandle(fdt, fdt32_to_cpu(val[0]));
	如果 (noff < 0)
		返回 noff;

	返回 fdt_cache_find(fdt, noff, out_dev);
}
