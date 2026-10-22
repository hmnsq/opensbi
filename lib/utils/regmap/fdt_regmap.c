/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#అన్తర్భావ <libfdt.h>
#అన్తర్భావ <sbi/sbi_error.h>
#అన్తర్భావ <sbi_utils/fdt/fdt_helper.h>
#అన్తర్భావ <sbi_utils/regmap/fdt_regmap.h>

/* List of FDT regmap drivers generated at compile time */
బాహ్య స్థిరరాశి నిర్మాణం fdt_driver *స్థిరరాశి fdt_regmap_drivers[];

స్థిర పూర్ణాంకం fdt_regmap_కనుగొను(స్థిరరాశి శూన్య *fdt, పూర్ణాంకం nodeoff,
			   నిర్మాణం regmap **బహిర్గతం_rmap)
{
	పూర్ణాంకం rc;
	నిర్మాణం regmap *rmap = regmap_కనుగొను(nodeoff);

	ఒకవేళ (!rmap) {
		/* Regmap not found so initialize matching driver */
		rc = fdt_driver_init_by_స్థానభ్రంశం(fdt, nodeoff,
					       fdt_regmap_drivers);
		ఒకవేళ (rc)
			తిరిగి rc;

		/* Try to find regmap again */
		rmap = regmap_కనుగొను(nodeoff);
		ఒకవేళ (!rmap)
			తిరిగి SBI_ENOSYS;
	}

	ఒకవేళ (బహిర్గతం_rmap)
		*బహిర్గతం_rmap = rmap;

	తిరిగి 0;
}

పూర్ణాంకం fdt_regmap_get_by_phandle(స్థిరరాశి శూన్య *fdt, u32 phandle,
			      నిర్మాణం regmap **బహిర్గతం_rmap)
{
	పూర్ణాంకం pnodeoff;

	ఒకవేళ (!fdt || !బహిర్గతం_rmap)
		తిరిగి SBI_EINVAL;

	pnodeoff = fdt_కణుపు_స్థానభ్రంశం_by_phandle(fdt, phandle);
	ఒకవేళ (pnodeoff < 0)
		తిరిగి pnodeoff;

	తిరిగి fdt_regmap_కనుగొను(fdt, pnodeoff, బహిర్గతం_rmap);
}

పూర్ణాంకం fdt_regmap_get(స్థిరరాశి శూన్య *fdt, పూర్ణాంకం nodeoff, నిర్మాణం regmap **బహిర్గతం_rmap)
{
	పూర్ణాంకం len;
	స్థిరరాశి fdt32_t *val;

	ఒకవేళ (!fdt || (nodeoff < 0) || !బహిర్గతం_rmap)
		తిరిగి SBI_EINVAL;

	val = fdt_getprop(fdt, nodeoff, "regmap", &len);
	ఒకవేళ (!val)
		తిరిగి SBI_ENOENT;

	తిరిగి fdt_regmap_get_by_phandle(fdt, fdt32_గమ్యం_cpu(*val), బహిర్గతం_rmap);
}
