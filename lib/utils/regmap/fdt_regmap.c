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

	ᱡᱩᱫᱤ (!rmap) {
		/* Regmap not found so initialize matching driver */
		rc = fdt_driver_init_by_offset(fdt, nodeoff,
					       fdt_regmap_drivers);
		ᱡᱩᱫᱤ (rc)
			ᱨᱩᱣᱟᱹᱲ rc;

		/* Try to find regmap again */
		rmap = regmap_find(nodeoff);
		ᱡᱩᱫᱤ (!rmap)
			ᱨᱩᱣᱟᱹᱲ SBI_ENOSYS;
	}

	ᱡᱩᱫᱤ (out_rmap)
		*out_rmap = rmap;

	ᱨᱩᱣᱟᱹᱲ 0;
}

puur_naa_nka fdt_regmap_get_by_phandle(const void *fdt, u32 phandle,
			      sa_nracanaa regmap **out_rmap)
{
	puur_naa_nka pnodeoff;

	ᱡᱩᱫᱤ (!fdt || !out_rmap)
		ᱨᱩᱣᱟᱹᱲ SBI_EINVAL;

	pnodeoff = fdt_node_offset_by_phandle(fdt, phandle);
	ᱡᱩᱫᱤ (pnodeoff < 0)
		ᱨᱩᱣᱟᱹᱲ pnodeoff;

	ᱨᱩᱣᱟᱹᱲ fdt_regmap_find(fdt, pnodeoff, out_rmap);
}

puur_naa_nka fdt_regmap_get(const void *fdt, puur_naa_nka nodeoff, sa_nracanaa regmap **out_rmap)
{
	puur_naa_nka len;
	const fdt32_t *val;

	ᱡᱩᱫᱤ (!fdt || (nodeoff < 0) || !out_rmap)
		ᱨᱩᱣᱟᱹᱲ SBI_EINVAL;

	val = fdt_getprop(fdt, nodeoff, "regmap", &len);
	ᱡᱩᱫᱤ (!val)
		ᱨᱩᱣᱟᱹᱲ SBI_ENOENT;

	ᱨᱩᱣᱟᱹᱲ fdt_regmap_get_by_phandle(fdt, fdt32_to_cpu(*val), out_rmap);
}
