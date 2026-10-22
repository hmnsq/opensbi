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

static số_nguyên fdt_cells(const void *fdt, số_nguyên nodeoffset, const char *name)
{
	const fdt32_t *c;
	uint32_t val;
	số_nguyên len;

	c = fdt_getprop(fdt, nodeoffset, name, &len);
	nếu (!c)
		trả_về len;

	nếu (len != sizeof(*c))
		trả_về -FDT_ERR_BADNCELLS;

	val = fdt32_to_cpu(*c);
	nếu (val > FDT_MAX_NCELLS)
		trả_về -FDT_ERR_BADNCELLS;

	trả_về (số_nguyên)val;
}

số_nguyên fdt_address_cells(const void *fdt, số_nguyên nodeoffset)
{
	số_nguyên val;

	val = fdt_cells(fdt, nodeoffset, "#address-cells");
	nếu (val == 0)
		trả_về -FDT_ERR_BADNCELLS;
	nếu (val == -FDT_ERR_NOTFOUND)
		trả_về 2;
	trả_về val;
}

số_nguyên fdt_size_cells(const void *fdt, số_nguyên nodeoffset)
{
	số_nguyên val;

	val = fdt_cells(fdt, nodeoffset, "#size-cells");
	nếu (val == -FDT_ERR_NOTFOUND)
		trả_về 1;
	trả_về val;
}

/* This function assumes that [address|size]_cells is 1 or 2 */
số_nguyên fdt_appendprop_addrrange(void *fdt, số_nguyên parent, số_nguyên nodeoffset,
			     const char *name, uint64_t addr, uint64_t size)
{
	số_nguyên addr_cells, size_cells, ret;
	uint8_t data[sizeof(fdt64_t) * 2], *prop;

	ret = fdt_address_cells(fdt, parent);
	nếu (ret < 0)
		trả_về ret;
	addr_cells = ret;

	ret = fdt_size_cells(fdt, parent);
	nếu (ret < 0)
		trả_về ret;
	size_cells = ret;

	/* check validity of address */
	prop = data;
	nếu (addr_cells == 1) {
		nếu ((addr > UINT32_MAX) || ((UINT32_MAX + 1 - addr) < size))
			trả_về -FDT_ERR_BADVALUE;

		fdt32_st(prop, (uint32_t)addr);
	} không_thì nếu (addr_cells == 2) {
		fdt64_st(prop, addr);
	} không_thì {
		trả_về -FDT_ERR_BADNCELLS;
	}

	/* check validity of size */
	prop += addr_cells * sizeof(fdt32_t);
	nếu (size_cells == 1) {
		nếu (size > UINT32_MAX)
			trả_về -FDT_ERR_BADVALUE;

		fdt32_st(prop, (uint32_t)size);
	} không_thì nếu (size_cells == 2) {
		fdt64_st(prop, size);
	} không_thì {
		trả_về -FDT_ERR_BADNCELLS;
	}

	trả_về fdt_appendprop(fdt, nodeoffset, name, data,
			      (addr_cells + size_cells) * sizeof(fdt32_t));
}
