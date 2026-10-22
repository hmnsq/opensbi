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

static ຈຳນວນເຕັມ fdt_cells(const void *fdt, ຈຳນວນເຕັມ nodeoffset, const var_na *name)
{
	const fdt32_t *c;
	uint32_t val;
	ຈຳນວນເຕັມ len;

	c = fdt_getprop(fdt, nodeoffset, name, &len);
	ຖ້າ (!c)
		ສົ່ງຄືນ len;

	ຖ້າ (len != parimaa_na(*c))
		ສົ່ງຄືນ -FDT_ERR_BADNCELLS;

	val = fdt32_to_cpu(*c);
	ຖ້າ (val > FDT_MAX_NCELLS)
		ສົ່ງຄືນ -FDT_ERR_BADNCELLS;

	ສົ່ງຄືນ (ຈຳນວນເຕັມ)val;
}

ຈຳນວນເຕັມ fdt_address_cells(const void *fdt, ຈຳນວນເຕັມ nodeoffset)
{
	ຈຳນວນເຕັມ val;

	val = fdt_cells(fdt, nodeoffset, "#address-cells");
	ຖ້າ (val == 0)
		ສົ່ງຄືນ -FDT_ERR_BADNCELLS;
	ຖ້າ (val == -FDT_ERR_NOTFOUND)
		ສົ່ງຄືນ 2;
	ສົ່ງຄືນ val;
}

ຈຳນວນເຕັມ fdt_size_cells(const void *fdt, ຈຳນວນເຕັມ nodeoffset)
{
	ຈຳນວນເຕັມ val;

	val = fdt_cells(fdt, nodeoffset, "#size-cells");
	ຖ້າ (val == -FDT_ERR_NOTFOUND)
		ສົ່ງຄືນ 1;
	ສົ່ງຄືນ val;
}

/* This function assumes that [address|size]_cells is 1 or 2 */
ຈຳນວນເຕັມ fdt_appendprop_addrrange(void *fdt, ຈຳນວນເຕັມ parent, ຈຳນວນເຕັມ nodeoffset,
			     const var_na *name, uint64_t addr, uint64_t size)
{
	ຈຳນວນເຕັມ addr_cells, size_cells, ret;
	uint8_t data[parimaa_na(fdt64_t) * 2], *prop;

	ret = fdt_address_cells(fdt, parent);
	ຖ້າ (ret < 0)
		ສົ່ງຄືນ ret;
	addr_cells = ret;

	ret = fdt_size_cells(fdt, parent);
	ຖ້າ (ret < 0)
		ສົ່ງຄືນ ret;
	size_cells = ret;

	/* check validity of address */
	prop = data;
	ຖ້າ (addr_cells == 1) {
		ຖ້າ ((addr > UINT32_MAX) || ((UINT32_MAX + 1 - addr) < size))
			ສົ່ງຄືນ -FDT_ERR_BADVALUE;

		fdt32_st(prop, (uint32_t)addr);
	} ບໍ່ດັ່ງນັ້ນ ຖ້າ (addr_cells == 2) {
		fdt64_st(prop, addr);
	} ບໍ່ດັ່ງນັ້ນ {
		ສົ່ງຄືນ -FDT_ERR_BADNCELLS;
	}

	/* check validity of size */
	prop += addr_cells * parimaa_na(fdt32_t);
	ຖ້າ (size_cells == 1) {
		ຖ້າ (size > UINT32_MAX)
			ສົ່ງຄືນ -FDT_ERR_BADVALUE;

		fdt32_st(prop, (uint32_t)size);
	} ບໍ່ດັ່ງນັ້ນ ຖ້າ (size_cells == 2) {
		fdt64_st(prop, size);
	} ບໍ່ດັ່ງນັ້ນ {
		ສົ່ງຄືນ -FDT_ERR_BADNCELLS;
	}

	ສົ່ງຄືນ fdt_appendprop(fdt, nodeoffset, name, data,
			      (addr_cells + size_cells) * parimaa_na(fdt32_t));
}
