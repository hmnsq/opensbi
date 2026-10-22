/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#شامل <libfdt.h>
#شامل <sbi/sbi_error.h>
#شامل <sbi_utils/fdt/fdt_helper.h>
#شامل <sbi_utils/regmap/fdt_regmap.h>

/* List of FDT regmap drivers generated at compile time */
بیرونی ثابت ساختار fdt_driver *ثابت fdt_regmap_drivers[];

ایستا صحیح fdt_regmap_find(ثابت تهی *fdt, صحیح nodeoff,
			   ساختار regmap **out_rmap)
{
	صحیح rc;
	ساختار regmap *rmap = regmap_find(nodeoff);

	اگر (!rmap) {
		/* Regmap not found so initialize matching driver */
		rc = fdt_driver_init_by_offset(fdt, nodeoff,
					       fdt_regmap_drivers);
		اگر (rc)
			بازگشت rc;

		/* Try to find regmap again */
		rmap = regmap_find(nodeoff);
		اگر (!rmap)
			بازگشت SBI_ENOSYS;
	}

	اگر (out_rmap)
		*out_rmap = rmap;

	بازگشت 0;
}

صحیح fdt_regmap_get_by_phandle(ثابت تهی *fdt, u32 phandle,
			      ساختار regmap **out_rmap)
{
	صحیح pnodeoff;

	اگر (!fdt || !out_rmap)
		بازگشت SBI_EINVAL;

	pnodeoff = fdt_گره_offset_by_phandle(fdt, phandle);
	اگر (pnodeoff < 0)
		بازگشت pnodeoff;

	بازگشت fdt_regmap_find(fdt, pnodeoff, out_rmap);
}

صحیح fdt_regmap_get(ثابت تهی *fdt, صحیح nodeoff, ساختار regmap **out_rmap)
{
	صحیح len;
	ثابت fdt32_t *val;

	اگر (!fdt || (nodeoff < 0) || !out_rmap)
		بازگشت SBI_EINVAL;

	val = fdt_getprop(fdt, nodeoff, "regmap", &len);
	اگر (!val)
		بازگشت SBI_ENOENT;

	بازگشت fdt_regmap_get_by_phandle(fdt, fdt32_to_cpu(*val), out_rmap);
}
