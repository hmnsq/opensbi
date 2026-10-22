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

static ຈຳນວນເຕັມ fdt_regmap_find(const void *fdt, ຈຳນວນເຕັມ nodeoff,
			   sa_nracanaa regmap **out_rmap)
{
	ຈຳນວນເຕັມ rc;
	sa_nracanaa regmap *rmap = regmap_find(nodeoff);

	ຖ້າ (!rmap) {
		/* Regmap not found so initialize matching driver */
		rc = fdt_driver_init_by_offset(fdt, nodeoff,
					       fdt_regmap_drivers);
		ຖ້າ (rc)
			ສົ່ງຄືນ rc;

		/* Try to find regmap again */
		rmap = regmap_find(nodeoff);
		ຖ້າ (!rmap)
			ສົ່ງຄືນ SBI_ENOSYS;
	}

	ຖ້າ (out_rmap)
		*out_rmap = rmap;

	ສົ່ງຄືນ 0;
}

ຈຳນວນເຕັມ fdt_regmap_get_by_phandle(const void *fdt, u32 phandle,
			      sa_nracanaa regmap **out_rmap)
{
	ຈຳນວນເຕັມ pnodeoff;

	ຖ້າ (!fdt || !out_rmap)
		ສົ່ງຄືນ SBI_EINVAL;

	pnodeoff = fdt_node_offset_by_phandle(fdt, phandle);
	ຖ້າ (pnodeoff < 0)
		ສົ່ງຄືນ pnodeoff;

	ສົ່ງຄືນ fdt_regmap_find(fdt, pnodeoff, out_rmap);
}

ຈຳນວນເຕັມ fdt_regmap_get(const void *fdt, ຈຳນວນເຕັມ nodeoff, sa_nracanaa regmap **out_rmap)
{
	ຈຳນວນເຕັມ len;
	const fdt32_t *val;

	ຖ້າ (!fdt || (nodeoff < 0) || !out_rmap)
		ສົ່ງຄືນ SBI_EINVAL;

	val = fdt_getprop(fdt, nodeoff, "regmap", &len);
	ຖ້າ (!val)
		ສົ່ງຄືນ SBI_ENOENT;

	ສົ່ງຄືນ fdt_regmap_get_by_phandle(fdt, fdt32_to_cpu(*val), out_rmap);
}
