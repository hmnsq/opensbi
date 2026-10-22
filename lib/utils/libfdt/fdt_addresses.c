// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2014 David Gibson <david@gibson.dropbear.id.au>
 * Copyright (C) 2018 embedded brains GmbH
 */
#ಅನ್ತರ್ಭಾವ "libfdt_env.h"

#ಅನ್ತರ್ಭಾವ <fdt.h>
#ಅನ್ತರ್ಭಾವ <libfdt.h>

#ಅನ್ತರ್ಭಾವ "libfdt_internal.h"

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ fdt_cells(ಧ್ರುವ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ nodeoffset, ಧ್ರುವ ವರ್ಣ *name)
{
	ಧ್ರುವ fdt32_t *c;
	uint32_t val;
	ಪೂರ್ಣಾಙ್ಕ len;

	c = fdt_getprop(fdt, nodeoffset, name, &len);
	ಯದಿ (!c)
		ಪ್ರತ್ಯಾಗಮ len;

	ಯದಿ (len != ಪರಿಮಾಣ(*c))
		ಪ್ರತ್ಯಾಗಮ -FDT_ERR_BADNCELLS;

	val = fdt32_to_cpu(*c);
	ಯದಿ (val > FDT_MAX_NCELLS)
		ಪ್ರತ್ಯಾಗಮ -FDT_ERR_BADNCELLS;

	ಪ್ರತ್ಯಾಗಮ (ಪೂರ್ಣಾಙ್ಕ)val;
}

ಪೂರ್ಣಾಙ್ಕ fdt_address_cells(ಧ್ರುವ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ nodeoffset)
{
	ಪೂರ್ಣಾಙ್ಕ val;

	val = fdt_cells(fdt, nodeoffset, "#address-cells");
	ಯದಿ (val == 0)
		ಪ್ರತ್ಯಾಗಮ -FDT_ERR_BADNCELLS;
	ಯದಿ (val == -FDT_ERR_NOTFOUND)
		ಪ್ರತ್ಯಾಗಮ 2;
	ಪ್ರತ್ಯಾಗಮ val;
}

ಪೂರ್ಣಾಙ್ಕ fdt_size_cells(ಧ್ರುವ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ nodeoffset)
{
	ಪೂರ್ಣಾಙ್ಕ val;

	val = fdt_cells(fdt, nodeoffset, "#size-cells");
	ಯದಿ (val == -FDT_ERR_NOTFOUND)
		ಪ್ರತ್ಯಾಗಮ 1;
	ಪ್ರತ್ಯಾಗಮ val;
}

/* This function assumes that [address|size]_cells is 1 or 2 */
ಪೂರ್ಣಾಙ್ಕ fdt_appendprop_addrrange(ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ parent, ಪೂರ್ಣಾಙ್ಕ nodeoffset,
			     ಧ್ರುವ ವರ್ಣ *name, uint64_t addr, uint64_t size)
{
	ಪೂರ್ಣಾಙ್ಕ addr_cells, size_cells, ret;
	uint8_t data[ಪರಿಮಾಣ(fdt64_t) * 2], *prop;

	ret = fdt_address_cells(fdt, parent);
	ಯದಿ (ret < 0)
		ಪ್ರತ್ಯಾಗಮ ret;
	addr_cells = ret;

	ret = fdt_size_cells(fdt, parent);
	ಯದಿ (ret < 0)
		ಪ್ರತ್ಯಾಗಮ ret;
	size_cells = ret;

	/* check validity of address */
	prop = data;
	ಯದಿ (addr_cells == 1) {
		ಯದಿ ((addr > UINT32_MAX) || ((UINT32_MAX + 1 - addr) < size))
			ಪ್ರತ್ಯಾಗಮ -FDT_ERR_BADVALUE;

		fdt32_st(prop, (uint32_t)addr);
	} ಅನ್ಯಥಾ ಯದಿ (addr_cells == 2) {
		fdt64_st(prop, addr);
	} ಅನ್ಯಥಾ {
		ಪ್ರತ್ಯಾಗಮ -FDT_ERR_BADNCELLS;
	}

	/* check validity of size */
	prop += addr_cells * ಪರಿಮಾಣ(fdt32_t);
	ಯದಿ (size_cells == 1) {
		ಯದಿ (size > UINT32_MAX)
			ಪ್ರತ್ಯಾಗಮ -FDT_ERR_BADVALUE;

		fdt32_st(prop, (uint32_t)size);
	} ಅನ್ಯಥಾ ಯದಿ (size_cells == 2) {
		fdt64_st(prop, size);
	} ಅನ್ಯಥಾ {
		ಪ್ರತ್ಯಾಗಮ -FDT_ERR_BADNCELLS;
	}

	ಪ್ರತ್ಯಾಗಮ fdt_appendprop(fdt, nodeoffset, name, data,
			      (addr_cells + size_cells) * ಪರಿಮಾಣ(fdt32_t));
}
