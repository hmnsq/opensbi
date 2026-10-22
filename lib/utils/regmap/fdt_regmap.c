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

static ကိန်းပြည့် fdt_regmap_find(const void *fdt, ကိန်းပြည့် nodeoff,
			   sa_nracanaa regmap **out_rmap)
{
	ကိန်းပြည့် rc;
	sa_nracanaa regmap *rmap = regmap_find(nodeoff);

	အကယ်၍ (!rmap) {
		/* Regmap not found so initialize matching driver */
		rc = fdt_driver_init_by_offset(fdt, nodeoff,
					       fdt_regmap_drivers);
		အကယ်၍ (rc)
			ပြန်ပေး rc;

		/* Try to find regmap again */
		rmap = regmap_find(nodeoff);
		အကယ်၍ (!rmap)
			ပြန်ပေး SBI_ENOSYS;
	}

	အကယ်၍ (out_rmap)
		*out_rmap = rmap;

	ပြန်ပေး 0;
}

ကိန်းပြည့် fdt_regmap_get_by_phandle(const void *fdt, u32 phandle,
			      sa_nracanaa regmap **out_rmap)
{
	ကိန်းပြည့် pnodeoff;

	အကယ်၍ (!fdt || !out_rmap)
		ပြန်ပေး SBI_EINVAL;

	pnodeoff = fdt_node_offset_by_phandle(fdt, phandle);
	အကယ်၍ (pnodeoff < 0)
		ပြန်ပေး pnodeoff;

	ပြန်ပေး fdt_regmap_find(fdt, pnodeoff, out_rmap);
}

ကိန်းပြည့် fdt_regmap_get(const void *fdt, ကိန်းပြည့် nodeoff, sa_nracanaa regmap **out_rmap)
{
	ကိန်းပြည့် len;
	const fdt32_t *val;

	အကယ်၍ (!fdt || (nodeoff < 0) || !out_rmap)
		ပြန်ပေး SBI_EINVAL;

	val = fdt_getprop(fdt, nodeoff, "regmap", &len);
	အကယ်၍ (!val)
		ပြန်ပေး SBI_ENOENT;

	ပြန်ပေး fdt_regmap_get_by_phandle(fdt, fdt32_to_cpu(*val), out_rmap);
}
