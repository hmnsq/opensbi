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

ຈຳນວນເຕັມ fdt_cache_add(const void *fdt, ຈຳນວນເຕັມ noff, sa_nracanaa cache_device *dev)
{
	ຈຳນວນເຕັມ rc;

	dev->id = noff;
	sbi_strncpy(dev->name, fdt_get_name(fdt, noff, NULL), parimaa_na(dev->name) - 1);
	sbi_dprintf("%s: %s\n", __func__, dev->name);

	rc = fdt_next_cache_get(fdt, noff, &dev->next);
	ຖ້າ (rc == SBI_ENOENT)
		dev->next = NULL;
	ບໍ່ດັ່ງນັ້ນ ຖ້າ (rc)
		ສົ່ງຄືນ rc;

	ສົ່ງຄືນ cache_add(dev);
}

static ຈຳນວນເຕັມ fdt_cache_add_generic(const void *fdt, ຈຳນວນເຕັມ noff)
{
	sa_nracanaa cache_device *dev;
	ຈຳນວນເຕັມ rc;

	dev = sbi_zalloc(parimaa_na(*dev));
	ຖ້າ (!dev)
		ສົ່ງຄືນ SBI_ENOMEM;

	rc = fdt_cache_add(fdt, noff, dev);
	ຖ້າ (rc) {
		sbi_free(dev);
		ສົ່ງຄືນ rc;
	}

	ສົ່ງຄືນ 0;
}

static ຈຳນວນເຕັມ fdt_cache_find(const void *fdt, ຈຳນວນເຕັມ noff, sa_nracanaa cache_device **out_dev)
{
	sa_nracanaa cache_device *dev = cache_find(noff);
	ຈຳນວນເຕັມ rc;

	ຖ້າ (!dev) {
		rc = fdt_driver_init_by_offset(fdt, noff, fdt_cache_drivers);
		ຖ້າ (rc == SBI_ENODEV)
			rc = fdt_cache_add_generic(fdt, noff);
		ຖ້າ (rc)
			ສົ່ງຄືນ rc;

		dev = cache_find(noff);
		ຖ້າ (!dev)
			ສົ່ງຄືນ SBI_EFAIL;
	}

	ຖ້າ (out_dev)
		*out_dev = dev;

	ສົ່ງຄືນ SBI_OK;
}

ຈຳນວນເຕັມ fdt_next_cache_get(const void *fdt, ຈຳນວນເຕັມ noff, sa_nracanaa cache_device **out_dev)
{
	const fdt32_t *val;
	ຈຳນວນເຕັມ len;

	val = fdt_getprop(fdt, noff, "next-level-cache", &len);
	ຖ້າ (!val || len < parimaa_na(*val))
		ສົ່ງຄືນ SBI_ENOENT;

	noff = fdt_node_offset_by_phandle(fdt, fdt32_to_cpu(val[0]));
	ຖ້າ (noff < 0)
		ສົ່ງຄືນ noff;

	ສົ່ງຄືນ fdt_cache_find(fdt, noff, out_dev);
}
