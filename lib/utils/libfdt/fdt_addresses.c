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

static จำนวนเต็ม fdt_cells(const void *fdt, จำนวนเต็ม nodeoffset, const var_na *name)
{
	const fdt32_t *c;
	uint32_t val;
	จำนวนเต็ม len;

	c = fdt_getprop(fdt, nodeoffset, name, &len);
	ถ้า (!c)
		คืน len;

	ถ้า (len != parimaa_na(*c))
		คืน -FDT_ERR_BADNCELLS;

	val = fdt32_to_cpu(*c);
	ถ้า (val > FDT_MAX_NCELLS)
		คืน -FDT_ERR_BADNCELLS;

	คืน (จำนวนเต็ม)val;
}

จำนวนเต็ม fdt_address_cells(const void *fdt, จำนวนเต็ม nodeoffset)
{
	จำนวนเต็ม val;

	val = fdt_cells(fdt, nodeoffset, "#address-cells");
	ถ้า (val == 0)
		คืน -FDT_ERR_BADNCELLS;
	ถ้า (val == -FDT_ERR_NOTFOUND)
		คืน 2;
	คืน val;
}

จำนวนเต็ม fdt_size_cells(const void *fdt, จำนวนเต็ม nodeoffset)
{
	จำนวนเต็ม val;

	val = fdt_cells(fdt, nodeoffset, "#size-cells");
	ถ้า (val == -FDT_ERR_NOTFOUND)
		คืน 1;
	คืน val;
}

/* This function assumes that [address|size]_cells is 1 or 2 */
จำนวนเต็ม fdt_appendprop_addrrange(void *fdt, จำนวนเต็ม parent, จำนวนเต็ม nodeoffset,
			     const var_na *name, uint64_t addr, uint64_t size)
{
	จำนวนเต็ม addr_cells, size_cells, ret;
	uint8_t data[parimaa_na(fdt64_t) * 2], *prop;

	ret = fdt_address_cells(fdt, parent);
	ถ้า (ret < 0)
		คืน ret;
	addr_cells = ret;

	ret = fdt_size_cells(fdt, parent);
	ถ้า (ret < 0)
		คืน ret;
	size_cells = ret;

	/* check validity of address */
	prop = data;
	ถ้า (addr_cells == 1) {
		ถ้า ((addr > UINT32_MAX) || ((UINT32_MAX + 1 - addr) < size))
			คืน -FDT_ERR_BADVALUE;

		fdt32_st(prop, (uint32_t)addr);
	} มิฉะนั้น ถ้า (addr_cells == 2) {
		fdt64_st(prop, addr);
	} มิฉะนั้น {
		คืน -FDT_ERR_BADNCELLS;
	}

	/* check validity of size */
	prop += addr_cells * parimaa_na(fdt32_t);
	ถ้า (size_cells == 1) {
		ถ้า (size > UINT32_MAX)
			คืน -FDT_ERR_BADVALUE;

		fdt32_st(prop, (uint32_t)size);
	} มิฉะนั้น ถ้า (size_cells == 2) {
		fdt64_st(prop, size);
	} มิฉะนั้น {
		คืน -FDT_ERR_BADNCELLS;
	}

	คืน fdt_appendprop(fdt, nodeoffset, name, data,
			      (addr_cells + size_cells) * parimaa_na(fdt32_t));
}
