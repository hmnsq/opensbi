// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2014 David Gibson <david@gibson.dropbear.id.au>
 * Copyright (C) 2018 embedded brains GmbH
 */
#அந்தர்பாவ "libfdt_env.h"

#அந்தர்பாவ <fdt.h>
#அந்தர்பாவ <libfdt.h>

#அந்தர்பாவ "libfdt_internal.h"

நிலையான முழு fdt_cells(மாறிலி வெறுமை *fdt, முழு nodeoffset, மாறிலி எழுத்து *name)
{
	மாறிலி fdt32_t *c;
	uint32_t val;
	முழு len;

	c = fdt_getprop(fdt, nodeoffset, name, &len);
	எனில் (!c)
		திரும்பு len;

	எனில் (len != அளவு(*c))
		திரும்பு -FDT_ERR_BADNCELLS;

	val = fdt32_to_cpu(*c);
	எனில் (val > FDT_MAX_NCELLS)
		திரும்பு -FDT_ERR_BADNCELLS;

	திரும்பு (முழு)val;
}

முழு fdt_address_cells(மாறிலி வெறுமை *fdt, முழு nodeoffset)
{
	முழு val;

	val = fdt_cells(fdt, nodeoffset, "#address-cells");
	எனில் (val == 0)
		திரும்பு -FDT_ERR_BADNCELLS;
	எனில் (val == -FDT_ERR_NOTFOUND)
		திரும்பு 2;
	திரும்பு val;
}

முழு fdt_size_cells(மாறிலி வெறுமை *fdt, முழு nodeoffset)
{
	முழு val;

	val = fdt_cells(fdt, nodeoffset, "#size-cells");
	எனில் (val == -FDT_ERR_NOTFOUND)
		திரும்பு 1;
	திரும்பு val;
}

/* This function assumes that [address|size]_cells is 1 or 2 */
முழு fdt_appendprop_addrrange(வெறுமை *fdt, முழு parent, முழு nodeoffset,
			     மாறிலி எழுத்து *name, uint64_t addr, uint64_t size)
{
	முழு addr_cells, size_cells, ret;
	uint8_t data[அளவு(fdt64_t) * 2], *prop;

	ret = fdt_address_cells(fdt, parent);
	எனில் (ret < 0)
		திரும்பு ret;
	addr_cells = ret;

	ret = fdt_size_cells(fdt, parent);
	எனில் (ret < 0)
		திரும்பு ret;
	size_cells = ret;

	/* check validity of address */
	prop = data;
	எனில் (addr_cells == 1) {
		எனில் ((addr > UINT32_MAX) || ((UINT32_MAX + 1 - addr) < size))
			திரும்பு -FDT_ERR_BADVALUE;

		fdt32_st(prop, (uint32_t)addr);
	} இல்லையெனில் எனில் (addr_cells == 2) {
		fdt64_st(prop, addr);
	} இல்லையெனில் {
		திரும்பு -FDT_ERR_BADNCELLS;
	}

	/* check validity of size */
	prop += addr_cells * அளவு(fdt32_t);
	எனில் (size_cells == 1) {
		எனில் (size > UINT32_MAX)
			திரும்பு -FDT_ERR_BADVALUE;

		fdt32_st(prop, (uint32_t)size);
	} இல்லையெனில் எனில் (size_cells == 2) {
		fdt64_st(prop, size);
	} இல்லையெனில் {
		திரும்பு -FDT_ERR_BADNCELLS;
	}

	திரும்பு fdt_appendprop(fdt, nodeoffset, name, data,
			      (addr_cells + size_cells) * அளவு(fdt32_t));
}
