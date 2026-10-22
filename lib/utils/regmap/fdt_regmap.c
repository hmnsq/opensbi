/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#ಅನ್ತರ್ಭಾವ <libfdt.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_error.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/fdt/fdt_helper.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/regmap/fdt_regmap.h>

/* List of FDT regmap drivers generated at compile time */
ಬಾಹ್ಯ ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_driver *ಸ್ಥಿರಾಂಕ fdt_regmap_drivers[];

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ fdt_regmap_ಕಂಡುಹಿಡಿ(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ nodeoff,
			   ರಚನೆ regmap **ಹೊರಹೋಗು_rmap)
{
	ಪೂರ್ಣಾಂಕ rc;
	ರಚನೆ regmap *rmap = regmap_ಕಂಡುಹಿಡಿ(nodeoff);

	ಆದರೆ (!rmap) {
		/* Regmap not found so initialize matching driver */
		rc = fdt_driver_init_by_ಸ್ಥಾನಾಂತರ(fdt, nodeoff,
					       fdt_regmap_drivers);
		ಆದರೆ (rc)
			ಹಿಂತಿರುಗಿ rc;

		/* Try to find regmap again */
		rmap = regmap_ಕಂಡುಹಿಡಿ(nodeoff);
		ಆದರೆ (!rmap)
			ಹಿಂತಿರುಗಿ SBI_ENOSYS;
	}

	ಆದರೆ (ಹೊರಹೋಗು_rmap)
		*ಹೊರಹೋಗು_rmap = rmap;

	ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ fdt_regmap_get_by_phandle(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, u32 phandle,
			      ರಚನೆ regmap **ಹೊರಹೋಗು_rmap)
{
	ಪೂರ್ಣಾಂಕ pnodeoff;

	ಆದರೆ (!fdt || !ಹೊರಹೋಗು_rmap)
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	pnodeoff = fdt_ಗಂಟು_ಸ್ಥಾನಾಂತರ_by_phandle(fdt, phandle);
	ಆದರೆ (pnodeoff < 0)
		ಹಿಂತಿರುಗಿ pnodeoff;

	ಹಿಂತಿರುಗಿ fdt_regmap_ಕಂಡುಹಿಡಿ(fdt, pnodeoff, ಹೊರಹೋಗು_rmap);
}

ಪೂರ್ಣಾಂಕ fdt_regmap_get(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ nodeoff, ರಚನೆ regmap **ಹೊರಹೋಗು_rmap)
{
	ಪೂರ್ಣಾಂಕ len;
	ಸ್ಥಿರಾಂಕ fdt32_t *val;

	ಆದರೆ (!fdt || (nodeoff < 0) || !ಹೊರಹೋಗು_rmap)
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	val = fdt_getprop(fdt, nodeoff, "regmap", &len);
	ಆದರೆ (!val)
		ಹಿಂತಿರುಗಿ SBI_ENOENT;

	ಹಿಂತಿರುಗಿ fdt_regmap_get_by_phandle(fdt, fdt32_ಗಮ್ಯ_cpu(*val), ಹೊರಹೋಗು_rmap);
}
