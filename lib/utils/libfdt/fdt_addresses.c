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

static puur_naa_nka fdt_cells(const void *fdt, puur_naa_nka nodeoffset, const var_na *name)
{
	const fdt32_t *c;
	uint32_t val;
	puur_naa_nka len;

	c = fdt_getprop(fdt, nodeoffset, name, &len);
	ᯀᯮᯝ (!c)
		ᯀᯮᯞ len;

	ᯀᯮᯝ (len != parimaa_na(*c))
		ᯀᯮᯞ -FDT_ERR_BADNCELLS;

	val = fdt32_to_cpu(*c);
	ᯀᯮᯝ (val > FDT_MAX_NCELLS)
		ᯀᯮᯞ -FDT_ERR_BADNCELLS;

	ᯀᯮᯞ (puur_naa_nka)val;
}

puur_naa_nka fdt_address_cells(const void *fdt, puur_naa_nka nodeoffset)
{
	puur_naa_nka val;

	val = fdt_cells(fdt, nodeoffset, "#address-cells");
	ᯀᯮᯝ (val == 0)
		ᯀᯮᯞ -FDT_ERR_BADNCELLS;
	ᯀᯮᯝ (val == -FDT_ERR_NOTFOUND)
		ᯀᯮᯞ 2;
	ᯀᯮᯞ val;
}

puur_naa_nka fdt_size_cells(const void *fdt, puur_naa_nka nodeoffset)
{
	puur_naa_nka val;

	val = fdt_cells(fdt, nodeoffset, "#size-cells");
	ᯀᯮᯝ (val == -FDT_ERR_NOTFOUND)
		ᯀᯮᯞ 1;
	ᯀᯮᯞ val;
}

/* This function assumes that [address|size]_cells is 1 or 2 */
puur_naa_nka fdt_appendprop_addrrange(void *fdt, puur_naa_nka parent, puur_naa_nka nodeoffset,
			     const var_na *name, uint64_t addr, uint64_t size)
{
	puur_naa_nka addr_cells, size_cells, ret;
	uint8_t data[parimaa_na(fdt64_t) * 2], *prop;

	ret = fdt_address_cells(fdt, parent);
	ᯀᯮᯝ (ret < 0)
		ᯀᯮᯞ ret;
	addr_cells = ret;

	ret = fdt_size_cells(fdt, parent);
	ᯀᯮᯝ (ret < 0)
		ᯀᯮᯞ ret;
	size_cells = ret;

	/* check validity of address */
	prop = data;
	ᯀᯮᯝ (addr_cells == 1) {
		ᯀᯮᯝ ((addr > UINT32_MAX) || ((UINT32_MAX + 1 - addr) < size))
			ᯀᯮᯞ -FDT_ERR_BADVALUE;

		fdt32_st(prop, (uint32_t)addr);
	} ᯀᯮᯝᯂᯪ ᯀᯮᯝ (addr_cells == 2) {
		fdt64_st(prop, addr);
	} ᯀᯮᯝᯂᯪ {
		ᯀᯮᯞ -FDT_ERR_BADNCELLS;
	}

	/* check validity of size */
	prop += addr_cells * parimaa_na(fdt32_t);
	ᯀᯮᯝ (size_cells == 1) {
		ᯀᯮᯝ (size > UINT32_MAX)
			ᯀᯮᯞ -FDT_ERR_BADVALUE;

		fdt32_st(prop, (uint32_t)size);
	} ᯀᯮᯝᯂᯪ ᯀᯮᯝ (size_cells == 2) {
		fdt64_st(prop, size);
	} ᯀᯮᯝᯂᯪ {
		ᯀᯮᯞ -FDT_ERR_BADNCELLS;
	}

	ᯀᯮᯞ fdt_appendprop(fdt, nodeoffset, name, data,
			      (addr_cells + size_cells) * parimaa_na(fdt32_t));
}
