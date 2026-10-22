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
		أرجع len;

	إذا (len != حجم(*c))
		أرجع -FDT_ERR_BADNCELLS;

	val = fdt32_منزل_cpu(*c);
	إذا (val > FDT_MAX_NCELLS)
		أرجع -FDT_ERR_BADNCELLS;

	أرجع (صحيح)val;
}

صحيح fdt_پتہ_cells(ثابت فراغ *fdt, صحيح nodeoffset)
{
	صحيح val;

	val = fdt_cells(fdt, nodeoffset, "#address-cells");
	إذا (val == 0)
		أرجع -FDT_ERR_BADNCELLS;
	إذا (val == -FDT_ERR_NOTFOUND)
		أرجع 2;
	أرجع val;
}

صحيح fdt_ناپ_cells(ثابت فراغ *fdt, صحيح nodeoffset)
{
	صحيح val;

	val = fdt_cells(fdt, nodeoffset, "#size-cells");
	إذا (val == -FDT_ERR_NOTFOUND)
		أرجع 1;
	أرجع val;
}

/* This function assumes that [address|size]_cells is 1 or 2 */
صحيح fdt_appendprop_addrrange(فراغ *fdt, صحيح ماپا, صحيح nodeoffset,
			     ثابت حرف *name, uint64_t addr, uint64_t ناپ)
{
	صحيح addr_cells, ناپ_cells, ret;
	uint8_t انکڑے[حجم(fdt64_t) * 2], *prop;

	ret = fdt_پتہ_cells(fdt, ماپا);
	إذا (ret < 0)
		أرجع ret;
	addr_cells = ret;

	ret = fdt_ناپ_cells(fdt, ماپا);
	إذا (ret < 0)
		أرجع ret;
	ناپ_cells = ret;

	/* check validity of address */
	prop = انکڑے;
	إذا (addr_cells == 1) {
		إذا ((addr > UINT32_MAX) || ((UINT32_MAX + 1 - addr) < ناپ))
			أرجع -FDT_ERR_BADVALUE;

		fdt32_st(prop, (uint32_t)addr);
	} وإلا إذا (addr_cells == 2) {
		fdt64_st(prop, addr);
	} وإلا {
		أرجع -FDT_ERR_BADNCELLS;
	}

	/* check validity of size */
	prop += addr_cells * حجم(fdt32_t);
	إذا (ناپ_cells == 1) {
		إذا (ناپ > UINT32_MAX)
			أرجع -FDT_ERR_BADVALUE;

		fdt32_st(prop, (uint32_t)ناپ);
	} وإلا إذا (ناپ_cells == 2) {
		fdt64_st(prop, ناپ);
	} وإلا {
		أرجع -FDT_ERR_BADNCELLS;
	}

	أرجع fdt_appendprop(fdt, nodeoffset, name, انکڑے,
			      (addr_cells + ناپ_cells) * حجم(fdt32_t));
}
