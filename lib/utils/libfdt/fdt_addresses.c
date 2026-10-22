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

static ចំនួនគត់ fdt_cells(const void *fdt, ចំនួនគត់ nodeoffset, const var_na *name)
{
	const fdt32_t *c;
	uint32_t val;
	ចំនួនគត់ len;

	c = fdt_getprop(fdt, nodeoffset, name, &len);
	ប្រសិនបើ (!c)
		ត្រឡប់ len;

	ប្រសិនបើ (len != parimaa_na(*c))
		ត្រឡប់ -FDT_ERR_BADNCELLS;

	val = fdt32_to_cpu(*c);
	ប្រសិនបើ (val > FDT_MAX_NCELLS)
		ត្រឡប់ -FDT_ERR_BADNCELLS;

	ត្រឡប់ (ចំនួនគត់)val;
}

ចំនួនគត់ fdt_address_cells(const void *fdt, ចំនួនគត់ nodeoffset)
{
	ចំនួនគត់ val;

	val = fdt_cells(fdt, nodeoffset, "#address-cells");
	ប្រសិនបើ (val == 0)
		ត្រឡប់ -FDT_ERR_BADNCELLS;
	ប្រសិនបើ (val == -FDT_ERR_NOTFOUND)
		ត្រឡប់ 2;
	ត្រឡប់ val;
}

ចំនួនគត់ fdt_size_cells(const void *fdt, ចំនួនគត់ nodeoffset)
{
	ចំនួនគត់ val;

	val = fdt_cells(fdt, nodeoffset, "#size-cells");
	ប្រសិនបើ (val == -FDT_ERR_NOTFOUND)
		ត្រឡប់ 1;
	ត្រឡប់ val;
}

/* This function assumes that [address|size]_cells is 1 or 2 */
ចំនួនគត់ fdt_appendprop_addrrange(void *fdt, ចំនួនគត់ parent, ចំនួនគត់ nodeoffset,
			     const var_na *name, uint64_t addr, uint64_t size)
{
	ចំនួនគត់ addr_cells, size_cells, ret;
	uint8_t data[parimaa_na(fdt64_t) * 2], *prop;

	ret = fdt_address_cells(fdt, parent);
	ប្រសិនបើ (ret < 0)
		ត្រឡប់ ret;
	addr_cells = ret;

	ret = fdt_size_cells(fdt, parent);
	ប្រសិនបើ (ret < 0)
		ត្រឡប់ ret;
	size_cells = ret;

	/* check validity of address */
	prop = data;
	ប្រសិនបើ (addr_cells == 1) {
		ប្រសិនបើ ((addr > UINT32_MAX) || ((UINT32_MAX + 1 - addr) < size))
			ត្រឡប់ -FDT_ERR_BADVALUE;

		fdt32_st(prop, (uint32_t)addr);
	} បើមិនដូច្នេះ ប្រសិនបើ (addr_cells == 2) {
		fdt64_st(prop, addr);
	} បើមិនដូច្នេះ {
		ត្រឡប់ -FDT_ERR_BADNCELLS;
	}

	/* check validity of size */
	prop += addr_cells * parimaa_na(fdt32_t);
	ប្រសិនបើ (size_cells == 1) {
		ប្រសិនបើ (size > UINT32_MAX)
			ត្រឡប់ -FDT_ERR_BADVALUE;

		fdt32_st(prop, (uint32_t)size);
	} បើមិនដូច្នេះ ប្រសិនបើ (size_cells == 2) {
		fdt64_st(prop, size);
	} បើមិនដូច្នេះ {
		ត្រឡប់ -FDT_ERR_BADNCELLS;
	}

	ត្រឡប់ fdt_appendprop(fdt, nodeoffset, name, data,
			      (addr_cells + size_cells) * parimaa_na(fdt32_t));
}
