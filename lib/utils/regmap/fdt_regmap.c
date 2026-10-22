/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#অন্তর্ভাৱ <libfdt.h>
#অন্তর্ভাৱ <sbi/sbi_error.h>
#অন্তর্ভাৱ <sbi_utils/fdt/fdt_helper.h>
#অন্তর্ভাৱ <sbi_utils/regmap/fdt_regmap.h>

/* List of FDT regmap drivers generated at compile time */
বাহ্য ধ্রুৱ সংরচনা fdt_driver *ধ্রুৱ fdt_regmap_drivers[];

স্থির পূর্ণাঙ্ক fdt_regmap_find(ধ্রুৱ শূন্য *fdt, পূর্ণাঙ্ক nodeoff,
			   সংরচনা regmap **out_rmap)
{
	পূর্ণাঙ্ক rc;
	সংরচনা regmap *rmap = regmap_find(nodeoff);

	যদি (!rmap) {
		/* Regmap not found so initialize matching driver */
		rc = fdt_driver_init_by_offset(fdt, nodeoff,
					       fdt_regmap_drivers);
		যদি (rc)
			ঘূৰাই rc;

		/* Try to find regmap again */
		rmap = regmap_find(nodeoff);
		যদি (!rmap)
			ঘূৰাই SBI_ENOSYS;
	}

	যদি (out_rmap)
		*out_rmap = rmap;

	ঘূৰাই 0;
}

পূর্ণাঙ্ক fdt_regmap_get_by_phandle(ধ্রুৱ শূন্য *fdt, u32 phandle,
			      সংরচনা regmap **out_rmap)
{
	পূর্ণাঙ্ক pnodeoff;

	যদি (!fdt || !out_rmap)
		ঘূৰাই SBI_EINVAL;

	pnodeoff = fdt_গ্ৰন্থি_offset_by_phandle(fdt, phandle);
	যদি (pnodeoff < 0)
		ঘূৰাই pnodeoff;

	ঘূৰাই fdt_regmap_find(fdt, pnodeoff, out_rmap);
}

পূর্ণাঙ্ক fdt_regmap_get(ধ্রুৱ শূন্য *fdt, পূর্ণাঙ্ক nodeoff, সংরচনা regmap **out_rmap)
{
	পূর্ণাঙ্ক len;
	ধ্রুৱ fdt32_t *val;

	যদি (!fdt || (nodeoff < 0) || !out_rmap)
		ঘূৰাই SBI_EINVAL;

	val = fdt_getprop(fdt, nodeoff, "regmap", &len);
	যদি (!val)
		ঘূৰাই SBI_ENOENT;

	ঘূৰাই fdt_regmap_get_by_phandle(fdt, fdt32_to_cpu(*val), out_rmap);
}
