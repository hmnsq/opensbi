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

static จำนวนเต็ม fdt_regmap_find(const void *fdt, จำนวนเต็ม nodeoff,
			   sa_nracanaa regmap **out_rmap)
{
	จำนวนเต็ม rc;
	sa_nracanaa regmap *rmap = regmap_find(nodeoff);

	ถ้า (!rmap) {
		/* Regmap not found so initialize matching driver */
		rc = fdt_driver_init_by_offset(fdt, nodeoff,
					       fdt_regmap_drivers);
		ถ้า (rc)
			คืน rc;

		/* Try to find regmap again */
		rmap = regmap_find(nodeoff);
		ถ้า (!rmap)
			คืน SBI_ENOSYS;
	}

	ถ้า (out_rmap)
		*out_rmap = rmap;

	คืน 0;
}

จำนวนเต็ม fdt_regmap_get_by_phandle(const void *fdt, u32 phandle,
			      sa_nracanaa regmap **out_rmap)
{
	จำนวนเต็ม pnodeoff;

	ถ้า (!fdt || !out_rmap)
		คืน SBI_EINVAL;

	pnodeoff = fdt_node_offset_by_phandle(fdt, phandle);
	ถ้า (pnodeoff < 0)
		คืน pnodeoff;

	คืน fdt_regmap_find(fdt, pnodeoff, out_rmap);
}

จำนวนเต็ม fdt_regmap_get(const void *fdt, จำนวนเต็ม nodeoff, sa_nracanaa regmap **out_rmap)
{
	จำนวนเต็ม len;
	const fdt32_t *val;

	ถ้า (!fdt || (nodeoff < 0) || !out_rmap)
		คืน SBI_EINVAL;

	val = fdt_getprop(fdt, nodeoff, "regmap", &len);
	ถ้า (!val)
		คืน SBI_ENOENT;

	คืน fdt_regmap_get_by_phandle(fdt, fdt32_to_cpu(*val), out_rmap);
}
