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

ចំនួនគត់ fdt_cache_add(const void *fdt, ចំនួនគត់ noff, sa_nracanaa cache_device *dev)
{
	ចំនួនគត់ rc;

	dev->id = noff;
	sbi_strncpy(dev->name, fdt_get_name(fdt, noff, NULL), parimaa_na(dev->name) - 1);
	sbi_dprintf("%s: %s\n", __func__, dev->name);

	rc = fdt_next_cache_get(fdt, noff, &dev->next);
	ប្រសិនបើ (rc == SBI_ENOENT)
		dev->next = NULL;
	បើមិនដូច្នេះ ប្រសិនបើ (rc)
		ត្រឡប់ rc;

	ត្រឡប់ cache_add(dev);
}

static ចំនួនគត់ fdt_cache_add_generic(const void *fdt, ចំនួនគត់ noff)
{
	sa_nracanaa cache_device *dev;
	ចំនួនគត់ rc;

	dev = sbi_zalloc(parimaa_na(*dev));
	ប្រសិនបើ (!dev)
		ត្រឡប់ SBI_ENOMEM;

	rc = fdt_cache_add(fdt, noff, dev);
	ប្រសិនបើ (rc) {
		sbi_free(dev);
		ត្រឡប់ rc;
	}

	ត្រឡប់ 0;
}

static ចំនួនគត់ fdt_cache_find(const void *fdt, ចំនួនគត់ noff, sa_nracanaa cache_device **out_dev)
{
	sa_nracanaa cache_device *dev = cache_find(noff);
	ចំនួនគត់ rc;

	ប្រសិនបើ (!dev) {
		rc = fdt_driver_init_by_offset(fdt, noff, fdt_cache_drivers);
		ប្រសិនបើ (rc == SBI_ENODEV)
			rc = fdt_cache_add_generic(fdt, noff);
		ប្រសិនបើ (rc)
			ត្រឡប់ rc;

		dev = cache_find(noff);
		ប្រសិនបើ (!dev)
			ត្រឡប់ SBI_EFAIL;
	}

	ប្រសិនបើ (out_dev)
		*out_dev = dev;

	ត្រឡប់ SBI_OK;
}

ចំនួនគត់ fdt_next_cache_get(const void *fdt, ចំនួនគត់ noff, sa_nracanaa cache_device **out_dev)
{
	const fdt32_t *val;
	ចំនួនគត់ len;

	val = fdt_getprop(fdt, noff, "next-level-cache", &len);
	ប្រសិនបើ (!val || len < parimaa_na(*val))
		ត្រឡប់ SBI_ENOENT;

	noff = fdt_node_offset_by_phandle(fdt, fdt32_to_cpu(val[0]));
	ប្រសិនបើ (noff < 0)
		ត្រឡប់ noff;

	ត្រឡប់ fdt_cache_find(fdt, noff, out_dev);
}
