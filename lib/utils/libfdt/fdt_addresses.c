// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2014 David Gibson <david@gibson.dropbear.id.au>
 * Copyright (C) 2018 embedded brains GmbH
 */
#ضمن "libfdt_env.h"

#ضمن <fdt.h>
#ضمن <libfdt.h>

#ضمن "libfdt_internal.h"

ساكن صحيح fdt_cells(ثابت فراغ *fdt, صحيح nodeoffset, ثابت حرف *name)
{
	ثابت fdt32_t *c;
	uint32_t val;
	صحيح len;

	c = fdt_getprop(fdt, nodeoffset, name, &len);
	إذا (!c)
		إرجاع len;

	إذا (len != حجم(*c))
		إرجاع -FDT_ERR_BADNCELLS;

	val = fdt32_to_cpu(*c);
	إذا (val > FDT_MAX_NCELLS)
		إرجاع -FDT_ERR_BADNCELLS;

	إرجاع (صحيح)val;
}

صحيح fdt_address_cells(ثابت فراغ *fdt, صحيح nodeoffset)
{
	صحيح val;

	val = fdt_cells(fdt, nodeoffset, "#address-cells");
	إذا (val == 0)
		إرجاع -FDT_ERR_BADNCELLS;
	إذا (val == -FDT_ERR_NOTFOUND)
		إرجاع 2;
	إرجاع val;
}

صحيح fdt_size_cells(ثابت فراغ *fdt, صحيح nodeoffset)
{
	صحيح val;

	val = fdt_cells(fdt, nodeoffset, "#size-cells");
	إذا (val == -FDT_ERR_NOTFOUND)
		إرجاع 1;
	إرجاع val;
}

/* This function assumes that [address|size]_cells is 1 or 2 */
صحيح fdt_appendprop_addrrange(فراغ *fdt, صحيح parent, صحيح nodeoffset,
			     ثابت حرف *name, uint64_t addr, uint64_t size)
{
	صحيح addr_cells, size_cells, ret;
	uint8_t data[حجم(fdt64_t) * 2], *prop;

	ret = fdt_address_cells(fdt, parent);
	إذا (ret < 0)
		إرجاع ret;
	addr_cells = ret;

	ret = fdt_size_cells(fdt, parent);
	إذا (ret < 0)
		إرجاع ret;
	size_cells = ret;

	/* check validity of address */
	prop = data;
	إذا (addr_cells == 1) {
		إذا ((addr > UINT32_MAX) || ((UINT32_MAX + 1 - addr) < size))
			إرجاع -FDT_ERR_BADVALUE;

		fdt32_st(prop, (uint32_t)addr);
	} وإلا إذا (addr_cells == 2) {
		fdt64_st(prop, addr);
	} وإلا {
		إرجاع -FDT_ERR_BADNCELLS;
	}

	/* check validity of size */
	prop += addr_cells * حجم(fdt32_t);
	إذا (size_cells == 1) {
		إذا (size > UINT32_MAX)
			إرجاع -FDT_ERR_BADVALUE;

		fdt32_st(prop, (uint32_t)size);
	} وإلا إذا (size_cells == 2) {
		fdt64_st(prop, size);
	} وإلا {
		إرجاع -FDT_ERR_BADNCELLS;
	}

	إرجاع fdt_appendprop(fdt, nodeoffset, name, data,
			      (addr_cells + size_cells) * حجم(fdt32_t));
}
