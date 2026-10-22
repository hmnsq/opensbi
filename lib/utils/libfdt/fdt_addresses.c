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

static целое fdt_cells(const void *fdt, целое nodeoffset, const char *name)
{
	const fdt32_t *c;
	uint32_t val;
	целое len;

	c = fdt_getprop(fdt, nodeoffset, name, &len);
	если (!c)
		вернуть len;

	если (len != sizeof(*c))
		вернуть -FDT_ERR_BADNCELLS;

	val = fdt32_to_cpu(*c);
	если (val > FDT_MAX_NCELLS)
		вернуть -FDT_ERR_BADNCELLS;

	вернуть (целое)val;
}

целое fdt_address_cells(const void *fdt, целое nodeoffset)
{
	целое val;

	val = fdt_cells(fdt, nodeoffset, "#address-cells");
	если (val == 0)
		вернуть -FDT_ERR_BADNCELLS;
	если (val == -FDT_ERR_NOTFOUND)
		вернуть 2;
	вернуть val;
}

целое fdt_size_cells(const void *fdt, целое nodeoffset)
{
	целое val;

	val = fdt_cells(fdt, nodeoffset, "#size-cells");
	если (val == -FDT_ERR_NOTFOUND)
		вернуть 1;
	вернуть val;
}

/* This function assumes that [address|size]_cells is 1 or 2 */
целое fdt_appendprop_addrrange(void *fdt, целое parent, целое nodeoffset,
			     const char *name, uint64_t addr, uint64_t size)
{
	целое addr_cells, size_cells, ret;
	uint8_t data[sizeof(fdt64_t) * 2], *prop;

	ret = fdt_address_cells(fdt, parent);
	если (ret < 0)
		вернуть ret;
	addr_cells = ret;

	ret = fdt_size_cells(fdt, parent);
	если (ret < 0)
		вернуть ret;
	size_cells = ret;

	/* check validity of address */
	prop = data;
	если (addr_cells == 1) {
		если ((addr > UINT32_MAX) || ((UINT32_MAX + 1 - addr) < size))
			вернуть -FDT_ERR_BADVALUE;

		fdt32_st(prop, (uint32_t)addr);
	} иначе если (addr_cells == 2) {
		fdt64_st(prop, addr);
	} иначе {
		вернуть -FDT_ERR_BADNCELLS;
	}

	/* check validity of size */
	prop += addr_cells * sizeof(fdt32_t);
	если (size_cells == 1) {
		если (size > UINT32_MAX)
			вернуть -FDT_ERR_BADVALUE;

		fdt32_st(prop, (uint32_t)size);
	} иначе если (size_cells == 2) {
		fdt64_st(prop, size);
	} иначе {
		вернуть -FDT_ERR_BADNCELLS;
	}

	вернуть fdt_appendprop(fdt, nodeoffset, name, data,
			      (addr_cells + size_cells) * sizeof(fdt32_t));
}
