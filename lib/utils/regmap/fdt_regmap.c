/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <libfdt.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_error.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/fdt/fdt_helper.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/regmap/fdt_regmap.h>

/* List of FDT regmap drivers generated at compile time */
ਬਾਹਰੀ ਅਚਲ ਬਣਤਰ fdt_driver *ਅਚਲ fdt_regmap_drivers[];

ਸਥਿਰ ਪੂਰਨ fdt_regmap_ਲੱਭੋ(ਅਚਲ ਖਾਲੀ *fdt, ਪੂਰਨ nodeoff,
			   ਬਣਤਰ regmap **ਬਾਹਰਲਾ_rmap)
{
	ਪੂਰਨ rc;
	ਬਣਤਰ regmap *rmap = regmap_ਲੱਭੋ(nodeoff);

	ਜੇ (!rmap) {
		/* Regmap not found so initialize matching driver */
		rc = fdt_driver_init_by_ਵਿੱਥ(fdt, nodeoff,
					       fdt_regmap_drivers);
		ਜੇ (rc)
			ਵਾਪਸ rc;

		/* Try to find regmap again */
		rmap = regmap_ਲੱਭੋ(nodeoff);
		ਜੇ (!rmap)
			ਵਾਪਸ SBI_ENOSYS;
	}

	ਜੇ (ਬਾਹਰਲਾ_rmap)
		*ਬਾਹਰਲਾ_rmap = rmap;

	ਵਾਪਸ 0;
}

ਪੂਰਨ fdt_regmap_get_by_phandle(ਅਚਲ ਖਾਲੀ *fdt, u32 phandle,
			      ਬਣਤਰ regmap **ਬਾਹਰਲਾ_rmap)
{
	ਪੂਰਨ pnodeoff;

	ਜੇ (!fdt || !ਬਾਹਰਲਾ_rmap)
		ਵਾਪਸ SBI_EINVAL;

	pnodeoff = fdt_ਗੰਢ_ਵਿੱਥ_by_phandle(fdt, phandle);
	ਜੇ (pnodeoff < 0)
		ਵਾਪਸ pnodeoff;

	ਵਾਪਸ fdt_regmap_ਲੱਭੋ(fdt, pnodeoff, ਬਾਹਰਲਾ_rmap);
}

ਪੂਰਨ fdt_regmap_get(ਅਚਲ ਖਾਲੀ *fdt, ਪੂਰਨ nodeoff, ਬਣਤਰ regmap **ਬਾਹਰਲਾ_rmap)
{
	ਪੂਰਨ len;
	ਅਚਲ fdt32_t *val;

	ਜੇ (!fdt || (nodeoff < 0) || !ਬਾਹਰਲਾ_rmap)
		ਵਾਪਸ SBI_EINVAL;

	val = fdt_getprop(fdt, nodeoff, "regmap", &len);
	ਜੇ (!val)
		ਵਾਪਸ SBI_ENOENT;

	ਵਾਪਸ fdt_regmap_get_by_phandle(fdt, fdt32_ਮੰਜ਼ਿਲ_cpu(*val), ਬਾਹਰਲਾ_rmap);
}
