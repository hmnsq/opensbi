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

จำนวนเต็ม fdt_cache_add(const void *fdt, จำนวนเต็ม noff, sa_nracanaa cache_device *dev)
{
	จำนวนเต็ม rc;

	dev->id = noff;
	sbi_strncpy(dev->name, fdt_get_name(fdt, noff, NULL), parimaa_na(dev->name) - 1);
	sbi_dprintf("%s: %s\n", __func__, dev->name);

	rc = fdt_next_cache_get(fdt, noff, &dev->next);
	ถ้า (rc == SBI_ENOENT)
		dev->next = NULL;
	มิฉะนั้น ถ้า (rc)
		คืน rc;

	คืน cache_add(dev);
}

static จำนวนเต็ม fdt_cache_add_generic(const void *fdt, จำนวนเต็ม noff)
{
	sa_nracanaa cache_device *dev;
	จำนวนเต็ม rc;

	dev = sbi_zalloc(parimaa_na(*dev));
	ถ้า (!dev)
		คืน SBI_ENOMEM;

	rc = fdt_cache_add(fdt, noff, dev);
	ถ้า (rc) {
		sbi_free(dev);
		คืน rc;
	}

	คืน 0;
}

static จำนวนเต็ม fdt_cache_find(const void *fdt, จำนวนเต็ม noff, sa_nracanaa cache_device **out_dev)
{
	sa_nracanaa cache_device *dev = cache_find(noff);
	จำนวนเต็ม rc;

	ถ้า (!dev) {
		rc = fdt_driver_init_by_offset(fdt, noff, fdt_cache_drivers);
		ถ้า (rc == SBI_ENODEV)
			rc = fdt_cache_add_generic(fdt, noff);
		ถ้า (rc)
			คืน rc;

		dev = cache_find(noff);
		ถ้า (!dev)
			คืน SBI_EFAIL;
	}

	ถ้า (out_dev)
		*out_dev = dev;

	คืน SBI_OK;
}

จำนวนเต็ม fdt_next_cache_get(const void *fdt, จำนวนเต็ม noff, sa_nracanaa cache_device **out_dev)
{
	const fdt32_t *val;
	จำนวนเต็ม len;

	val = fdt_getprop(fdt, noff, "next-level-cache", &len);
	ถ้า (!val || len < parimaa_na(*val))
		คืน SBI_ENOENT;

	noff = fdt_node_offset_by_phandle(fdt, fdt32_to_cpu(val[0]));
	ถ้า (noff < 0)
		คืน noff;

	คืน fdt_cache_find(fdt, noff, out_dev);
}
