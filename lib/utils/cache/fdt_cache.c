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

цяло fdt_cache_add(const void *fdt, цяло noff, struct cache_device *dev)
{
	цяло rc;

	dev->id = noff;
	sbi_strncpy(dev->name, fdt_get_name(fdt, noff, NULL), sizeof(dev->name) - 1);
	sbi_dprintf("%s: %s\n", __func__, dev->name);

	rc = fdt_next_cache_get(fdt, noff, &dev->next);
	ако (rc == SBI_ENOENT)
		dev->next = NULL;
	иначе ако (rc)
		върни rc;

	върни cache_add(dev);
}

static цяло fdt_cache_add_generic(const void *fdt, цяло noff)
{
	struct cache_device *dev;
	цяло rc;

	dev = sbi_zalloc(sizeof(*dev));
	ако (!dev)
		върни SBI_ENOMEM;

	rc = fdt_cache_add(fdt, noff, dev);
	ако (rc) {
		sbi_free(dev);
		върни rc;
	}

	върни 0;
}

static цяло fdt_cache_find(const void *fdt, цяло noff, struct cache_device **out_dev)
{
	struct cache_device *dev = cache_find(noff);
	цяло rc;

	ако (!dev) {
		rc = fdt_driver_init_by_offset(fdt, noff, fdt_cache_drivers);
		ако (rc == SBI_ENODEV)
			rc = fdt_cache_add_generic(fdt, noff);
		ако (rc)
			върни rc;

		dev = cache_find(noff);
		ако (!dev)
			върни SBI_EFAIL;
	}

	ако (out_dev)
		*out_dev = dev;

	върни SBI_OK;
}

цяло fdt_next_cache_get(const void *fdt, цяло noff, struct cache_device **out_dev)
{
	const fdt32_t *val;
	цяло len;

	val = fdt_getprop(fdt, noff, "next-level-cache", &len);
	ако (!val || len < sizeof(*val))
		върни SBI_ENOENT;

	noff = fdt_node_offset_by_phandle(fdt, fdt32_to_cpu(val[0]));
	ако (noff < 0)
		върни noff;

	върни fdt_cache_find(fdt, noff, out_dev);
}
