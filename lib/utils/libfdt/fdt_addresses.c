// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2014 David Gibson <david@gibson.dropbear.id.au>
 * Copyright (C) 2018 embedded brains GmbH
 */
#ضمن "libfdt_env.h"

#ضمن <fdt.h>
#ضمن <libfdt.h>

#ضمن "libfdt_internal.h"

ساكن صحيح fdt_cells(ثابت فراغ *fdt, صحيح nodeoffset, ثابت حرف *name)
{
	ثابت fdt32_t *c;
	uint32_t val;
	صحيح len;

	c = fdt_getprop(fdt, nodeoffset, name, &len);
	ܐܢ (!c)
		ܦܢܐ len;

	ܐܢ (len != حجم(*c))
		ܦܢܐ -FDT_ERR_BADNCELLS;

	val = fdt32_to_cpu(*c);
	ܐܢ (val > FDT_MAX_NCELLS)
		ܦܢܐ -FDT_ERR_BADNCELLS;

	ܦܢܐ (صحيح)val;
}

صحيح fdt_address_cells(ثابت فراغ *fdt, صحيح nodeoffset)
{
	صحيح val;

	val = fdt_cells(fdt, nodeoffset, "#address-cells");
	ܐܢ (val == 0)
		ܦܢܐ -FDT_ERR_BADNCELLS;
	ܐܢ (val == -FDT_ERR_NOTFOUND)
		ܦܢܐ 2;
	ܦܢܐ val;
}

صحيح fdt_size_cells(ثابت فراغ *fdt, صحيح nodeoffset)
{
	صحيح val;

	val = fdt_cells(fdt, nodeoffset, "#size-cells");
	ܐܢ (val == -FDT_ERR_NOTFOUND)
		ܦܢܐ 1;
	ܦܢܐ val;
}

/* This function assumes that [address|size]_cells is 1 or 2 */
صحيح fdt_appendprop_addrrange(فراغ *fdt, صحيح parent, صحيح nodeoffset,
			     ثابت حرف *name, uint64_t addr, uint64_t size)
{
	صحيح addr_cells, size_cells, ret;
	uint8_t data[حجم(fdt64_t) * 2], *prop;

	ret = fdt_address_cells(fdt, parent);
	ܐܢ (ret < 0)
		ܦܢܐ ret;
	addr_cells = ret;

	ret = fdt_size_cells(fdt, parent);
	ܐܢ (ret < 0)
		ܦܢܐ ret;
	size_cells = ret;

	/* check validity of address */
	prop = data;
	ܐܢ (addr_cells == 1) {
		ܐܢ ((addr > UINT32_MAX) || ((UINT32_MAX + 1 - addr) < size))
			ܦܢܐ -FDT_ERR_BADVALUE;

		fdt32_st(prop, (uint32_t)addr);
	} ܐܚܪܢܐ ܐܢ (addr_cells == 2) {
		fdt64_st(prop, addr);
	} ܐܚܪܢܐ {
		ܦܢܐ -FDT_ERR_BADNCELLS;
	}

	/* check validity of size */
	prop += addr_cells * حجم(fdt32_t);
	ܐܢ (size_cells == 1) {
		ܐܢ (size > UINT32_MAX)
			ܦܢܐ -FDT_ERR_BADVALUE;

		fdt32_st(prop, (uint32_t)size);
	} ܐܚܪܢܐ ܐܢ (size_cells == 2) {
		fdt64_st(prop, size);
	} ܐܚܪܢܐ {
		ܦܢܐ -FDT_ERR_BADNCELLS;
	}

	ܦܢܐ fdt_appendprop(fdt, nodeoffset, name, data,
			      (addr_cells + size_cells) * حجم(fdt32_t));
}
