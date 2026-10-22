// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2014 David Gibson <david@gibson.dropbear.id.au>
 * Copyright (C) 2018 embedded brains GmbH
 */
#ଅନ୍ତର୍ଭାଵ "libfdt_env.h"

#ଅନ୍ତର୍ଭାଵ <fdt.h>
#ଅନ୍ତର୍ଭାଵ <libfdt.h>

#ଅନ୍ତର୍ଭାଵ "libfdt_internal.h"

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_cells(ଅଚଳ ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nodeoffset, ଅଚଳ ଅକ୍ଷର *name)
{
	ଅଚଳ fdt32_t *c;
	uint32_t val;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା len;

	c = fdt_getprop(fdt, nodeoffset, name, &len);
	ଯଦି (!c)
		ଫେରନ୍ତୁ len;

	ଯଦି (len != ଆକାର(*c))
		ଫେରନ୍ତୁ -FDT_ERR_BADNCELLS;

	val = fdt32_to_cpu(*c);
	ଯଦି (val > FDT_MAX_NCELLS)
		ଫେରନ୍ତୁ -FDT_ERR_BADNCELLS;

	ଫେରନ୍ତୁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)val;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_address_cells(ଅଚଳ ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nodeoffset)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା val;

	val = fdt_cells(fdt, nodeoffset, "#address-cells");
	ଯଦି (val == 0)
		ଫେରନ୍ତୁ -FDT_ERR_BADNCELLS;
	ଯଦି (val == -FDT_ERR_NOTFOUND)
		ଫେରନ୍ତୁ 2;
	ଫେରନ୍ତୁ val;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_size_cells(ଅଚଳ ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nodeoffset)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା val;

	val = fdt_cells(fdt, nodeoffset, "#size-cells");
	ଯଦି (val == -FDT_ERR_NOTFOUND)
		ଫେରନ୍ତୁ 1;
	ଫେରନ୍ତୁ val;
}

/* This function assumes that [address|size]_cells is 1 or 2 */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_appendprop_addrrange(ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା parent, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nodeoffset,
			     ଅଚଳ ଅକ୍ଷର *name, uint64_t addr, uint64_t size)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା addr_cells, size_cells, ret;
	uint8_t data[ଆକାର(fdt64_t) * 2], *prop;

	ret = fdt_address_cells(fdt, parent);
	ଯଦି (ret < 0)
		ଫେରନ୍ତୁ ret;
	addr_cells = ret;

	ret = fdt_size_cells(fdt, parent);
	ଯଦି (ret < 0)
		ଫେରନ୍ତୁ ret;
	size_cells = ret;

	/* check validity of address */
	prop = data;
	ଯଦି (addr_cells == 1) {
		ଯଦି ((addr > UINT32_MAX) || ((UINT32_MAX + 1 - addr) < size))
			ଫେରନ୍ତୁ -FDT_ERR_BADVALUE;

		fdt32_st(prop, (uint32_t)addr);
	} ଅନ୍ୟଥା ଯଦି (addr_cells == 2) {
		fdt64_st(prop, addr);
	} ଅନ୍ୟଥା {
		ଫେରନ୍ତୁ -FDT_ERR_BADNCELLS;
	}

	/* check validity of size */
	prop += addr_cells * ଆକାର(fdt32_t);
	ଯଦି (size_cells == 1) {
		ଯଦି (size > UINT32_MAX)
			ଫେରନ୍ତୁ -FDT_ERR_BADVALUE;

		fdt32_st(prop, (uint32_t)size);
	} ଅନ୍ୟଥା ଯଦି (size_cells == 2) {
		fdt64_st(prop, size);
	} ଅନ୍ୟଥା {
		ଫେରନ୍ତୁ -FDT_ERR_BADNCELLS;
	}

	ଫେରନ୍ତୁ fdt_appendprop(fdt, nodeoffset, name, data,
			      (addr_cells + size_cells) * ଆକାର(fdt32_t));
}
