/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#अन्तर्भाव <libfdt.h>
#अन्तर्भाव <sbi/sbi_error.h>
#अन्तर्भाव <sbi_utils/fdt/fdt_helper.h>
#अन्तर्भाव <sbi_utils/regmap/fdt_regmap.h>

/* List of FDT regmap drivers generated at compile time */
बाह्य स्थिरांक संरचना fdt_driver *स्थिरांक fdt_regmap_drivers[];

स्थिर पूर्णांक fdt_regmap_find(स्थिरांक शून्य *fdt, पूर्णांक nodeoff,
			   संरचना regmap **out_rmap)
{
	पूर्णांक rc;
	संरचना regmap *rmap = regmap_find(nodeoff);

	जर (!rmap) {
		/* Regmap not found so initialize matching driver */
		rc = fdt_driver_init_by_offset(fdt, nodeoff,
					       fdt_regmap_drivers);
		जर (rc)
			परत rc;

		/* Try to find regmap again */
		rmap = regmap_find(nodeoff);
		जर (!rmap)
			परत SBI_ENOSYS;
	}

	जर (out_rmap)
		*out_rmap = rmap;

	परत 0;
}

पूर्णांक fdt_regmap_get_by_phandle(स्थिरांक शून्य *fdt, u32 phandle,
			      संरचना regmap **out_rmap)
{
	पूर्णांक pnodeoff;

	जर (!fdt || !out_rmap)
		परत SBI_EINVAL;

	pnodeoff = fdt_गाठ_offset_by_phandle(fdt, phandle);
	जर (pnodeoff < 0)
		परत pnodeoff;

	परत fdt_regmap_find(fdt, pnodeoff, out_rmap);
}

पूर्णांक fdt_regmap_get(स्थिरांक शून्य *fdt, पूर्णांक nodeoff, संरचना regmap **out_rmap)
{
	पूर्णांक len;
	स्थिरांक fdt32_t *val;

	जर (!fdt || (nodeoff < 0) || !out_rmap)
		परत SBI_EINVAL;

	val = fdt_getprop(fdt, nodeoff, "regmap", &len);
	जर (!val)
		परत SBI_ENOENT;

	परत fdt_regmap_get_by_phandle(fdt, fdt32_to_cpu(*val), out_rmap);
}
