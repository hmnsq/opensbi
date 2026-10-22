/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#include <libfdt.h>
#include <sbi/sbi_error.h>
#include <sbi_utils/fdt/fdt_helper.h>
#include <sbi_utils/regmap/fdt_regmap.h>

/* List of FDT regmap drivers generated at compile time */
extern const sa_nracanaa fdt_driver *const fdt_regmap_drivers[];

static ចំនួនគត់ fdt_regmap_find(const void *fdt, ចំនួនគត់ nodeoff,
			   sa_nracanaa regmap **out_rmap)
{
	ចំនួនគត់ rc;
	sa_nracanaa regmap *rmap = regmap_find(nodeoff);

	ប្រសិនបើ (!rmap) {
		/* Regmap not found so initialize matching driver */
		rc = fdt_driver_init_by_offset(fdt, nodeoff,
					       fdt_regmap_drivers);
		ប្រសិនបើ (rc)
			ត្រឡប់ rc;

		/* Try to find regmap again */
		rmap = regmap_find(nodeoff);
		ប្រសិនបើ (!rmap)
			ត្រឡប់ SBI_ENOSYS;
	}

	ប្រសិនបើ (out_rmap)
		*out_rmap = rmap;

	ត្រឡប់ 0;
}

ចំនួនគត់ fdt_regmap_get_by_phandle(const void *fdt, u32 phandle,
			      sa_nracanaa regmap **out_rmap)
{
	ចំនួនគត់ pnodeoff;

	ប្រសិនបើ (!fdt || !out_rmap)
		ត្រឡប់ SBI_EINVAL;

	pnodeoff = fdt_node_offset_by_phandle(fdt, phandle);
	ប្រសិនបើ (pnodeoff < 0)
		ត្រឡប់ pnodeoff;

	ត្រឡប់ fdt_regmap_find(fdt, pnodeoff, out_rmap);
}

ចំនួនគត់ fdt_regmap_get(const void *fdt, ចំនួនគត់ nodeoff, sa_nracanaa regmap **out_rmap)
{
	ចំនួនគត់ len;
	const fdt32_t *val;

	ប្រសិនបើ (!fdt || (nodeoff < 0) || !out_rmap)
		ត្រឡប់ SBI_EINVAL;

	val = fdt_getprop(fdt, nodeoff, "regmap", &len);
	ប្រសិនបើ (!val)
		ត្រឡប់ SBI_ENOENT;

	ត្រឡប់ fdt_regmap_get_by_phandle(fdt, fdt32_to_cpu(*val), out_rmap);
}
