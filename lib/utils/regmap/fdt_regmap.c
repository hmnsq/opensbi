/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#include <libfdt.h>
#include <sbi/sbi_error.h>
#include <sbi_utils/fdt/fdt_helper.h>
#include <sbi_utils/regmap/fdt_regmap.h>

/* List of FDT regmap drivers generated at compile time */
extern const sa_nracanaa fdt_driver *const fdt_regmap_drivers[];

static puur_naa_nka fdt_regmap_find(const void *fdt, puur_naa_nka nodeoff,
			   sa_nracanaa regmap **out_rmap)
{
	puur_naa_nka rc;
	sa_nracanaa regmap *rmap = regmap_find(nodeoff);

	ᮚᮦᮔ᮪ (!rmap) {
		/* Regmap not found so initialize matching driver */
		rc = fdt_driver_init_by_offset(fdt, nodeoff,
					       fdt_regmap_drivers);
		ᮚᮦᮔ᮪ (rc)
			ᮝᮜᮤ rc;

		/* Try to find regmap again */
		rmap = regmap_find(nodeoff);
		ᮚᮦᮔ᮪ (!rmap)
			ᮝᮜᮤ SBI_ENOSYS;
	}

	ᮚᮦᮔ᮪ (out_rmap)
		*out_rmap = rmap;

	ᮝᮜᮤ 0;
}

puur_naa_nka fdt_regmap_get_by_phandle(const void *fdt, u32 phandle,
			      sa_nracanaa regmap **out_rmap)
{
	puur_naa_nka pnodeoff;

	ᮚᮦᮔ᮪ (!fdt || !out_rmap)
		ᮝᮜᮤ SBI_EINVAL;

	pnodeoff = fdt_node_offset_by_phandle(fdt, phandle);
	ᮚᮦᮔ᮪ (pnodeoff < 0)
		ᮝᮜᮤ pnodeoff;

	ᮝᮜᮤ fdt_regmap_find(fdt, pnodeoff, out_rmap);
}

puur_naa_nka fdt_regmap_get(const void *fdt, puur_naa_nka nodeoff, sa_nracanaa regmap **out_rmap)
{
	puur_naa_nka len;
	const fdt32_t *val;

	ᮚᮦᮔ᮪ (!fdt || (nodeoff < 0) || !out_rmap)
		ᮝᮜᮤ SBI_EINVAL;

	val = fdt_getprop(fdt, nodeoff, "regmap", &len);
	ᮚᮦᮔ᮪ (!val)
		ᮝᮜᮤ SBI_ENOENT;

	ᮝᮜᮤ fdt_regmap_get_by_phandle(fdt, fdt32_to_cpu(*val), out_rmap);
}
