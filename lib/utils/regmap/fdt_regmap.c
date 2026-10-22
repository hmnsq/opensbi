/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#ଅନ୍ତର୍ଭାଵ <libfdt.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_error.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/fdt/fdt_helper.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/regmap/fdt_regmap.h>

/* List of FDT regmap drivers generated at compile time */
ବାହ୍ୟ ଅଚଳ ସଂରଚନା fdt_driver *ଅଚଳ fdt_regmap_drivers[];

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_regmap_find(ଅଚଳ ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nodeoff,
			   ସଂରଚନା regmap **out_rmap)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;
	ସଂରଚନା regmap *rmap = regmap_find(nodeoff);

	ଯଦି (!rmap) {
		/* Regmap not found so initialize matching driver */
		rc = fdt_driver_init_by_offset(fdt, nodeoff,
					       fdt_regmap_drivers);
		ଯଦି (rc)
			ଫେରନ୍ତୁ rc;

		/* Try to find regmap again */
		rmap = regmap_find(nodeoff);
		ଯଦି (!rmap)
			ଫେରନ୍ତୁ SBI_ENOSYS;
	}

	ଯଦି (out_rmap)
		*out_rmap = rmap;

	ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_regmap_get_by_phandle(ଅଚଳ ଶୂନ୍ୟ *fdt, u32 phandle,
			      ସଂରଚନା regmap **out_rmap)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା pnodeoff;

	ଯଦି (!fdt || !out_rmap)
		ଫେରନ୍ତୁ SBI_EINVAL;

	pnodeoff = fdt_ଗଣ୍ଠି_offset_by_phandle(fdt, phandle);
	ଯଦି (pnodeoff < 0)
		ଫେରନ୍ତୁ pnodeoff;

	ଫେରନ୍ତୁ fdt_regmap_find(fdt, pnodeoff, out_rmap);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_regmap_get(ଅଚଳ ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nodeoff, ସଂରଚନା regmap **out_rmap)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା len;
	ଅଚଳ fdt32_t *val;

	ଯଦି (!fdt || (nodeoff < 0) || !out_rmap)
		ଫେରନ୍ତୁ SBI_EINVAL;

	val = fdt_getprop(fdt, nodeoff, "regmap", &len);
	ଯଦି (!val)
		ଫେରନ୍ତୁ SBI_ENOENT;

	ଫେରନ୍ତୁ fdt_regmap_get_by_phandle(fdt, fdt32_to_cpu(*val), out_rmap);
}
