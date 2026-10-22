/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#অন্তর্ভাব <libfdt.h>
#অন্তর্ভাব <sbi/sbi_error.h>
#অন্তর্ভাব <sbi_utils/fdt/fdt_helper.h>
#অন্তর্ভাব <sbi_utils/regmap/fdt_regmap.h>

/* List of FDT regmap drivers generated at compile time */
বাহ্য ধ্রুব সংরচনা fdt_driver *ধ্রুব fdt_regmap_drivers[];

স্থির পূর্ণাঙ্ক fdt_regmap_find(ধ্রুব শূন্য *fdt, পূর্ণাঙ্ক nodeoff,
			   সংরচনা regmap **out_rmap)
{
	পূর্ণাঙ্ক rc;
	সংরচনা regmap *rmap = regmap_find(nodeoff);

	যদি (!rmap) {
		/* Regmap not found so initialize matching driver */
		rc = fdt_driver_init_by_offset(fdt, nodeoff,
					       fdt_regmap_drivers);
		যদি (rc)
			প্রত্যাগম rc;

		/* Try to find regmap again */
		rmap = regmap_find(nodeoff);
		যদি (!rmap)
			প্রত্যাগম SBI_ENOSYS;
	}

	যদি (out_rmap)
		*out_rmap = rmap;

	প্রত্যাগম 0;
}

পূর্ণাঙ্ক fdt_regmap_get_by_phandle(ধ্রুব শূন্য *fdt, u32 phandle,
			      সংরচনা regmap **out_rmap)
{
	পূর্ণাঙ্ক pnodeoff;

	যদি (!fdt || !out_rmap)
		প্রত্যাগম SBI_EINVAL;

	pnodeoff = fdt_গ্রন্থি_offset_by_phandle(fdt, phandle);
	যদি (pnodeoff < 0)
		প্রত্যাগম pnodeoff;

	প্রত্যাগম fdt_regmap_find(fdt, pnodeoff, out_rmap);
}

পূর্ণাঙ্ক fdt_regmap_get(ধ্রুব শূন্য *fdt, পূর্ণাঙ্ক nodeoff, সংরচনা regmap **out_rmap)
{
	পূর্ণাঙ্ক len;
	ধ্রুব fdt32_t *val;

	যদি (!fdt || (nodeoff < 0) || !out_rmap)
		প্রত্যাগম SBI_EINVAL;

	val = fdt_getprop(fdt, nodeoff, "regmap", &len);
	যদি (!val)
		প্রত্যাগম SBI_ENOENT;

	প্রত্যাগম fdt_regmap_get_by_phandle(fdt, fdt32_to_cpu(*val), out_rmap);
}
