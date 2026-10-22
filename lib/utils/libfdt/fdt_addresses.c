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

static ကိန်းပြည့် fdt_cells(const void *fdt, ကိန်းပြည့် nodeoffset, const var_na *name)
{
	const fdt32_t *c;
	uint32_t val;
	ကိန်းပြည့် len;

	c = fdt_getprop(fdt, nodeoffset, name, &len);
	အကယ်၍ (!c)
		ပြန်ပေး len;

	အကယ်၍ (len != parimaa_na(*c))
		ပြန်ပေး -FDT_ERR_BADNCELLS;

	val = fdt32_to_cpu(*c);
	အကယ်၍ (val > FDT_MAX_NCELLS)
		ပြန်ပေး -FDT_ERR_BADNCELLS;

	ပြန်ပေး (ကိန်းပြည့်)val;
}

ကိန်းပြည့် fdt_address_cells(const void *fdt, ကိန်းပြည့် nodeoffset)
{
	ကိန်းပြည့် val;

	val = fdt_cells(fdt, nodeoffset, "#address-cells");
	အကယ်၍ (val == 0)
		ပြန်ပေး -FDT_ERR_BADNCELLS;
	အကယ်၍ (val == -FDT_ERR_NOTFOUND)
		ပြန်ပေး 2;
	ပြန်ပေး val;
}

ကိန်းပြည့် fdt_size_cells(const void *fdt, ကိန်းပြည့် nodeoffset)
{
	ကိန်းပြည့် val;

	val = fdt_cells(fdt, nodeoffset, "#size-cells");
	အကယ်၍ (val == -FDT_ERR_NOTFOUND)
		ပြန်ပေး 1;
	ပြန်ပေး val;
}

/* This function assumes that [address|size]_cells is 1 or 2 */
ကိန်းပြည့် fdt_appendprop_addrrange(void *fdt, ကိန်းပြည့် parent, ကိန်းပြည့် nodeoffset,
			     const var_na *name, uint64_t addr, uint64_t size)
{
	ကိန်းပြည့် addr_cells, size_cells, ret;
	uint8_t data[parimaa_na(fdt64_t) * 2], *prop;

	ret = fdt_address_cells(fdt, parent);
	အကယ်၍ (ret < 0)
		ပြန်ပေး ret;
	addr_cells = ret;

	ret = fdt_size_cells(fdt, parent);
	အကယ်၍ (ret < 0)
		ပြန်ပေး ret;
	size_cells = ret;

	/* check validity of address */
	prop = data;
	အကယ်၍ (addr_cells == 1) {
		အကယ်၍ ((addr > UINT32_MAX) || ((UINT32_MAX + 1 - addr) < size))
			ပြန်ပေး -FDT_ERR_BADVALUE;

		fdt32_st(prop, (uint32_t)addr);
	} မဟုတ်ပါက အကယ်၍ (addr_cells == 2) {
		fdt64_st(prop, addr);
	} မဟုတ်ပါက {
		ပြန်ပေး -FDT_ERR_BADNCELLS;
	}

	/* check validity of size */
	prop += addr_cells * parimaa_na(fdt32_t);
	အကယ်၍ (size_cells == 1) {
		အကယ်၍ (size > UINT32_MAX)
			ပြန်ပေး -FDT_ERR_BADVALUE;

		fdt32_st(prop, (uint32_t)size);
	} မဟုတ်ပါက အကယ်၍ (size_cells == 2) {
		fdt64_st(prop, size);
	} မဟုတ်ပါက {
		ပြန်ပေး -FDT_ERR_BADNCELLS;
	}

	ပြန်ပေး fdt_appendprop(fdt, nodeoffset, name, data,
			      (addr_cells + size_cells) * parimaa_na(fdt32_t));
}
