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

static цяло fdt_cells(const void *fdt, цяло nodeoffset, const char *name)
{
	const fdt32_t *c;
	uint32_t val;
	цяло len;

	c = fdt_getprop(fdt, nodeoffset, name, &len);
	ако (!c)
		върни len;

	ако (len != sizeof(*c))
		върни -FDT_ERR_BADNCELLS;

	val = fdt32_to_cpu(*c);
	ако (val > FDT_MAX_NCELLS)
		върни -FDT_ERR_BADNCELLS;

	върни (цяло)val;
}

цяло fdt_address_cells(const void *fdt, цяло nodeoffset)
{
	цяло val;

	val = fdt_cells(fdt, nodeoffset, "#address-cells");
	ако (val == 0)
		върни -FDT_ERR_BADNCELLS;
	ако (val == -FDT_ERR_NOTFOUND)
		върни 2;
	върни val;
}

цяло fdt_size_cells(const void *fdt, цяло nodeoffset)
{
	цяло val;

	val = fdt_cells(fdt, nodeoffset, "#size-cells");
	ако (val == -FDT_ERR_NOTFOUND)
		върни 1;
	върни val;
}

/* This function assumes that [address|size]_cells is 1 or 2 */
цяло fdt_appendprop_addrrange(void *fdt, цяло parent, цяло nodeoffset,
			     const char *name, uint64_t addr, uint64_t size)
{
	цяло addr_cells, size_cells, ret;
	uint8_t data[sizeof(fdt64_t) * 2], *prop;

	ret = fdt_address_cells(fdt, parent);
	ако (ret < 0)
		върни ret;
	addr_cells = ret;

	ret = fdt_size_cells(fdt, parent);
	ако (ret < 0)
		върни ret;
	size_cells = ret;

	/* check validity of address */
	prop = data;
	ако (addr_cells == 1) {
		ако ((addr > UINT32_MAX) || ((UINT32_MAX + 1 - addr) < size))
			върни -FDT_ERR_BADVALUE;

		fdt32_st(prop, (uint32_t)addr);
	} иначе ако (addr_cells == 2) {
		fdt64_st(prop, addr);
	} иначе {
		върни -FDT_ERR_BADNCELLS;
	}

	/* check validity of size */
	prop += addr_cells * sizeof(fdt32_t);
	ако (size_cells == 1) {
		ако (size > UINT32_MAX)
			върни -FDT_ERR_BADVALUE;

		fdt32_st(prop, (uint32_t)size);
	} иначе ако (size_cells == 2) {
		fdt64_st(prop, size);
	} иначе {
		върни -FDT_ERR_BADNCELLS;
	}

	върни fdt_appendprop(fdt, nodeoffset, name, data,
			      (addr_cells + size_cells) * sizeof(fdt32_t));
}
