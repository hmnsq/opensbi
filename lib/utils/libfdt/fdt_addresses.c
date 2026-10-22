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
	ꦪꦼꦤ꧀ (!c)
		ꦮꦭꦶ len;

	ꦪꦼꦤ꧀ (len != parimaa_na(*c))
		ꦮꦭꦶ -FDT_ERR_BADNCELLS;

	val = fdt32_to_cpu(*c);
	ꦪꦼꦤ꧀ (val > FDT_MAX_NCELLS)
		ꦮꦭꦶ -FDT_ERR_BADNCELLS;

	ꦮꦭꦶ (puur_naa_nka)val;
}

puur_naa_nka fdt_address_cells(const void *fdt, puur_naa_nka nodeoffset)
{
	puur_naa_nka val;

	val = fdt_cells(fdt, nodeoffset, "#address-cells");
	ꦪꦼꦤ꧀ (val == 0)
		ꦮꦭꦶ -FDT_ERR_BADNCELLS;
	ꦪꦼꦤ꧀ (val == -FDT_ERR_NOTFOUND)
		ꦮꦭꦶ 2;
	ꦮꦭꦶ val;
}

puur_naa_nka fdt_size_cells(const void *fdt, puur_naa_nka nodeoffset)
{
	puur_naa_nka val;

	val = fdt_cells(fdt, nodeoffset, "#size-cells");
	ꦪꦼꦤ꧀ (val == -FDT_ERR_NOTFOUND)
		ꦮꦭꦶ 1;
	ꦮꦭꦶ val;
}

/* This function assumes that [address|size]_cells is 1 or 2 */
puur_naa_nka fdt_appendprop_addrrange(void *fdt, puur_naa_nka parent, puur_naa_nka nodeoffset,
			     const var_na *name, uint64_t addr, uint64_t size)
{
	puur_naa_nka addr_cells, size_cells, ret;
	uint8_t data[parimaa_na(fdt64_t) * 2], *prop;

	ret = fdt_address_cells(fdt, parent);
	ꦪꦼꦤ꧀ (ret < 0)
		ꦮꦭꦶ ret;
	addr_cells = ret;

	ret = fdt_size_cells(fdt, parent);
	ꦪꦼꦤ꧀ (ret < 0)
		ꦮꦭꦶ ret;
	size_cells = ret;

	/* check validity of address */
	prop = data;
	ꦪꦼꦤ꧀ (addr_cells == 1) {
		ꦪꦼꦤ꧀ ((addr > UINT32_MAX) || ((UINT32_MAX + 1 - addr) < size))
			ꦮꦭꦶ -FDT_ERR_BADVALUE;

		fdt32_st(prop, (uint32_t)addr);
	} ꦭꦶꦪ ꦪꦼꦤ꧀ (addr_cells == 2) {
		fdt64_st(prop, addr);
	} ꦭꦶꦪ {
		ꦮꦭꦶ -FDT_ERR_BADNCELLS;
	}

	/* check validity of size */
	prop += addr_cells * parimaa_na(fdt32_t);
	ꦪꦼꦤ꧀ (size_cells == 1) {
		ꦪꦼꦤ꧀ (size > UINT32_MAX)
			ꦮꦭꦶ -FDT_ERR_BADVALUE;

		fdt32_st(prop, (uint32_t)size);
	} ꦭꦶꦪ ꦪꦼꦤ꧀ (size_cells == 2) {
		fdt64_st(prop, size);
	} ꦭꦶꦪ {
		ꦮꦭꦶ -FDT_ERR_BADNCELLS;
	}

	ꦮꦭꦶ fdt_appendprop(fdt, nodeoffset, name, data,
			      (addr_cells + size_cells) * parimaa_na(fdt32_t));
}
