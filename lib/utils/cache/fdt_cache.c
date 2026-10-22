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

පූර්ණ_සංඛ්‍යා fdt_cache_add(const void *fdt, පූර්ණ_සංඛ්‍යා noff, sa_nracanaa cache_device *dev)
{
	පූර්ණ_සංඛ්‍යා rc;

	dev->id = noff;
	sbi_strncpy(dev->name, fdt_get_name(fdt, noff, NULL), parimaa_na(dev->name) - 1);
	sbi_dprintf("%s: %s\n", __func__, dev->name);

	rc = fdt_next_cache_get(fdt, noff, &dev->next);
	නම් (rc == SBI_ENOENT)
		dev->next = NULL;
	නැතහොත් නම් (rc)
		ආපසු rc;

	ආපසු cache_add(dev);
}

static පූර්ණ_සංඛ්‍යා fdt_cache_add_generic(const void *fdt, පූර්ණ_සංඛ්‍යා noff)
{
	sa_nracanaa cache_device *dev;
	පූර්ණ_සංඛ්‍යා rc;

	dev = sbi_zalloc(parimaa_na(*dev));
	නම් (!dev)
		ආපසු SBI_ENOMEM;

	rc = fdt_cache_add(fdt, noff, dev);
	නම් (rc) {
		sbi_free(dev);
		ආපසු rc;
	}

	ආපසු 0;
}

static පූර්ණ_සංඛ්‍යා fdt_cache_find(const void *fdt, පූර්ණ_සංඛ්‍යා noff, sa_nracanaa cache_device **out_dev)
{
	sa_nracanaa cache_device *dev = cache_find(noff);
	පූර්ණ_සංඛ්‍යා rc;

	නම් (!dev) {
		rc = fdt_driver_init_by_offset(fdt, noff, fdt_cache_drivers);
		නම් (rc == SBI_ENODEV)
			rc = fdt_cache_add_generic(fdt, noff);
		නම් (rc)
			ආපසු rc;

		dev = cache_find(noff);
		නම් (!dev)
			ආපසු SBI_EFAIL;
	}

	නම් (out_dev)
		*out_dev = dev;

	ආපසු SBI_OK;
}

පූර්ණ_සංඛ්‍යා fdt_next_cache_get(const void *fdt, පූර්ණ_සංඛ්‍යා noff, sa_nracanaa cache_device **out_dev)
{
	const fdt32_t *val;
	පූර්ණ_සංඛ්‍යා len;

	val = fdt_getprop(fdt, noff, "next-level-cache", &len);
	නම් (!val || len < parimaa_na(*val))
		ආපසු SBI_ENOENT;

	noff = fdt_node_offset_by_phandle(fdt, fdt32_to_cpu(val[0]));
	නම් (noff < 0)
		ආපසු noff;

	ආපසු fdt_cache_find(fdt, noff, out_dev);
}
