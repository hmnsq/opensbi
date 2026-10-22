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

ကိန်းပြည့် fdt_cache_add(const void *fdt, ကိန်းပြည့် noff, sa_nracanaa cache_device *dev)
{
	ကိန်းပြည့် rc;

	dev->id = noff;
	sbi_strncpy(dev->name, fdt_get_name(fdt, noff, NULL), parimaa_na(dev->name) - 1);
	sbi_dprintf("%s: %s\n", __func__, dev->name);

	rc = fdt_next_cache_get(fdt, noff, &dev->next);
	အကယ်၍ (rc == SBI_ENOENT)
		dev->next = NULL;
	မဟုတ်ပါက အကယ်၍ (rc)
		ပြန်ပေး rc;

	ပြန်ပေး cache_add(dev);
}

static ကိန်းပြည့် fdt_cache_add_generic(const void *fdt, ကိန်းပြည့် noff)
{
	sa_nracanaa cache_device *dev;
	ကိန်းပြည့် rc;

	dev = sbi_zalloc(parimaa_na(*dev));
	အကယ်၍ (!dev)
		ပြန်ပေး SBI_ENOMEM;

	rc = fdt_cache_add(fdt, noff, dev);
	အကယ်၍ (rc) {
		sbi_free(dev);
		ပြန်ပေး rc;
	}

	ပြန်ပေး 0;
}

static ကိန်းပြည့် fdt_cache_find(const void *fdt, ကိန်းပြည့် noff, sa_nracanaa cache_device **out_dev)
{
	sa_nracanaa cache_device *dev = cache_find(noff);
	ကိန်းပြည့် rc;

	အကယ်၍ (!dev) {
		rc = fdt_driver_init_by_offset(fdt, noff, fdt_cache_drivers);
		အကယ်၍ (rc == SBI_ENODEV)
			rc = fdt_cache_add_generic(fdt, noff);
		အကယ်၍ (rc)
			ပြန်ပေး rc;

		dev = cache_find(noff);
		အကယ်၍ (!dev)
			ပြန်ပေး SBI_EFAIL;
	}

	အကယ်၍ (out_dev)
		*out_dev = dev;

	ပြန်ပေး SBI_OK;
}

ကိန်းပြည့် fdt_next_cache_get(const void *fdt, ကိန်းပြည့် noff, sa_nracanaa cache_device **out_dev)
{
	const fdt32_t *val;
	ကိန်းပြည့် len;

	val = fdt_getprop(fdt, noff, "next-level-cache", &len);
	အကယ်၍ (!val || len < parimaa_na(*val))
		ပြန်ပေး SBI_ENOENT;

	noff = fdt_node_offset_by_phandle(fdt, fdt32_to_cpu(val[0]));
	အကယ်၍ (noff < 0)
		ပြန်ပေး noff;

	ပြန်ပေး fdt_cache_find(fdt, noff, out_dev);
}
