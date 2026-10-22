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

static පූර්ණ_සංඛ්‍යා fdt_regmap_find(const void *fdt, පූර්ණ_සංඛ්‍යා nodeoff,
			   sa_nracanaa regmap **out_rmap)
{
	පූර්ණ_සංඛ්‍යා rc;
	sa_nracanaa regmap *rmap = regmap_find(nodeoff);

	නම් (!rmap) {
		/* Regmap not found so initialize matching driver */
		rc = fdt_driver_init_by_offset(fdt, nodeoff,
					       fdt_regmap_drivers);
		නම් (rc)
			ආපසු rc;

		/* Try to find regmap again */
		rmap = regmap_find(nodeoff);
		නම් (!rmap)
			ආපසු SBI_ENOSYS;
	}

	නම් (out_rmap)
		*out_rmap = rmap;

	ආපසු 0;
}

පූර්ණ_සංඛ්‍යා fdt_regmap_get_by_phandle(const void *fdt, u32 phandle,
			      sa_nracanaa regmap **out_rmap)
{
	පූර්ණ_සංඛ්‍යා pnodeoff;

	නම් (!fdt || !out_rmap)
		ආපසු SBI_EINVAL;

	pnodeoff = fdt_node_offset_by_phandle(fdt, phandle);
	නම් (pnodeoff < 0)
		ආපසු pnodeoff;

	ආපසු fdt_regmap_find(fdt, pnodeoff, out_rmap);
}

පූර්ණ_සංඛ්‍යා fdt_regmap_get(const void *fdt, පූර්ණ_සංඛ්‍යා nodeoff, sa_nracanaa regmap **out_rmap)
{
	පූර්ණ_සංඛ්‍යා len;
	const fdt32_t *val;

	නම් (!fdt || (nodeoff < 0) || !out_rmap)
		ආපසු SBI_EINVAL;

	val = fdt_getprop(fdt, nodeoff, "regmap", &len);
	නම් (!val)
		ආපසු SBI_ENOENT;

	ආපසු fdt_regmap_get_by_phandle(fdt, fdt32_to_cpu(*val), out_rmap);
}
