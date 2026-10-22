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

static số_nguyên fdt_regmap_find(const void *fdt, số_nguyên nodeoff,
			   struct regmap **out_rmap)
{
	số_nguyên rc;
	struct regmap *rmap = regmap_find(nodeoff);

	nếu (!rmap) {
		/* Regmap not found so initialize matching driver */
		rc = fdt_driver_init_by_offset(fdt, nodeoff,
					       fdt_regmap_drivers);
		nếu (rc)
			trả_về rc;

		/* Try to find regmap again */
		rmap = regmap_find(nodeoff);
		nếu (!rmap)
			trả_về SBI_ENOSYS;
	}

	nếu (out_rmap)
		*out_rmap = rmap;

	trả_về 0;
}

số_nguyên fdt_regmap_get_by_phandle(const void *fdt, u32 phandle,
			      struct regmap **out_rmap)
{
	số_nguyên pnodeoff;

	nếu (!fdt || !out_rmap)
		trả_về SBI_EINVAL;

	pnodeoff = fdt_node_offset_by_phandle(fdt, phandle);
	nếu (pnodeoff < 0)
		trả_về pnodeoff;

	trả_về fdt_regmap_find(fdt, pnodeoff, out_rmap);
}

số_nguyên fdt_regmap_get(const void *fdt, số_nguyên nodeoff, struct regmap **out_rmap)
{
	số_nguyên len;
	const fdt32_t *val;

	nếu (!fdt || (nodeoff < 0) || !out_rmap)
		trả_về SBI_EINVAL;

	val = fdt_getprop(fdt, nodeoff, "regmap", &len);
	nếu (!val)
		trả_về SBI_ENOENT;

	trả_về fdt_regmap_get_by_phandle(fdt, fdt32_to_cpu(*val), out_rmap);
}
