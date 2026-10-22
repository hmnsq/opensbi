/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#અન્તર્ભાવ <libfdt.h>
#અન્તર્ભાવ <sbi/sbi_error.h>
#અન્તર્ભાવ <sbi_utils/fdt/fdt_helper.h>
#અન્તર્ભાવ <sbi_utils/regmap/fdt_regmap.h>

/* List of FDT regmap drivers generated at compile time */
બાહ્ય અચલ સંરચના fdt_driver *અચલ fdt_regmap_drivers[];

સ્થિર પૂર્ણાંક fdt_regmap_find(અચલ શૂન્ય *fdt, પૂર્ણાંક nodeoff,
			   સંરચના regmap **out_rmap)
{
	પૂર્ણાંક rc;
	સંરચના regmap *rmap = regmap_find(nodeoff);

	જો (!rmap) {
		/* Regmap not found so initialize matching driver */
		rc = fdt_driver_init_by_offset(fdt, nodeoff,
					       fdt_regmap_drivers);
		જો (rc)
			પાછા rc;

		/* Try to find regmap again */
		rmap = regmap_find(nodeoff);
		જો (!rmap)
			પાછા SBI_ENOSYS;
	}

	જો (out_rmap)
		*out_rmap = rmap;

	પાછા 0;
}

પૂર્ણાંક fdt_regmap_get_by_phandle(અચલ શૂન્ય *fdt, u32 phandle,
			      સંરચના regmap **out_rmap)
{
	પૂર્ણાંક pnodeoff;

	જો (!fdt || !out_rmap)
		પાછા SBI_EINVAL;

	pnodeoff = fdt_ગાંઠ_offset_by_phandle(fdt, phandle);
	જો (pnodeoff < 0)
		પાછા pnodeoff;

	પાછા fdt_regmap_find(fdt, pnodeoff, out_rmap);
}

પૂર્ણાંક fdt_regmap_get(અચલ શૂન્ય *fdt, પૂર્ણાંક nodeoff, સંરચના regmap **out_rmap)
{
	પૂર્ણાંક len;
	અચલ fdt32_t *val;

	જો (!fdt || (nodeoff < 0) || !out_rmap)
		પાછા SBI_EINVAL;

	val = fdt_getprop(fdt, nodeoff, "regmap", &len);
	જો (!val)
		પાછા SBI_ENOENT;

	પાછા fdt_regmap_get_by_phandle(fdt, fdt32_to_cpu(*val), out_rmap);
}
