/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#அந்தர்பாவ <libfdt.h>
#அந்தர்பாவ <sbi/sbi_error.h>
#அந்தர்பாவ <sbi_utils/fdt/fdt_helper.h>
#அந்தர்பாவ <sbi_utils/regmap/fdt_regmap.h>

/* List of FDT regmap drivers generated at compile time */
வெளி மாறிலி கட்டமைப்பு fdt_driver *மாறிலி fdt_regmap_drivers[];

நிலையான முழு fdt_regmap_find(மாறிலி வெறுமை *fdt, முழு nodeoff,
			   கட்டமைப்பு regmap **out_rmap)
{
	முழு rc;
	கட்டமைப்பு regmap *rmap = regmap_find(nodeoff);

	எனில் (!rmap) {
		/* Regmap not found so initialize matching driver */
		rc = fdt_driver_init_by_offset(fdt, nodeoff,
					       fdt_regmap_drivers);
		எனில் (rc)
			திரும்பு rc;

		/* Try to find regmap again */
		rmap = regmap_find(nodeoff);
		எனில் (!rmap)
			திரும்பு SBI_ENOSYS;
	}

	எனில் (out_rmap)
		*out_rmap = rmap;

	திரும்பு 0;
}

முழு fdt_regmap_get_by_phandle(மாறிலி வெறுமை *fdt, u32 phandle,
			      கட்டமைப்பு regmap **out_rmap)
{
	முழு pnodeoff;

	எனில் (!fdt || !out_rmap)
		திரும்பு SBI_EINVAL;

	pnodeoff = fdt_முனை_offset_by_phandle(fdt, phandle);
	எனில் (pnodeoff < 0)
		திரும்பு pnodeoff;

	திரும்பு fdt_regmap_find(fdt, pnodeoff, out_rmap);
}

முழு fdt_regmap_get(மாறிலி வெறுமை *fdt, முழு nodeoff, கட்டமைப்பு regmap **out_rmap)
{
	முழு len;
	மாறிலி fdt32_t *val;

	எனில் (!fdt || (nodeoff < 0) || !out_rmap)
		திரும்பு SBI_EINVAL;

	val = fdt_getprop(fdt, nodeoff, "regmap", &len);
	எனில் (!val)
		திரும்பு SBI_ENOENT;

	திரும்பு fdt_regmap_get_by_phandle(fdt, fdt32_to_cpu(*val), out_rmap);
}
