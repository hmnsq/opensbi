// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2014 David Gibson <david@gibson.dropbear.id.au>
 * Copyright (C) 2018 embedded brains GmbH
 */
#شامل "libfdt_env.h"

#شامل <fdt.h>
#شامل <libfdt.h>

#شامل "libfdt_internal.h"

ساکن صحیح fdt_cells(ثابت خالی *fdt, صحیح nodeoffset, ثابت حرف *name)
{
	ثابت fdt32_t *c;
	uint32_t val;
	صحیح len;

	c = fdt_getprop(fdt, nodeoffset, name, &len);
	اگر (!c)
		واپس len;

	اگر (len != حجم(*c))
		واپس -FDT_ERR_BADNCELLS;

	val = fdt32_مقصد_cpu(*c);
	اگر (val > FDT_MAX_NCELLS)
		واپس -FDT_ERR_BADNCELLS;

	واپس (صحیح)val;
}

صحیح fdt_پتہ_cells(ثابت خالی *fdt, صحیح nodeoffset)
{
	صحیح val;

	val = fdt_cells(fdt, nodeoffset, "#address-cells");
	اگر (val == 0)
		واپس -FDT_ERR_BADNCELLS;
	اگر (val == -FDT_ERR_NOTFOUND)
		واپس 2;
	واپس val;
}

صحیح fdt_سائز_cells(ثابت خالی *fdt, صحیح nodeoffset)
{
	صحیح val;

	val = fdt_cells(fdt, nodeoffset, "#size-cells");
	اگر (val == -FDT_ERR_NOTFOUND)
		واپس 1;
	واپس val;
}

/* This function assumes that [address|size]_cells is 1 or 2 */
صحیح fdt_appendprop_addrrange(خالی *fdt, صحیح والد, صحیح nodeoffset,
			     ثابت حرف *name, uint64_t addr, uint64_t سائز)
{
	صحیح addr_cells, سائز_cells, ret;
	uint8_t ڈیٹا[حجم(fdt64_t) * 2], *prop;

	ret = fdt_پتہ_cells(fdt, والد);
	اگر (ret < 0)
		واپس ret;
	addr_cells = ret;

	ret = fdt_سائز_cells(fdt, والد);
	اگر (ret < 0)
		واپس ret;
	سائز_cells = ret;

	/* check validity of address */
	prop = ڈیٹا;
	اگر (addr_cells == 1) {
		اگر ((addr > UINT32_MAX) || ((UINT32_MAX + 1 - addr) < سائز))
			واپس -FDT_ERR_BADVALUE;

		fdt32_st(prop, (uint32_t)addr);
	} ورنہ اگر (addr_cells == 2) {
		fdt64_st(prop, addr);
	} ورنہ {
		واپس -FDT_ERR_BADNCELLS;
	}

	/* check validity of size */
	prop += addr_cells * حجم(fdt32_t);
	اگر (سائز_cells == 1) {
		اگر (سائز > UINT32_MAX)
			واپس -FDT_ERR_BADVALUE;

		fdt32_st(prop, (uint32_t)سائز);
	} ورنہ اگر (سائز_cells == 2) {
		fdt64_st(prop, سائز);
	} ورنہ {
		واپس -FDT_ERR_BADNCELLS;
	}

	واپس fdt_appendprop(fdt, nodeoffset, name, ڈیٹا,
			      (addr_cells + سائز_cells) * حجم(fdt32_t));
}
