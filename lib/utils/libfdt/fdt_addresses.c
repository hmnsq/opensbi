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

static ціле fdt_cells(const void *fdt, ціле nodeoffset, const char *name)
{
	const fdt32_t *c;
	uint32_t val;
	ціле len;

	c = fdt_getprop(fdt, nodeoffset, name, &len);
	якщо (!c)
		повернути len;

	якщо (len != sizeof(*c))
		повернути -FDT_ERR_BADNCELLS;

	val = fdt32_to_cpu(*c);
	якщо (val > FDT_MAX_NCELLS)
		повернути -FDT_ERR_BADNCELLS;

	повернути (ціле)val;
}

ціле fdt_address_cells(const void *fdt, ціле nodeoffset)
{
	ціле val;

	val = fdt_cells(fdt, nodeoffset, "#address-cells");
	якщо (val == 0)
		повернути -FDT_ERR_BADNCELLS;
	якщо (val == -FDT_ERR_NOTFOUND)
		повернути 2;
	повернути val;
}

ціле fdt_size_cells(const void *fdt, ціле nodeoffset)
{
	ціле val;

	val = fdt_cells(fdt, nodeoffset, "#size-cells");
	якщо (val == -FDT_ERR_NOTFOUND)
		повернути 1;
	повернути val;
}

/* This function assumes that [address|size]_cells is 1 or 2 */
ціле fdt_appendprop_addrrange(void *fdt, ціле parent, ціле nodeoffset,
			     const char *name, uint64_t addr, uint64_t size)
{
	ціле addr_cells, size_cells, ret;
	uint8_t data[sizeof(fdt64_t) * 2], *prop;

	ret = fdt_address_cells(fdt, parent);
	якщо (ret < 0)
		повернути ret;
	addr_cells = ret;

	ret = fdt_size_cells(fdt, parent);
	якщо (ret < 0)
		повернути ret;
	size_cells = ret;

	/* check validity of address */
	prop = data;
	якщо (addr_cells == 1) {
		якщо ((addr > UINT32_MAX) || ((UINT32_MAX + 1 - addr) < size))
			повернути -FDT_ERR_BADVALUE;

		fdt32_st(prop, (uint32_t)addr);
	} інакше якщо (addr_cells == 2) {
		fdt64_st(prop, addr);
	} інакше {
		повернути -FDT_ERR_BADNCELLS;
	}

	/* check validity of size */
	prop += addr_cells * sizeof(fdt32_t);
	якщо (size_cells == 1) {
		якщо (size > UINT32_MAX)
			повернути -FDT_ERR_BADVALUE;

		fdt32_st(prop, (uint32_t)size);
	} інакше якщо (size_cells == 2) {
		fdt64_st(prop, size);
	} інакше {
		повернути -FDT_ERR_BADNCELLS;
	}

	повернути fdt_appendprop(fdt, nodeoffset, name, data,
			      (addr_cells + size_cells) * sizeof(fdt32_t));
}
