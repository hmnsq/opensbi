/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#समावेश <libfdt.h>
#समावेश <sbi/sbi_error.h>
#समावेश <sbi_utils/fdt/fdt_helper.h>
#समावेश <sbi_utils/regmap/fdt_regmap.h>

/* List of FDT regmap drivers generated at compile time */
बाह्य स्थिर काष्ठा fdt_driver *स्थिर fdt_regmap_drivers[];

अटल पूर्णांक fdt_regmap_find(स्थिर व्योम *fdt, पूर्णांक nodeoff,
			   काष्ठा regmap **out_rmap)
{
	पूर्णांक rc;
	काष्ठा regmap *rmap = regmap_find(nodeoff);

	अगर (!rmap) {
		/* Regmap not found so initialize matching driver */
		rc = fdt_driver_init_by_offset(fdt, nodeoff,
					       fdt_regmap_drivers);
		अगर (rc)
			वापस rc;

		/* Try to find regmap again */
		rmap = regmap_find(nodeoff);
		अगर (!rmap)
			वापस SBI_ENOSYS;
	}

	अगर (out_rmap)
		*out_rmap = rmap;

	वापस 0;
}

पूर्णांक fdt_regmap_get_by_phandle(स्थिर व्योम *fdt, u32 phandle,
			      काष्ठा regmap **out_rmap)
{
	पूर्णांक pnodeoff;

	अगर (!fdt || !out_rmap)
		वापस SBI_EINVAL;

	pnodeoff = fdt_गाँठ_offset_by_phandle(fdt, phandle);
	अगर (pnodeoff < 0)
		वापस pnodeoff;

	वापस fdt_regmap_find(fdt, pnodeoff, out_rmap);
}

पूर्णांक fdt_regmap_get(स्थिर व्योम *fdt, पूर्णांक nodeoff, काष्ठा regmap **out_rmap)
{
	पूर्णांक len;
	स्थिर fdt32_t *val;

	अगर (!fdt || (nodeoff < 0) || !out_rmap)
		वापस SBI_EINVAL;

	val = fdt_getprop(fdt, nodeoff, "regmap", &len);
	अगर (!val)
		वापस SBI_ENOENT;

	वापस fdt_regmap_get_by_phandle(fdt, fdt32_to_cpu(*val), out_rmap);
}
