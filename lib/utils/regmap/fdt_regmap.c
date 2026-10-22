/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#അന്തര്ഭാവ <libfdt.h>
#അന്തര്ഭാവ <sbi/sbi_error.h>
#അന്തര്ഭാവ <sbi_utils/fdt/fdt_helper.h>
#അന്തര്ഭാവ <sbi_utils/regmap/fdt_regmap.h>

/* List of FDT regmap drivers generated at compile time */
ബാഹ്യ സ്ഥിരം ഘടന fdt_driver *സ്ഥിരം fdt_regmap_drivers[];

സ്ഥിതം പൂർണ്ണസംഖ്യ fdt_regmap_find(സ്ഥിരം ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ nodeoff,
			   ഘടന regmap **out_rmap)
{
	പൂർണ്ണസംഖ്യ rc;
	ഘടന regmap *rmap = regmap_find(nodeoff);

	എങ്കിൽ (!rmap) {
		/* Regmap not found so initialize matching driver */
		rc = fdt_driver_init_by_offset(fdt, nodeoff,
					       fdt_regmap_drivers);
		എങ്കിൽ (rc)
			തിരികെ rc;

		/* Try to find regmap again */
		rmap = regmap_find(nodeoff);
		എങ്കിൽ (!rmap)
			തിരികെ SBI_ENOSYS;
	}

	എങ്കിൽ (out_rmap)
		*out_rmap = rmap;

	തിരികെ 0;
}

പൂർണ്ണസംഖ്യ fdt_regmap_get_by_phandle(സ്ഥിരം ശൂന്യം *fdt, u32 phandle,
			      ഘടന regmap **out_rmap)
{
	പൂർണ്ണസംഖ്യ pnodeoff;

	എങ്കിൽ (!fdt || !out_rmap)
		തിരികെ SBI_EINVAL;

	pnodeoff = fdt_കണ്ണി_offset_by_phandle(fdt, phandle);
	എങ്കിൽ (pnodeoff < 0)
		തിരികെ pnodeoff;

	തിരികെ fdt_regmap_find(fdt, pnodeoff, out_rmap);
}

പൂർണ്ണസംഖ്യ fdt_regmap_get(സ്ഥിരം ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ nodeoff, ഘടന regmap **out_rmap)
{
	പൂർണ്ണസംഖ്യ len;
	സ്ഥിരം fdt32_t *val;

	എങ്കിൽ (!fdt || (nodeoff < 0) || !out_rmap)
		തിരികെ SBI_EINVAL;

	val = fdt_getprop(fdt, nodeoff, "regmap", &len);
	എങ്കിൽ (!val)
		തിരികെ SBI_ENOENT;

	തിരികെ fdt_regmap_get_by_phandle(fdt, fdt32_to_cpu(*val), out_rmap);
}
