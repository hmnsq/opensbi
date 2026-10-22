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

स्थिर पूर्णांक fdt_cells(स्थिरांक शून्य *fdt, पूर्णांक nodeoffset, स्थिरांक अक्षर *name)
{
	स्थिरांक fdt32_t *c;
	uint32_t val;
	पूर्णांक len;

	c = fdt_getprop(fdt, nodeoffset, name, &len);
	जर (!c)
		परत len;

	जर (len != आकार(*c))
		परत -FDT_ERR_BADNCELLS;

	val = fdt32_to_cpu(*c);
	जर (val > FDT_MAX_NCELLS)
		परत -FDT_ERR_BADNCELLS;

	परत (पूर्णांक)val;
}

पूर्णांक fdt_address_cells(स्थिरांक शून्य *fdt, पूर्णांक nodeoffset)
{
	पूर्णांक val;

	val = fdt_cells(fdt, nodeoffset, "#address-cells");
	जर (val == 0)
		परत -FDT_ERR_BADNCELLS;
	जर (val == -FDT_ERR_NOTFOUND)
		परत 2;
	परत val;
}

पूर्णांक fdt_size_cells(स्थिरांक शून्य *fdt, पूर्णांक nodeoffset)
{
	पूर्णांक val;

	val = fdt_cells(fdt, nodeoffset, "#size-cells");
	जर (val == -FDT_ERR_NOTFOUND)
		परत 1;
	परत val;
}

/* This function assumes that [address|size]_cells is 1 or 2 */
पूर्णांक fdt_appendprop_addrrange(शून्य *fdt, पूर्णांक parent, पूर्णांक nodeoffset,
			     स्थिरांक अक्षर *name, uint64_t addr, uint64_t size)
{
	पूर्णांक addr_cells, size_cells, ret;
	uint8_t data[आकार(fdt64_t) * 2], *prop;

	ret = fdt_address_cells(fdt, parent);
	जर (ret < 0)
		परत ret;
	addr_cells = ret;

	ret = fdt_size_cells(fdt, parent);
	जर (ret < 0)
		परत ret;
	size_cells = ret;

	/* check validity of address */
	prop = data;
	जर (addr_cells == 1) {
		जर ((addr > UINT32_MAX) || ((UINT32_MAX + 1 - addr) < size))
			परत -FDT_ERR_BADVALUE;

		fdt32_st(prop, (uint32_t)addr);
	} अन्यथा जर (addr_cells == 2) {
		fdt64_st(prop, addr);
	} अन्यथा {
		परत -FDT_ERR_BADNCELLS;
	}

	/* check validity of size */
	prop += addr_cells * आकार(fdt32_t);
	जर (size_cells == 1) {
		जर (size > UINT32_MAX)
			परत -FDT_ERR_BADVALUE;

		fdt32_st(prop, (uint32_t)size);
	} अन्यथा जर (size_cells == 2) {
		fdt64_st(prop, size);
	} अन्यथा {
		परत -FDT_ERR_BADNCELLS;
	}

	परत fdt_appendprop(fdt, nodeoffset, name, data,
			      (addr_cells + size_cells) * आकार(fdt32_t));
}
