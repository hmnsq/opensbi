// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2014 David Gibson <david@gibson.dropbear.id.au>
 * Copyright (C) 2018 embedded brains GmbH
 */
#అన్తర్భావ "libfdt_env.h"

#అన్తర్భావ <fdt.h>
#అన్తర్భావ <libfdt.h>

#అన్తర్భావ "libfdt_internal.h"

స్థిర పూర్ణాంకం fdt_cells(స్థిరరాశి శూన్య *fdt, పూర్ణాంకం nodeoffset, స్థిరరాశి అక్షరం *name)
{
	స్థిరరాశి fdt32_t *c;
	uint32_t val;
	పూర్ణాంకం len;

	c = fdt_getprop(fdt, nodeoffset, name, &len);
	ఒకవేళ (!c)
		తిరిగి len;

	ఒకవేళ (len != పరిమాణం(*c))
		తిరిగి -FDT_ERR_BADNCELLS;

	val = fdt32_గమ్యం_cpu(*c);
	ఒకవేళ (val > FDT_MAX_NCELLS)
		తిరిగి -FDT_ERR_BADNCELLS;

	తిరిగి (పూర్ణాంకం)val;
}

పూర్ణాంకం fdt_చిరునామా_cells(స్థిరరాశి శూన్య *fdt, పూర్ణాంకం nodeoffset)
{
	పూర్ణాంకం val;

	val = fdt_cells(fdt, nodeoffset, "#address-cells");
	ఒకవేళ (val == 0)
		తిరిగి -FDT_ERR_BADNCELLS;
	ఒకవేళ (val == -FDT_ERR_NOTFOUND)
		తిరిగి 2;
	తిరిగి val;
}

పూర్ణాంకం fdt_కొలత_cells(స్థిరరాశి శూన్య *fdt, పూర్ణాంకం nodeoffset)
{
	పూర్ణాంకం val;

	val = fdt_cells(fdt, nodeoffset, "#size-cells");
	ఒకవేళ (val == -FDT_ERR_NOTFOUND)
		తిరిగి 1;
	తిరిగి val;
}

/* This function assumes that [address|size]_cells is 1 or 2 */
పూర్ణాంకం fdt_appendprop_addrrange(శూన్య *fdt, పూర్ణాంకం మాతృ, పూర్ణాంకం nodeoffset,
			     స్థిరరాశి అక్షరం *name, uint64_t addr, uint64_t కొలత)
{
	పూర్ణాంకం addr_cells, కొలత_cells, ret;
	uint8_t దత్తాంశం[పరిమాణం(fdt64_t) * 2], *prop;

	ret = fdt_చిరునామా_cells(fdt, మాతృ);
	ఒకవేళ (ret < 0)
		తిరిగి ret;
	addr_cells = ret;

	ret = fdt_కొలత_cells(fdt, మాతృ);
	ఒకవేళ (ret < 0)
		తిరిగి ret;
	కొలత_cells = ret;

	/* check validity of address */
	prop = దత్తాంశం;
	ఒకవేళ (addr_cells == 1) {
		ఒకవేళ ((addr > UINT32_MAX) || ((UINT32_MAX + 1 - addr) < కొలత))
			తిరిగి -FDT_ERR_BADVALUE;

		fdt32_st(prop, (uint32_t)addr);
	} లేకపోతే ఒకవేళ (addr_cells == 2) {
		fdt64_st(prop, addr);
	} లేకపోతే {
		తిరిగి -FDT_ERR_BADNCELLS;
	}

	/* check validity of size */
	prop += addr_cells * పరిమాణం(fdt32_t);
	ఒకవేళ (కొలత_cells == 1) {
		ఒకవేళ (కొలత > UINT32_MAX)
			తిరిగి -FDT_ERR_BADVALUE;

		fdt32_st(prop, (uint32_t)కొలత);
	} లేకపోతే ఒకవేళ (కొలత_cells == 2) {
		fdt64_st(prop, కొలత);
	} లేకపోతే {
		తిరిగి -FDT_ERR_BADNCELLS;
	}

	తిరిగి fdt_appendprop(fdt, nodeoffset, name, దత్తాంశం,
			      (addr_cells + కొలత_cells) * పరిమాణం(fdt32_t));
}
