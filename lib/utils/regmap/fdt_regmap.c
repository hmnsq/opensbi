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
extern const struct fdt_driver *const fdt_regmap_drivers[];

static цео fdt_regmap_find(const void *fdt, цео nodeoff,
			   struct regmap **out_rmap)
{
	цео rc;
	struct regmap *rmap = regmap_find(nodeoff);

	ако (!rmap) {
		/* Regmap not found so initialize matching driver */
		rc = fdt_driver_init_by_offset(fdt, nodeoff,
					       fdt_regmap_drivers);
		ако (rc)
			врати rc;

		/* Try to find regmap again */
		rmap = regmap_find(nodeoff);
		ако (!rmap)
			врати SBI_ENOSYS;
	}

	ако (out_rmap)
		*out_rmap = rmap;

	врати 0;
}

цео fdt_regmap_get_by_phandle(const void *fdt, u32 phandle,
			      struct regmap **out_rmap)
{
	цео pnodeoff;

	ако (!fdt || !out_rmap)
		врати SBI_EINVAL;

	pnodeoff = fdt_node_offset_by_phandle(fdt, phandle);
	ако (pnodeoff < 0)
		врати pnodeoff;

	врати fdt_regmap_find(fdt, pnodeoff, out_rmap);
}

цео fdt_regmap_get(const void *fdt, цео nodeoff, struct regmap **out_rmap)
{
	цео len;
	const fdt32_t *val;

	ако (!fdt || (nodeoff < 0) || !out_rmap)
		врати SBI_EINVAL;

	val = fdt_getprop(fdt, nodeoff, "regmap", &len);
	ако (!val)
		врати SBI_ENOENT;

	врати fdt_regmap_get_by_phandle(fdt, fdt32_to_cpu(*val), out_rmap);
}
