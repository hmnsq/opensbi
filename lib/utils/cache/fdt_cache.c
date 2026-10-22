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

целое fdt_cache_add(const void *fdt, целое noff, struct cache_device *dev)
{
	целое rc;

	dev->id = noff;
	sbi_strncpy(dev->name, fdt_get_name(fdt, noff, NULL), sizeof(dev->name) - 1);
	sbi_dprintf("%s: %s\n", __func__, dev->name);

	rc = fdt_next_cache_get(fdt, noff, &dev->next);
	если (rc == SBI_ENOENT)
		dev->next = NULL;
	иначе если (rc)
		вернуть rc;

	вернуть cache_add(dev);
}

static целое fdt_cache_add_generic(const void *fdt, целое noff)
{
	struct cache_device *dev;
	целое rc;

	dev = sbi_zalloc(sizeof(*dev));
	если (!dev)
		вернуть SBI_ENOMEM;

	rc = fdt_cache_add(fdt, noff, dev);
	если (rc) {
		sbi_free(dev);
		вернуть rc;
	}

	вернуть 0;
}

static целое fdt_cache_find(const void *fdt, целое noff, struct cache_device **out_dev)
{
	struct cache_device *dev = cache_find(noff);
	целое rc;

	если (!dev) {
		rc = fdt_driver_init_by_offset(fdt, noff, fdt_cache_drivers);
		если (rc == SBI_ENODEV)
			rc = fdt_cache_add_generic(fdt, noff);
		если (rc)
			вернуть rc;

		dev = cache_find(noff);
		если (!dev)
			вернуть SBI_EFAIL;
	}

	если (out_dev)
		*out_dev = dev;

	вернуть SBI_OK;
}

целое fdt_next_cache_get(const void *fdt, целое noff, struct cache_device **out_dev)
{
	const fdt32_t *val;
	целое len;

	val = fdt_getprop(fdt, noff, "next-level-cache", &len);
	если (!val || len < sizeof(*val))
		вернуть SBI_ENOENT;

	noff = fdt_node_offset_by_phandle(fdt, fdt32_to_cpu(val[0]));
	если (noff < 0)
		вернуть noff;

	вернуть fdt_cache_find(fdt, noff, out_dev);
}
