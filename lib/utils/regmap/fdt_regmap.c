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
خارجی ثابت ساخت fdt_driver *ثابت fdt_regmap_drivers[];

ساکن صحیح fdt_regmap_ڈھونڈو(ثابت خالی *fdt, صحیح nodeoff,
			   ساخت regmap **برآمد_rmap)
{
	صحیح rc;
	ساخت regmap *rmap = regmap_ڈھونڈو(nodeoff);

	اگر (!rmap) {
		/* Regmap not found so initialize matching driver */
		rc = fdt_driver_init_by_انحراف(fdt, nodeoff,
					       fdt_regmap_drivers);
		اگر (rc)
			واپس rc;

		/* Try to find regmap again */
		rmap = regmap_ڈھونڈو(nodeoff);
		اگر (!rmap)
			واپس SBI_ENOSYS;
	}

	اگر (برآمد_rmap)
		*برآمد_rmap = rmap;

	واپس 0;
}

صحیح fdt_regmap_get_by_phandle(ثابت خالی *fdt, u32 phandle,
			      ساخت regmap **برآمد_rmap)
{
	صحیح pnodeoff;

	اگر (!fdt || !برآمد_rmap)
		واپس SBI_EINVAL;

	pnodeoff = fdt_گرہ_انحراف_by_phandle(fdt, phandle);
	اگر (pnodeoff < 0)
		واپس pnodeoff;

	واپس fdt_regmap_ڈھونڈو(fdt, pnodeoff, برآمد_rmap);
}

صحیح fdt_regmap_get(ثابت خالی *fdt, صحیح nodeoff, ساخت regmap **برآمد_rmap)
{
	صحیح len;
	ثابت fdt32_t *val;

	اگر (!fdt || (nodeoff < 0) || !برآمد_rmap)
		واپس SBI_EINVAL;

	val = fdt_getprop(fdt, nodeoff, "regmap", &len);
	اگر (!val)
		واپس SBI_ENOENT;

	واپس fdt_regmap_get_by_phandle(fdt, fdt32_مقصد_cpu(*val), برآمد_rmap);
}
