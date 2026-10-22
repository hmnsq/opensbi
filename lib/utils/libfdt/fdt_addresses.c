// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2014 David Gibson <david@gibson.dropbear.id.au>
 * Copyright (C) 2018 embedded brains GmbH
 */
#অন্তর্ভাব "libfdt_env.h"

#অন্তর্ভাব <fdt.h>
#অন্তর্ভাব <libfdt.h>

#অন্তর্ভাব "libfdt_internal.h"

স্থির পূর্ণসংখ্যা fdt_cells(ধ্রুবক শূন্য *fdt, পূর্ণসংখ্যা nodeoffset, ধ্রুবক অক্ষর *name)
{
	ধ্রুবক fdt32_t *c;
	uint32_t val;
	পূর্ণসংখ্যা len;

	c = fdt_getprop(fdt, nodeoffset, name, &len);
	যদি (!c)
		ফেরত len;

	যদি (len != আকার(*c))
		ফেরত -FDT_ERR_BADNCELLS;

	val = fdt32_to_cpu(*c);
	যদি (val > FDT_MAX_NCELLS)
		ফেরত -FDT_ERR_BADNCELLS;

	ফেরত (পূর্ণসংখ্যা)val;
}

পূর্ণসংখ্যা fdt_address_cells(ধ্রুবক শূন্য *fdt, পূর্ণসংখ্যা nodeoffset)
{
	পূর্ণসংখ্যা val;

	val = fdt_cells(fdt, nodeoffset, "#address-cells");
	যদি (val == 0)
		ফেরত -FDT_ERR_BADNCELLS;
	যদি (val == -FDT_ERR_NOTFOUND)
		ফেরত 2;
	ফেরত val;
}

পূর্ণসংখ্যা fdt_আকার_cells(ধ্রুবক শূন্য *fdt, পূর্ণসংখ্যা nodeoffset)
{
	পূর্ণসংখ্যা val;

	val = fdt_cells(fdt, nodeoffset, "#size-cells");
	যদি (val == -FDT_ERR_NOTFOUND)
		ফেরত 1;
	ফেরত val;
}

/* This function assumes that [address|size]_cells is 1 or 2 */
পূর্ণসংখ্যা fdt_appendprop_addrrange(শূন্য *fdt, পূর্ণসংখ্যা parent, পূর্ণসংখ্যা nodeoffset,
			     ধ্রুবক অক্ষর *name, uint64_t addr, uint64_t আকার_2)
{
	পূর্ণসংখ্যা addr_cells, আকার_cells, ret;
	uint8_t তথ্য[আকার(fdt64_t) * 2], *prop;

	ret = fdt_address_cells(fdt, parent);
	যদি (ret < 0)
		ফেরত ret;
	addr_cells = ret;

	ret = fdt_আকার_cells(fdt, parent);
	যদি (ret < 0)
		ফেরত ret;
	আকার_cells = ret;

	/* check validity of address */
	prop = তথ্য;
	যদি (addr_cells == 1) {
		যদি ((addr > UINT32_MAX) || ((UINT32_MAX + 1 - addr) < আকার_2))
			ফেরত -FDT_ERR_BADVALUE;

		fdt32_st(prop, (uint32_t)addr);
	} অন্যথা যদি (addr_cells == 2) {
		fdt64_st(prop, addr);
	} অন্যথা {
		ফেরত -FDT_ERR_BADNCELLS;
	}

	/* check validity of size */
	prop += addr_cells * আকার(fdt32_t);
	যদি (আকার_cells == 1) {
		যদি (আকার_2 > UINT32_MAX)
			ফেরত -FDT_ERR_BADVALUE;

		fdt32_st(prop, (uint32_t)আকার_2);
	} অন্যথা যদি (আকার_cells == 2) {
		fdt64_st(prop, আকার_2);
	} অন্যথা {
		ফেরত -FDT_ERR_BADNCELLS;
	}

	ফেরত fdt_appendprop(fdt, nodeoffset, name, তথ্য,
			      (addr_cells + আকার_cells) * আকার(fdt32_t));
}
