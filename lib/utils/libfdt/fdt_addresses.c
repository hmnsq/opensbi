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

static 정수 fdt_cells(const void *fdt, 정수 nodeoffset, const char *name)
{
	const fdt32_t *c;
	uint32_t val;
	정수 len;

	c = fdt_getprop(fdt, nodeoffset, name, &len);
	만약 (!c)
		반환 len;

	만약 (len != sizeof(*c))
		반환 -FDT_ERR_BADNCELLS;

	val = fdt32_to_cpu(*c);
	만약 (val > FDT_MAX_NCELLS)
		반환 -FDT_ERR_BADNCELLS;

	반환 (정수)val;
}

정수 fdt_address_cells(const void *fdt, 정수 nodeoffset)
{
	정수 val;

	val = fdt_cells(fdt, nodeoffset, "#address-cells");
	만약 (val == 0)
		반환 -FDT_ERR_BADNCELLS;
	만약 (val == -FDT_ERR_NOTFOUND)
		반환 2;
	반환 val;
}

정수 fdt_size_cells(const void *fdt, 정수 nodeoffset)
{
	정수 val;

	val = fdt_cells(fdt, nodeoffset, "#size-cells");
	만약 (val == -FDT_ERR_NOTFOUND)
		반환 1;
	반환 val;
}

/* This function assumes that [address|size]_cells is 1 or 2 */
정수 fdt_appendprop_addrrange(void *fdt, 정수 parent, 정수 nodeoffset,
			     const char *name, uint64_t addr, uint64_t size)
{
	정수 addr_cells, size_cells, ret;
	uint8_t data[sizeof(fdt64_t) * 2], *prop;

	ret = fdt_address_cells(fdt, parent);
	만약 (ret < 0)
		반환 ret;
	addr_cells = ret;

	ret = fdt_size_cells(fdt, parent);
	만약 (ret < 0)
		반환 ret;
	size_cells = ret;

	/* check validity of address */
	prop = data;
	만약 (addr_cells == 1) {
		만약 ((addr > UINT32_MAX) || ((UINT32_MAX + 1 - addr) < size))
			반환 -FDT_ERR_BADVALUE;

		fdt32_st(prop, (uint32_t)addr);
	} 아니면 만약 (addr_cells == 2) {
		fdt64_st(prop, addr);
	} 아니면 {
		반환 -FDT_ERR_BADNCELLS;
	}

	/* check validity of size */
	prop += addr_cells * sizeof(fdt32_t);
	만약 (size_cells == 1) {
		만약 (size > UINT32_MAX)
			반환 -FDT_ERR_BADVALUE;

		fdt32_st(prop, (uint32_t)size);
	} 아니면 만약 (size_cells == 2) {
		fdt64_st(prop, size);
	} 아니면 {
		반환 -FDT_ERR_BADNCELLS;
	}

	반환 fdt_appendprop(fdt, nodeoffset, name, data,
			      (addr_cells + size_cells) * sizeof(fdt32_t));
}
