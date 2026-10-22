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

ایستا صحیح fdt_cells(ثابت تهی *fdt, صحیح nodeoffset, ثابت نویسه *name)
{
	ثابت fdt32_t *c;
	uint32_t val;
	صحیح len;

	c = fdt_getprop(fdt, nodeoffset, name, &len);
	اگر (!c)
		بازگشت len;

	اگر (len != اندازه(*c))
		بازگشت -FDT_ERR_BADNCELLS;

	val = fdt32_to_cpu(*c);
	اگر (val > FDT_MAX_NCELLS)
		بازگشت -FDT_ERR_BADNCELLS;

	بازگشت (صحیح)val;
}

صحیح fdt_address_cells(ثابت تهی *fdt, صحیح nodeoffset)
{
	صحیح val;

	val = fdt_cells(fdt, nodeoffset, "#address-cells");
	اگر (val == 0)
		بازگشت -FDT_ERR_BADNCELLS;
	اگر (val == -FDT_ERR_NOTFOUND)
		بازگشت 2;
	بازگشت val;
}

صحیح fdt_size_cells(ثابت تهی *fdt, صحیح nodeoffset)
{
	صحیح val;

	val = fdt_cells(fdt, nodeoffset, "#size-cells");
	اگر (val == -FDT_ERR_NOTFOUND)
		بازگشت 1;
	بازگشت val;
}

/* This function assumes that [address|size]_cells is 1 or 2 */
صحیح fdt_appendprop_addrrange(تهی *fdt, صحیح parent, صحیح nodeoffset,
			     ثابت نویسه *name, uint64_t addr, uint64_t size)
{
	صحیح addr_cells, size_cells, ret;
	uint8_t data[اندازه(fdt64_t) * 2], *prop;

	ret = fdt_address_cells(fdt, parent);
	اگر (ret < 0)
		بازگشت ret;
	addr_cells = ret;

	ret = fdt_size_cells(fdt, parent);
	اگر (ret < 0)
		بازگشت ret;
	size_cells = ret;

	/* check validity of address */
	prop = data;
	اگر (addr_cells == 1) {
		اگر ((addr > UINT32_MAX) || ((UINT32_MAX + 1 - addr) < size))
			بازگشت -FDT_ERR_BADVALUE;

		fdt32_st(prop, (uint32_t)addr);
	} وگرنه اگر (addr_cells == 2) {
		fdt64_st(prop, addr);
	} وگرنه {
		بازگشت -FDT_ERR_BADNCELLS;
	}

	/* check validity of size */
	prop += addr_cells * اندازه(fdt32_t);
	اگر (size_cells == 1) {
		اگر (size > UINT32_MAX)
			بازگشت -FDT_ERR_BADVALUE;

		fdt32_st(prop, (uint32_t)size);
	} وگرنه اگر (size_cells == 2) {
		fdt64_st(prop, size);
	} وگرنه {
		بازگشت -FDT_ERR_BADNCELLS;
	}

	بازگشت fdt_appendprop(fdt, nodeoffset, name, data,
			      (addr_cells + size_cells) * اندازه(fdt32_t));
}
