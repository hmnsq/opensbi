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

स्थिर पूर्णाङ्क fdt_cells(अचर शून्य *fdt, पूर्णाङ्क nodeoffset, अचर वर्ण *name)
{
	अचर fdt32_t *c;
	uint32_t val;
	पूर्णाङ्क len;

	c = fdt_getprop(fdt, nodeoffset, name, &len);
	यदि (!c)
		फर्काऊ len;

	यदि (len != परिमाण(*c))
		फर्काऊ -FDT_ERR_BADNCELLS;

	val = fdt32_to_cpu(*c);
	यदि (val > FDT_MAX_NCELLS)
		फर्काऊ -FDT_ERR_BADNCELLS;

	फर्काऊ (पूर्णाङ्क)val;
}

पूर्णाङ्क fdt_address_cells(अचर शून्य *fdt, पूर्णाङ्क nodeoffset)
{
	पूर्णाङ्क val;

	val = fdt_cells(fdt, nodeoffset, "#address-cells");
	यदि (val == 0)
		फर्काऊ -FDT_ERR_BADNCELLS;
	यदि (val == -FDT_ERR_NOTFOUND)
		फर्काऊ 2;
	फर्काऊ val;
}

पूर्णाङ्क fdt_size_cells(अचर शून्य *fdt, पूर्णाङ्क nodeoffset)
{
	पूर्णाङ्क val;

	val = fdt_cells(fdt, nodeoffset, "#size-cells");
	यदि (val == -FDT_ERR_NOTFOUND)
		फर्काऊ 1;
	फर्काऊ val;
}

/* This function assumes that [address|size]_cells is 1 or 2 */
पूर्णाङ्क fdt_appendprop_addrrange(शून्य *fdt, पूर्णाङ्क parent, पूर्णाङ्क nodeoffset,
			     अचर वर्ण *name, uint64_t addr, uint64_t size)
{
	पूर्णाङ्क addr_cells, size_cells, ret;
	uint8_t data[परिमाण(fdt64_t) * 2], *prop;

	ret = fdt_address_cells(fdt, parent);
	यदि (ret < 0)
		फर्काऊ ret;
	addr_cells = ret;

	ret = fdt_size_cells(fdt, parent);
	यदि (ret < 0)
		फर्काऊ ret;
	size_cells = ret;

	/* check validity of address */
	prop = data;
	यदि (addr_cells == 1) {
		यदि ((addr > UINT32_MAX) || ((UINT32_MAX + 1 - addr) < size))
			फर्काऊ -FDT_ERR_BADVALUE;

		fdt32_st(prop, (uint32_t)addr);
	} अन्यथा यदि (addr_cells == 2) {
		fdt64_st(prop, addr);
	} अन्यथा {
		फर्काऊ -FDT_ERR_BADNCELLS;
	}

	/* check validity of size */
	prop += addr_cells * परिमाण(fdt32_t);
	यदि (size_cells == 1) {
		यदि (size > UINT32_MAX)
			फर्काऊ -FDT_ERR_BADVALUE;

		fdt32_st(prop, (uint32_t)size);
	} अन्यथा यदि (size_cells == 2) {
		fdt64_st(prop, size);
	} अन्यथा {
		फर्काऊ -FDT_ERR_BADNCELLS;
	}

	फर्काऊ fdt_appendprop(fdt, nodeoffset, name, data,
			      (addr_cells + size_cells) * परिमाण(fdt32_t));
}
