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

static 整数 fdt_regmap_find(const void *fdt, 整数 nodeoff,
			   struct regmap **out_rmap)
{
	整数 rc;
	struct regmap *rmap = regmap_find(nodeoff);

	如果 (!rmap) {
		/* Regmap not found so initialize matching driver */
		rc = fdt_driver_init_by_offset(fdt, nodeoff,
					       fdt_regmap_drivers);
		如果 (rc)
			返回 rc;

		/* Try to find regmap again */
		rmap = regmap_find(nodeoff);
		如果 (!rmap)
			返回 SBI_ENOSYS;
	}

	如果 (out_rmap)
		*out_rmap = rmap;

	返回 0;
}

整数 fdt_regmap_get_by_phandle(const void *fdt, u32 phandle,
			      struct regmap **out_rmap)
{
	整数 pnodeoff;

	如果 (!fdt || !out_rmap)
		返回 SBI_EINVAL;

	pnodeoff = fdt_node_offset_by_phandle(fdt, phandle);
	如果 (pnodeoff < 0)
		返回 pnodeoff;

	返回 fdt_regmap_find(fdt, pnodeoff, out_rmap);
}

整数 fdt_regmap_get(const void *fdt, 整数 nodeoff, struct regmap **out_rmap)
{
	整数 len;
	const fdt32_t *val;

	如果 (!fdt || (nodeoff < 0) || !out_rmap)
		返回 SBI_EINVAL;

	val = fdt_getprop(fdt, nodeoff, "regmap", &len);
	如果 (!val)
		返回 SBI_ENOENT;

	返回 fdt_regmap_get_by_phandle(fdt, fdt32_to_cpu(*val), out_rmap);
}
