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

ціле fdt_cache_add(const void *fdt, ціле noff, struct cache_device *dev)
{
	ціле rc;

	dev->id = noff;
	sbi_strncpy(dev->name, fdt_get_name(fdt, noff, NULL), sizeof(dev->name) - 1);
	sbi_dprintf("%s: %s\n", __func__, dev->name);

	rc = fdt_next_cache_get(fdt, noff, &dev->next);
	якщо (rc == SBI_ENOENT)
		dev->next = NULL;
	інакше якщо (rc)
		повернути rc;

	повернути cache_add(dev);
}

static ціле fdt_cache_add_generic(const void *fdt, ціле noff)
{
	struct cache_device *dev;
	ціле rc;

	dev = sbi_zalloc(sizeof(*dev));
	якщо (!dev)
		повернути SBI_ENOMEM;

	rc = fdt_cache_add(fdt, noff, dev);
	якщо (rc) {
		sbi_free(dev);
		повернути rc;
	}

	повернути 0;
}

static ціле fdt_cache_find(const void *fdt, ціле noff, struct cache_device **out_dev)
{
	struct cache_device *dev = cache_find(noff);
	ціле rc;

	якщо (!dev) {
		rc = fdt_driver_init_by_offset(fdt, noff, fdt_cache_drivers);
		якщо (rc == SBI_ENODEV)
			rc = fdt_cache_add_generic(fdt, noff);
		якщо (rc)
			повернути rc;

		dev = cache_find(noff);
		якщо (!dev)
			повернути SBI_EFAIL;
	}

	якщо (out_dev)
		*out_dev = dev;

	повернути SBI_OK;
}

ціле fdt_next_cache_get(const void *fdt, ціле noff, struct cache_device **out_dev)
{
	const fdt32_t *val;
	ціле len;

	val = fdt_getprop(fdt, noff, "next-level-cache", &len);
	якщо (!val || len < sizeof(*val))
		повернути SBI_ENOENT;

	noff = fdt_node_offset_by_phandle(fdt, fdt32_to_cpu(val[0]));
	якщо (noff < 0)
		повернути noff;

	повернути fdt_cache_find(fdt, noff, out_dev);
}
