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
बाह्य ध्रुव संरचना fdt_driver *ध्रुव fdt_regmap_drivers[];

स्थिर पूर्णाङ्क fdt_regmap_find(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoff,
			   संरचना regmap **out_rmap)
{
	पूर्णाङ्क rc;
	संरचना regmap *rmap = regmap_find(nodeoff);

	यदि (!rmap) {
		/* Regmap not found so initialize matching driver */
		rc = fdt_driver_init_by_offset(fdt, nodeoff,
					       fdt_regmap_drivers);
		यदि (rc)
			प्रत्यागम rc;

		/* Try to find regmap again */
		rmap = regmap_find(nodeoff);
		यदि (!rmap)
			प्रत्यागम SBI_ENOSYS;
	}

	यदि (out_rmap)
		*out_rmap = rmap;

	प्रत्यागम 0;
}

पूर्णाङ्क fdt_regmap_get_by_phandle(ध्रुव शून्य *fdt, u32 phandle,
			      संरचना regmap **out_rmap)
{
	पूर्णाङ्क pnodeoff;

	यदि (!fdt || !out_rmap)
		प्रत्यागम SBI_EINVAL;

	pnodeoff = fdt_गाठ_offset_by_phandle(fdt, phandle);
	यदि (pnodeoff < 0)
		प्रत्यागम pnodeoff;

	प्रत्यागम fdt_regmap_find(fdt, pnodeoff, out_rmap);
}

पूर्णाङ्क fdt_regmap_get(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoff, संरचना regmap **out_rmap)
{
	पूर्णाङ्क len;
	ध्रुव fdt32_t *val;

	यदि (!fdt || (nodeoff < 0) || !out_rmap)
		प्रत्यागम SBI_EINVAL;

	val = fdt_getprop(fdt, nodeoff, "regmap", &len);
	यदि (!val)
		प्रत्यागम SBI_ENOENT;

	प्रत्यागम fdt_regmap_get_by_phandle(fdt, fdt32_to_cpu(*val), out_rmap);
}
