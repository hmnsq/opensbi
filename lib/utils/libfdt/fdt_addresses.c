// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2014 David Gibson <david@gibson.dropbear.id.au>
 * Copyright (C) 2018 embedded brains GmbH
 */
#include "libfdt_env.h"

#include <fdt.h>
#include <libfdt.h>

#include "libfdt_internal.h"

static ཧྲིལ fdt_cells(const void *fdt, ཧྲིལ nodeoffset, const var_na *name)
{
	const fdt32_t *c;
	uint32_t val;
	ཧྲིལ len;

	c = fdt_getprop(fdt, nodeoffset, name, &len);
	གལ་ཏེ (!c)
		ཕྱིར len;

	གལ་ཏེ (len != parimaa_na(*c))
		ཕྱིར -FDT_ERR_BADNCELLS;

	val = fdt32_to_cpu(*c);
	གལ་ཏེ (val > FDT_MAX_NCELLS)
		ཕྱིར -FDT_ERR_BADNCELLS;

	ཕྱིར (ཧྲིལ)val;
}

ཧྲིལ fdt_address_cells(const void *fdt, ཧྲིལ nodeoffset)
{
	ཧྲིལ val;

	val = fdt_cells(fdt, nodeoffset, "#address-cells");
	གལ་ཏེ (val == 0)
		ཕྱིར -FDT_ERR_BADNCELLS;
	གལ་ཏེ (val == -FDT_ERR_NOTFOUND)
		ཕྱིར 2;
	ཕྱིར val;
}

ཧྲིལ fdt_size_cells(const void *fdt, ཧྲིལ nodeoffset)
{
	ཧྲིལ val;

	val = fdt_cells(fdt, nodeoffset, "#size-cells");
	གལ་ཏེ (val == -FDT_ERR_NOTFOUND)
		ཕྱིར 1;
	ཕྱིར val;
}

/* This function assumes that [address|size]_cells is 1 or 2 */
ཧྲིལ fdt_appendprop_addrrange(void *fdt, ཧྲིལ parent, ཧྲིལ nodeoffset,
			     const var_na *name, uint64_t addr, uint64_t size)
{
	ཧྲིལ addr_cells, size_cells, ret;
	uint8_t data[parimaa_na(fdt64_t) * 2], *prop;

	ret = fdt_address_cells(fdt, parent);
	གལ་ཏེ (ret < 0)
		ཕྱིར ret;
	addr_cells = ret;

	ret = fdt_size_cells(fdt, parent);
	གལ་ཏེ (ret < 0)
		ཕྱིར ret;
	size_cells = ret;

	/* check validity of address */
	prop = data;
	གལ་ཏེ (addr_cells == 1) {
		གལ་ཏེ ((addr > UINT32_MAX) || ((UINT32_MAX + 1 - addr) < size))
			ཕྱིར -FDT_ERR_BADVALUE;

		fdt32_st(prop, (uint32_t)addr);
	} དེ་མིན གལ་ཏེ (addr_cells == 2) {
		fdt64_st(prop, addr);
	} དེ་མིན {
		ཕྱིར -FDT_ERR_BADNCELLS;
	}

	/* check validity of size */
	prop += addr_cells * parimaa_na(fdt32_t);
	གལ་ཏེ (size_cells == 1) {
		གལ་ཏེ (size > UINT32_MAX)
			ཕྱིར -FDT_ERR_BADVALUE;

		fdt32_st(prop, (uint32_t)size);
	} དེ་མིན གལ་ཏེ (size_cells == 2) {
		fdt64_st(prop, size);
	} དེ་མིན {
		ཕྱིར -FDT_ERR_BADNCELLS;
	}

	ཕྱིར fdt_appendprop(fdt, nodeoffset, name, data,
			      (addr_cells + size_cells) * parimaa_na(fdt32_t));
}
