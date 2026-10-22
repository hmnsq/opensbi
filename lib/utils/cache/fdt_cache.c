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

số_nguyên fdt_cache_add(const void *fdt, số_nguyên noff, struct cache_device *dev)
{
	số_nguyên rc;

	dev->id = noff;
	sbi_strncpy(dev->name, fdt_get_name(fdt, noff, NULL), sizeof(dev->name) - 1);
	sbi_dprintf("%s: %s\n", __func__, dev->name);

	rc = fdt_next_cache_get(fdt, noff, &dev->next);
	nếu (rc == SBI_ENOENT)
		dev->next = NULL;
	không_thì nếu (rc)
		trả_về rc;

	trả_về cache_add(dev);
}

static số_nguyên fdt_cache_add_generic(const void *fdt, số_nguyên noff)
{
	struct cache_device *dev;
	số_nguyên rc;

	dev = sbi_zalloc(sizeof(*dev));
	nếu (!dev)
		trả_về SBI_ENOMEM;

	rc = fdt_cache_add(fdt, noff, dev);
	nếu (rc) {
		sbi_free(dev);
		trả_về rc;
	}

	trả_về 0;
}

static số_nguyên fdt_cache_find(const void *fdt, số_nguyên noff, struct cache_device **out_dev)
{
	struct cache_device *dev = cache_find(noff);
	số_nguyên rc;

	nếu (!dev) {
		rc = fdt_driver_init_by_offset(fdt, noff, fdt_cache_drivers);
		nếu (rc == SBI_ENODEV)
			rc = fdt_cache_add_generic(fdt, noff);
		nếu (rc)
			trả_về rc;

		dev = cache_find(noff);
		nếu (!dev)
			trả_về SBI_EFAIL;
	}

	nếu (out_dev)
		*out_dev = dev;

	trả_về SBI_OK;
}

số_nguyên fdt_next_cache_get(const void *fdt, số_nguyên noff, struct cache_device **out_dev)
{
	const fdt32_t *val;
	số_nguyên len;

	val = fdt_getprop(fdt, noff, "next-level-cache", &len);
	nếu (!val || len < sizeof(*val))
		trả_về SBI_ENOENT;

	noff = fdt_node_offset_by_phandle(fdt, fdt32_to_cpu(val[0]));
	nếu (noff < 0)
		trả_về noff;

	trả_về fdt_cache_find(fdt, noff, out_dev);
}
