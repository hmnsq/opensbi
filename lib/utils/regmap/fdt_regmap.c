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

static ཧྲིལ fdt_regmap_find(const void *fdt, ཧྲིལ nodeoff,
			   sa_nracanaa regmap **out_rmap)
{
	ཧྲིལ rc;
	sa_nracanaa regmap *rmap = regmap_find(nodeoff);

	གལ་སྲིད (!rmap) {
		/* Regmap not found so initialize matching driver */
		rc = fdt_driver_init_by_offset(fdt, nodeoff,
					       fdt_regmap_drivers);
		གལ་སྲིད (rc)
			ལོག rc;

		/* Try to find regmap again */
		rmap = regmap_find(nodeoff);
		གལ་སྲིད (!rmap)
			ལོག SBI_ENOSYS;
	}

	གལ་སྲིད (out_rmap)
		*out_rmap = rmap;

	ལོག 0;
}

ཧྲིལ fdt_regmap_get_by_phandle(const void *fdt, u32 phandle,
			      sa_nracanaa regmap **out_rmap)
{
	ཧྲིལ pnodeoff;

	གལ་སྲིད (!fdt || !out_rmap)
		ལོག SBI_EINVAL;

	pnodeoff = fdt_node_offset_by_phandle(fdt, phandle);
	གལ་སྲིད (pnodeoff < 0)
		ལོག pnodeoff;

	ལོག fdt_regmap_find(fdt, pnodeoff, out_rmap);
}

ཧྲིལ fdt_regmap_get(const void *fdt, ཧྲིལ nodeoff, sa_nracanaa regmap **out_rmap)
{
	ཧྲིལ len;
	const fdt32_t *val;

	གལ་སྲིད (!fdt || (nodeoff < 0) || !out_rmap)
		ལོག SBI_EINVAL;

	val = fdt_getprop(fdt, nodeoff, "regmap", &len);
	གལ་སྲིད (!val)
		ལོག SBI_ENOENT;

	ལོག fdt_regmap_get_by_phandle(fdt, fdt32_to_cpu(*val), out_rmap);
}
