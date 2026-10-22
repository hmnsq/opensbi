// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2014 David Gibson <david@gibson.dropbear.id.au>
 * Copyright (C) 2018 embedded brains GmbH
 */
#অন্তর্ভাৱ "libfdt_env.h"

#অন্তর্ভাৱ <fdt.h>
#অন্তর্ভাৱ <libfdt.h>

#অন্তর্ভাৱ "libfdt_internal.h"

স্থির পূর্ণাঙ্ক fdt_cells(ধ্রুৱ শূন্য *fdt, পূর্ণাঙ্ক nodeoffset, ধ্রুৱ ৱর্ণ *name)
{
	ধ্রুৱ fdt32_t *c;
	uint32_t val;
	পূর্ণাঙ্ক len;

	c = fdt_getprop(fdt, nodeoffset, name, &len);
	যদি (!c)
		ঘূৰাই len;

	যদি (len != পরিমাণ(*c))
		ঘূৰাই -FDT_ERR_BADNCELLS;

	val = fdt32_to_cpu(*c);
	যদি (val > FDT_MAX_NCELLS)
		ঘূৰাই -FDT_ERR_BADNCELLS;

	ঘূৰাই (পূর্ণাঙ্ক)val;
}

পূর্ণাঙ্ক fdt_address_cells(ধ্রুৱ শূন্য *fdt, পূর্ণাঙ্ক nodeoffset)
{
	পূর্ণাঙ্ক val;

	val = fdt_cells(fdt, nodeoffset, "#address-cells");
	যদি (val == 0)
		ঘূৰাই -FDT_ERR_BADNCELLS;
	যদি (val == -FDT_ERR_NOTFOUND)
		ঘূৰাই 2;
	ঘূৰাই val;
}

পূর্ণাঙ্ক fdt_size_cells(ধ্রুৱ শূন্য *fdt, পূর্ণাঙ্ক nodeoffset)
{
	পূর্ণাঙ্ক val;

	val = fdt_cells(fdt, nodeoffset, "#size-cells");
	যদি (val == -FDT_ERR_NOTFOUND)
		ঘূৰাই 1;
	ঘূৰাই val;
}

/* This function assumes that [address|size]_cells is 1 or 2 */
পূর্ণাঙ্ক fdt_appendprop_addrrange(শূন্য *fdt, পূর্ণাঙ্ক parent, পূর্ণাঙ্ক nodeoffset,
			     ধ্রুৱ ৱর্ণ *name, uint64_t addr, uint64_t size)
{
	পূর্ণাঙ্ক addr_cells, size_cells, ret;
	uint8_t data[পরিমাণ(fdt64_t) * 2], *prop;

	ret = fdt_address_cells(fdt, parent);
	যদি (ret < 0)
		ঘূৰাই ret;
	addr_cells = ret;

	ret = fdt_size_cells(fdt, parent);
	যদি (ret < 0)
		ঘূৰাই ret;
	size_cells = ret;

	/* check validity of address */
	prop = data;
	যদি (addr_cells == 1) {
		যদি ((addr > UINT32_MAX) || ((UINT32_MAX + 1 - addr) < size))
			ঘূৰাই -FDT_ERR_BADVALUE;

		fdt32_st(prop, (uint32_t)addr);
	} অন্যথা যদি (addr_cells == 2) {
		fdt64_st(prop, addr);
	} অন্যথা {
		ঘূৰাই -FDT_ERR_BADNCELLS;
	}

	/* check validity of size */
	prop += addr_cells * পরিমাণ(fdt32_t);
	যদি (size_cells == 1) {
		যদি (size > UINT32_MAX)
			ঘূৰাই -FDT_ERR_BADVALUE;

		fdt32_st(prop, (uint32_t)size);
	} অন্যথা যদি (size_cells == 2) {
		fdt64_st(prop, size);
	} অন্যথা {
		ঘূৰাই -FDT_ERR_BADNCELLS;
	}

	ঘূৰাই fdt_appendprop(fdt, nodeoffset, name, data,
			      (addr_cells + size_cells) * পরিমাণ(fdt32_t));
}
