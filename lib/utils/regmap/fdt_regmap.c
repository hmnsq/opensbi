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

static 정수 fdt_regmap_find(const void *fdt, 정수 nodeoff,
			   struct regmap **out_rmap)
{
	정수 rc;
	struct regmap *rmap = regmap_find(nodeoff);

	만약 (!rmap) {
		/* Regmap not found so initialize matching driver */
		rc = fdt_driver_init_by_offset(fdt, nodeoff,
					       fdt_regmap_drivers);
		만약 (rc)
			반환 rc;

		/* Try to find regmap again */
		rmap = regmap_find(nodeoff);
		만약 (!rmap)
			반환 SBI_ENOSYS;
	}

	만약 (out_rmap)
		*out_rmap = rmap;

	반환 0;
}

정수 fdt_regmap_get_by_phandle(const void *fdt, u32 phandle,
			      struct regmap **out_rmap)
{
	정수 pnodeoff;

	만약 (!fdt || !out_rmap)
		반환 SBI_EINVAL;

	pnodeoff = fdt_node_offset_by_phandle(fdt, phandle);
	만약 (pnodeoff < 0)
		반환 pnodeoff;

	반환 fdt_regmap_find(fdt, pnodeoff, out_rmap);
}

정수 fdt_regmap_get(const void *fdt, 정수 nodeoff, struct regmap **out_rmap)
{
	정수 len;
	const fdt32_t *val;

	만약 (!fdt || (nodeoff < 0) || !out_rmap)
		반환 SBI_EINVAL;

	val = fdt_getprop(fdt, nodeoff, "regmap", &len);
	만약 (!val)
		반환 SBI_ENOENT;

	반환 fdt_regmap_get_by_phandle(fdt, fdt32_to_cpu(*val), out_rmap);
}
