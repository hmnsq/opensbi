/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#ضمن <libfdt.h>
#ضمن <sbi/sbi_error.h>
#ضمن <sbi_utils/fdt/fdt_helper.h>
#ضمن <sbi_utils/regmap/fdt_regmap.h>

/* List of FDT regmap drivers generated at compile time */
خارجي ثابت بنية fdt_driver *ثابت fdt_regmap_drivers[];

ساكن صحيح fdt_regmap_find(ثابت فراغ *fdt, صحيح nodeoff,
			   بنية regmap **out_rmap)
{
	صحيح rc;
	بنية regmap *rmap = regmap_find(nodeoff);

	ܐܢ (!rmap) {
		/* Regmap not found so initialize matching driver */
		rc = fdt_driver_init_by_offset(fdt, nodeoff,
					       fdt_regmap_drivers);
		ܐܢ (rc)
			ܦܢܐ rc;

		/* Try to find regmap again */
		rmap = regmap_find(nodeoff);
		ܐܢ (!rmap)
			ܦܢܐ SBI_ENOSYS;
	}

	ܐܢ (out_rmap)
		*out_rmap = rmap;

	ܦܢܐ 0;
}

صحيح fdt_regmap_get_by_phandle(ثابت فراغ *fdt, u32 phandle,
			      بنية regmap **out_rmap)
{
	صحيح pnodeoff;

	ܐܢ (!fdt || !out_rmap)
		ܦܢܐ SBI_EINVAL;

	pnodeoff = fdt_node_offset_by_phandle(fdt, phandle);
	ܐܢ (pnodeoff < 0)
		ܦܢܐ pnodeoff;

	ܦܢܐ fdt_regmap_find(fdt, pnodeoff, out_rmap);
}

صحيح fdt_regmap_get(ثابت فراغ *fdt, صحيح nodeoff, بنية regmap **out_rmap)
{
	صحيح len;
	ثابت fdt32_t *val;

	ܐܢ (!fdt || (nodeoff < 0) || !out_rmap)
		ܦܢܐ SBI_EINVAL;

	val = fdt_getprop(fdt, nodeoff, "regmap", &len);
	ܐܢ (!val)
		ܦܢܐ SBI_ENOENT;

	ܦܢܐ fdt_regmap_get_by_phandle(fdt, fdt32_to_cpu(*val), out_rmap);
}
