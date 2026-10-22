// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2014 David Gibson <david@gibson.dropbear.id.au>
 * Copyright (C) 2018 embedded brains GmbH
 */
#अन्तर्भाव "libfdt_env.h"

#अन्तर्भाव <fdt.h>
#अन्तर्भाव <libfdt.h>

#अन्तर्भाव "libfdt_internal.h"

स्थिर पूर्णाङ्क fdt_cells(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoffset, ध्रुव वर्ण *name)
{
	ध्रुव fdt32_t *c;
	uint32_t val;
	पूर्णाङ्क len;

	c = fdt_getprop(fdt, nodeoffset, name, &len);
	जइ (!c)
		प्रत्यागम len;

	जइ (len != परिमाण(*c))
		प्रत्यागम -FDT_ERR_BADNCELLS;

	val = fdt32_to_cpu(*c);
	जइ (val > FDT_MAX_NCELLS)
		प्रत्यागम -FDT_ERR_BADNCELLS;

	प्रत्यागम (पूर्णाङ्क)val;
}

पूर्णाङ्क fdt_address_cells(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoffset)
{
	पूर्णाङ्क val;

	val = fdt_cells(fdt, nodeoffset, "#address-cells");
	जइ (val == 0)
		प्रत्यागम -FDT_ERR_BADNCELLS;
	जइ (val == -FDT_ERR_NOTFOUND)
		प्रत्यागम 2;
	प्रत्यागम val;
}

पूर्णाङ्क fdt_size_cells(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoffset)
{
	पूर्णाङ्क val;

	val = fdt_cells(fdt, nodeoffset, "#size-cells");
	जइ (val == -FDT_ERR_NOTFOUND)
		प्रत्यागम 1;
	प्रत्यागम val;
}

/* This function assumes that [address|size]_cells is 1 or 2 */
पूर्णाङ्क fdt_appendprop_addrrange(शून्य *fdt, पूर्णाङ्क parent, पूर्णाङ्क nodeoffset,
			     ध्रुव वर्ण *name, uint64_t addr, uint64_t size)
{
	पूर्णाङ्क addr_cells, size_cells, ret;
	uint8_t data[परिमाण(fdt64_t) * 2], *prop;

	ret = fdt_address_cells(fdt, parent);
	जइ (ret < 0)
		प्रत्यागम ret;
	addr_cells = ret;

	ret = fdt_size_cells(fdt, parent);
	जइ (ret < 0)
		प्रत्यागम ret;
	size_cells = ret;

	/* check validity of address */
	prop = data;
	जइ (addr_cells == 1) {
		जइ ((addr > UINT32_MAX) || ((UINT32_MAX + 1 - addr) < size))
			प्रत्यागम -FDT_ERR_BADVALUE;

		fdt32_st(prop, (uint32_t)addr);
	} अण्णहा जइ (addr_cells == 2) {
		fdt64_st(prop, addr);
	} अण्णहा {
		प्रत्यागम -FDT_ERR_BADNCELLS;
	}

	/* check validity of size */
	prop += addr_cells * परिमाण(fdt32_t);
	जइ (size_cells == 1) {
		जइ (size > UINT32_MAX)
			प्रत्यागम -FDT_ERR_BADVALUE;

		fdt32_st(prop, (uint32_t)size);
	} अण्णहा जइ (size_cells == 2) {
		fdt64_st(prop, size);
	} अण्णहा {
		प्रत्यागम -FDT_ERR_BADNCELLS;
	}

	प्रत्यागम fdt_appendprop(fdt, nodeoffset, name, data,
			      (addr_cells + size_cells) * परिमाण(fdt32_t));
}
