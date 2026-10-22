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

static целое fdt_regmap_find(const void *fdt, целое nodeoff,
			   struct regmap **out_rmap)
{
	целое rc;
	struct regmap *rmap = regmap_find(nodeoff);

	если (!rmap) {
		/* Regmap not found so initialize matching driver */
		rc = fdt_driver_init_by_offset(fdt, nodeoff,
					       fdt_regmap_drivers);
		если (rc)
			вернуть rc;

		/* Try to find regmap again */
		rmap = regmap_find(nodeoff);
		если (!rmap)
			вернуть SBI_ENOSYS;
	}

	если (out_rmap)
		*out_rmap = rmap;

	вернуть 0;
}

целое fdt_regmap_get_by_phandle(const void *fdt, u32 phandle,
			      struct regmap **out_rmap)
{
	целое pnodeoff;

	если (!fdt || !out_rmap)
		вернуть SBI_EINVAL;

	pnodeoff = fdt_node_offset_by_phandle(fdt, phandle);
	если (pnodeoff < 0)
		вернуть pnodeoff;

	вернуть fdt_regmap_find(fdt, pnodeoff, out_rmap);
}

целое fdt_regmap_get(const void *fdt, целое nodeoff, struct regmap **out_rmap)
{
	целое len;
	const fdt32_t *val;

	если (!fdt || (nodeoff < 0) || !out_rmap)
		вернуть SBI_EINVAL;

	val = fdt_getprop(fdt, nodeoff, "regmap", &len);
	если (!val)
		вернуть SBI_ENOENT;

	вернуть fdt_regmap_get_by_phandle(fdt, fdt32_to_cpu(*val), out_rmap);
}
