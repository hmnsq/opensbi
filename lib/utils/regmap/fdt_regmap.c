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

ساكن صحيح fdt_regmap_پیدا_کن(ثابت فراغ *fdt, صحيح nodeoff,
			   بنية regmap **بیرون_rmap)
{
	صحيح rc;
	بنية regmap *rmap = regmap_پیدا_کن(nodeoff);

	إذا (!rmap) {
		/* Regmap not found so initialize matching driver */
		rc = fdt_driver_init_by_جابه_جایی(fdt, nodeoff,
					       fdt_regmap_drivers);
		إذا (rc)
			أرجع rc;

		/* Try to find regmap again */
		rmap = regmap_پیدا_کن(nodeoff);
		إذا (!rmap)
			أرجع SBI_ENOSYS;
	}

	إذا (بیرون_rmap)
		*بیرون_rmap = rmap;

	أرجع 0;
}

صحيح fdt_regmap_get_by_phandle(ثابت فراغ *fdt, u32 phandle,
			      بنية regmap **بیرون_rmap)
{
	صحيح pnodeoff;

	إذا (!fdt || !بیرون_rmap)
		أرجع SBI_EINVAL;

	pnodeoff = fdt_گره_جابه_جایی_by_phandle(fdt, phandle);
	إذا (pnodeoff < 0)
		أرجع pnodeoff;

	أرجع fdt_regmap_پیدا_کن(fdt, pnodeoff, بیرون_rmap);
}

صحيح fdt_regmap_get(ثابت فراغ *fdt, صحيح nodeoff, بنية regmap **بیرون_rmap)
{
	صحيح len;
	ثابت fdt32_t *val;

	إذا (!fdt || (nodeoff < 0) || !بیرون_rmap)
		أرجع SBI_EINVAL;

	val = fdt_getprop(fdt, nodeoff, "regmap", &len);
	إذا (!val)
		أرجع SBI_ENOENT;

	أرجع fdt_regmap_get_by_phandle(fdt, fdt32_مقصد_cpu(*val), بیرون_rmap);
}
