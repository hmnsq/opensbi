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
ಬಾಹ್ಯ ಧ್ರುವ ಸಂರಚನಾ fdt_driver *ಧ್ರುವ fdt_regmap_drivers[];

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ fdt_regmap_find(ಧ್ರುವ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ nodeoff,
			   ಸಂರಚನಾ regmap **out_rmap)
{
	ಪೂರ್ಣಾಙ್ಕ rc;
	ಸಂರಚನಾ regmap *rmap = regmap_find(nodeoff);

	ಯದಿ (!rmap) {
		/* Regmap not found so initialize matching driver */
		rc = fdt_driver_init_by_offset(fdt, nodeoff,
					       fdt_regmap_drivers);
		ಯದಿ (rc)
			ಪ್ರತ್ಯಾಗಮ rc;

		/* Try to find regmap again */
		rmap = regmap_find(nodeoff);
		ಯದಿ (!rmap)
			ಪ್ರತ್ಯಾಗಮ SBI_ENOSYS;
	}

	ಯದಿ (out_rmap)
		*out_rmap = rmap;

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ fdt_regmap_get_by_phandle(ಧ್ರುವ ಶೂನ್ಯ *fdt, u32 phandle,
			      ಸಂರಚನಾ regmap **out_rmap)
{
	ಪೂರ್ಣಾಙ್ಕ pnodeoff;

	ಯದಿ (!fdt || !out_rmap)
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;

	pnodeoff = fdt_ಗಂಟು_offset_by_phandle(fdt, phandle);
	ಯದಿ (pnodeoff < 0)
		ಪ್ರತ್ಯಾಗಮ pnodeoff;

	ಪ್ರತ್ಯಾಗಮ fdt_regmap_find(fdt, pnodeoff, out_rmap);
}

ಪೂರ್ಣಾಙ್ಕ fdt_regmap_get(ಧ್ರುವ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ nodeoff, ಸಂರಚನಾ regmap **out_rmap)
{
	ಪೂರ್ಣಾಙ್ಕ len;
	ಧ್ರುವ fdt32_t *val;

	ಯದಿ (!fdt || (nodeoff < 0) || !out_rmap)
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;

	val = fdt_getprop(fdt, nodeoff, "regmap", &len);
	ಯದಿ (!val)
		ಪ್ರತ್ಯಾಗಮ SBI_ENOENT;

	ಪ್ರತ್ಯಾಗಮ fdt_regmap_get_by_phandle(fdt, fdt32_to_cpu(*val), out_rmap);
}
