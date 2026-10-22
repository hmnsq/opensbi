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

static цео fdt_cells(const void *fdt, цео nodeoffset, const char *name)
{
	const fdt32_t *c;
	uint32_t val;
	цео len;

	c = fdt_getprop(fdt, nodeoffset, name, &len);
	ако (!c)
		врати len;

	ако (len != sizeof(*c))
		врати -FDT_ERR_BADNCELLS;

	val = fdt32_to_cpu(*c);
	ако (val > FDT_MAX_NCELLS)
		врати -FDT_ERR_BADNCELLS;

	врати (цео)val;
}

цео fdt_address_cells(const void *fdt, цео nodeoffset)
{
	цео val;

	val = fdt_cells(fdt, nodeoffset, "#address-cells");
	ако (val == 0)
		врати -FDT_ERR_BADNCELLS;
	ако (val == -FDT_ERR_NOTFOUND)
		врати 2;
	врати val;
}

цео fdt_size_cells(const void *fdt, цео nodeoffset)
{
	цео val;

	val = fdt_cells(fdt, nodeoffset, "#size-cells");
	ако (val == -FDT_ERR_NOTFOUND)
		врати 1;
	врати val;
}

/* This function assumes that [address|size]_cells is 1 or 2 */
цео fdt_appendprop_addrrange(void *fdt, цео parent, цео nodeoffset,
			     const char *name, uint64_t addr, uint64_t size)
{
	цео addr_cells, size_cells, ret;
	uint8_t data[sizeof(fdt64_t) * 2], *prop;

	ret = fdt_address_cells(fdt, parent);
	ако (ret < 0)
		врати ret;
	addr_cells = ret;

	ret = fdt_size_cells(fdt, parent);
	ако (ret < 0)
		врати ret;
	size_cells = ret;

	/* check validity of address */
	prop = data;
	ако (addr_cells == 1) {
		ако ((addr > UINT32_MAX) || ((UINT32_MAX + 1 - addr) < size))
			врати -FDT_ERR_BADVALUE;

		fdt32_st(prop, (uint32_t)addr);
	} инако ако (addr_cells == 2) {
		fdt64_st(prop, addr);
	} инако {
		врати -FDT_ERR_BADNCELLS;
	}

	/* check validity of size */
	prop += addr_cells * sizeof(fdt32_t);
	ако (size_cells == 1) {
		ако (size > UINT32_MAX)
			врати -FDT_ERR_BADVALUE;

		fdt32_st(prop, (uint32_t)size);
	} инако ако (size_cells == 2) {
		fdt64_st(prop, size);
	} инако {
		врати -FDT_ERR_BADNCELLS;
	}

	врати fdt_appendprop(fdt, nodeoffset, name, data,
			      (addr_cells + size_cells) * sizeof(fdt32_t));
}
