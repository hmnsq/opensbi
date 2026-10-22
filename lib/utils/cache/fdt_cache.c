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

puur_naa_nka fdt_cache_add(const void *fdt, puur_naa_nka noff, sa_nracanaa cache_device *dev)
{
	puur_naa_nka rc;

	dev->id = noff;
	sbi_strncpy(dev->name, fdt_get_name(fdt, noff, NULL), parimaa_na(dev->name) - 1);
	sbi_dprintf("%s: %s\n", __func__, dev->name);

	rc = fdt_next_cache_get(fdt, noff, &dev->next);
	ᱡᱩᱫᱤ (rc == SBI_ENOENT)
		dev->next = NULL;
	ᱵᱟᱝ ᱡᱩᱫᱤ (rc)
		ᱨᱩᱣᱟᱹᱲ rc;

	ᱨᱩᱣᱟᱹᱲ cache_add(dev);
}

static puur_naa_nka fdt_cache_add_generic(const void *fdt, puur_naa_nka noff)
{
	sa_nracanaa cache_device *dev;
	puur_naa_nka rc;

	dev = sbi_zalloc(parimaa_na(*dev));
	ᱡᱩᱫᱤ (!dev)
		ᱨᱩᱣᱟᱹᱲ SBI_ENOMEM;

	rc = fdt_cache_add(fdt, noff, dev);
	ᱡᱩᱫᱤ (rc) {
		sbi_free(dev);
		ᱨᱩᱣᱟᱹᱲ rc;
	}

	ᱨᱩᱣᱟᱹᱲ 0;
}

static puur_naa_nka fdt_cache_find(const void *fdt, puur_naa_nka noff, sa_nracanaa cache_device **out_dev)
{
	sa_nracanaa cache_device *dev = cache_find(noff);
	puur_naa_nka rc;

	ᱡᱩᱫᱤ (!dev) {
		rc = fdt_driver_init_by_offset(fdt, noff, fdt_cache_drivers);
		ᱡᱩᱫᱤ (rc == SBI_ENODEV)
			rc = fdt_cache_add_generic(fdt, noff);
		ᱡᱩᱫᱤ (rc)
			ᱨᱩᱣᱟᱹᱲ rc;

		dev = cache_find(noff);
		ᱡᱩᱫᱤ (!dev)
			ᱨᱩᱣᱟᱹᱲ SBI_EFAIL;
	}

	ᱡᱩᱫᱤ (out_dev)
		*out_dev = dev;

	ᱨᱩᱣᱟᱹᱲ SBI_OK;
}

puur_naa_nka fdt_next_cache_get(const void *fdt, puur_naa_nka noff, sa_nracanaa cache_device **out_dev)
{
	const fdt32_t *val;
	puur_naa_nka len;

	val = fdt_getprop(fdt, noff, "next-level-cache", &len);
	ᱡᱩᱫᱤ (!val || len < parimaa_na(*val))
		ᱨᱩᱣᱟᱹᱲ SBI_ENOENT;

	noff = fdt_node_offset_by_phandle(fdt, fdt32_to_cpu(val[0]));
	ᱡᱩᱫᱤ (noff < 0)
		ᱨᱩᱣᱟᱹᱲ noff;

	ᱨᱩᱣᱟᱹᱲ fdt_cache_find(fdt, noff, out_dev);
}
