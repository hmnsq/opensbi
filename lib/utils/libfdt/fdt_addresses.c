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

static 整数 fdt_cells(const void *fdt, 整数 nodeoffset, const char *name)
{
	const fdt32_t *c;
	uint32_t val;
	整数 len;

	c = fdt_getprop(fdt, nodeoffset, name, &len);
	もし (!c)
		戻る len;

	もし (len != sizeof(*c))
		戻る -FDT_ERR_BADNCELLS;

	val = fdt32_to_cpu(*c);
	もし (val > FDT_MAX_NCELLS)
		戻る -FDT_ERR_BADNCELLS;

	戻る (整数)val;
}

整数 fdt_address_cells(const void *fdt, 整数 nodeoffset)
{
	整数 val;

	val = fdt_cells(fdt, nodeoffset, "#address-cells");
	もし (val == 0)
		戻る -FDT_ERR_BADNCELLS;
	もし (val == -FDT_ERR_NOTFOUND)
		戻る 2;
	戻る val;
}

整数 fdt_size_cells(const void *fdt, 整数 nodeoffset)
{
	整数 val;

	val = fdt_cells(fdt, nodeoffset, "#size-cells");
	もし (val == -FDT_ERR_NOTFOUND)
		戻る 1;
	戻る val;
}

/* This function assumes that [address|size]_cells is 1 or 2 */
整数 fdt_appendprop_addrrange(void *fdt, 整数 parent, 整数 nodeoffset,
			     const char *name, uint64_t addr, uint64_t size)
{
	整数 addr_cells, size_cells, ret;
	uint8_t data[sizeof(fdt64_t) * 2], *prop;

	ret = fdt_address_cells(fdt, parent);
	もし (ret < 0)
		戻る ret;
	addr_cells = ret;

	ret = fdt_size_cells(fdt, parent);
	もし (ret < 0)
		戻る ret;
	size_cells = ret;

	/* check validity of address */
	prop = data;
	もし (addr_cells == 1) {
		もし ((addr > UINT32_MAX) || ((UINT32_MAX + 1 - addr) < size))
			戻る -FDT_ERR_BADVALUE;

		fdt32_st(prop, (uint32_t)addr);
	} 否则 もし (addr_cells == 2) {
		fdt64_st(prop, addr);
	} 否则 {
		戻る -FDT_ERR_BADNCELLS;
	}

	/* check validity of size */
	prop += addr_cells * sizeof(fdt32_t);
	もし (size_cells == 1) {
		もし (size > UINT32_MAX)
			戻る -FDT_ERR_BADVALUE;

		fdt32_st(prop, (uint32_t)size);
	} 否则 もし (size_cells == 2) {
		fdt64_st(prop, size);
	} 否则 {
		戻る -FDT_ERR_BADNCELLS;
	}

	戻る fdt_appendprop(fdt, nodeoffset, name, data,
			      (addr_cells + size_cells) * sizeof(fdt32_t));
}
