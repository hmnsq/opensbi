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
বাহ্যিক ধ্রুবক কাঠামো fdt_driver *ধ্রুবক fdt_regmap_drivers[];

স্থির পূর্ণসংখ্যা fdt_regmap_find(ধ্রুবক শূন্য *fdt, পূর্ণসংখ্যা nodeoff,
			   কাঠামো regmap **out_rmap)
{
	পূর্ণসংখ্যা rc;
	কাঠামো regmap *rmap = regmap_find(nodeoff);

	যদি (!rmap) {
		/* Regmap not found so initialize matching driver */
		rc = fdt_driver_init_by_offset(fdt, nodeoff,
					       fdt_regmap_drivers);
		যদি (rc)
			ফেরত rc;

		/* Try to find regmap again */
		rmap = regmap_find(nodeoff);
		যদি (!rmap)
			ফেরত SBI_ENOSYS;
	}

	যদি (out_rmap)
		*out_rmap = rmap;

	ফেরত 0;
}

পূর্ণসংখ্যা fdt_regmap_get_by_phandle(ধ্রুবক শূন্য *fdt, u32 phandle,
			      কাঠামো regmap **out_rmap)
{
	পূর্ণসংখ্যা pnodeoff;

	যদি (!fdt || !out_rmap)
		ফেরত SBI_EINVAL;

	pnodeoff = fdt_গ্রন্থি_offset_by_phandle(fdt, phandle);
	যদি (pnodeoff < 0)
		ফেরত pnodeoff;

	ফেরত fdt_regmap_find(fdt, pnodeoff, out_rmap);
}

পূর্ণসংখ্যা fdt_regmap_get(ধ্রুবক শূন্য *fdt, পূর্ণসংখ্যা nodeoff, কাঠামো regmap **out_rmap)
{
	পূর্ণসংখ্যা len;
	ধ্রুবক fdt32_t *val;

	যদি (!fdt || (nodeoff < 0) || !out_rmap)
		ফেরত SBI_EINVAL;

	val = fdt_getprop(fdt, nodeoff, "regmap", &len);
	যদি (!val)
		ফেরত SBI_ENOENT;

	ফেরত fdt_regmap_get_by_phandle(fdt, fdt32_to_cpu(*val), out_rmap);
}
