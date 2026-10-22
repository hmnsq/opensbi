// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2014 David Gibson <david@gibson.dropbear.id.au>
 * Copyright (C) 2018 embedded brains GmbH
 */
#समावेश "libfdt_env.h"

#समावेश <fdt.h>
#समावेश <libfdt.h>

#समावेश "libfdt_internal.h"

अटल पूर्णांक fdt_cells(स्थिर व्योम *fdt, पूर्णांक nodeoffset, स्थिर अक्षर *name)
{
	स्थिर fdt32_t *c;
	uint32_t val;
	पूर्णांक len;

	c = fdt_getprop(fdt, nodeoffset, name, &len);
	अगर (!c)
		वापस len;

	अगर (len != माप(*c))
		वापस -FDT_ERR_BADNCELLS;

	val = fdt32_to_cpu(*c);
	अगर (val > FDT_MAX_NCELLS)
		वापस -FDT_ERR_BADNCELLS;

	वापस (पूर्णांक)val;
}

पूर्णांक fdt_address_cells(स्थिर व्योम *fdt, पूर्णांक nodeoffset)
{
	पूर्णांक val;

	val = fdt_cells(fdt, nodeoffset, "#address-cells");
	अगर (val == 0)
		वापस -FDT_ERR_BADNCELLS;
	अगर (val == -FDT_ERR_NOTFOUND)
		वापस 2;
	वापस val;
}

पूर्णांक fdt_आकार_cells(स्थिर व्योम *fdt, पूर्णांक nodeoffset)
{
	पूर्णांक val;

	val = fdt_cells(fdt, nodeoffset, "#size-cells");
	अगर (val == -FDT_ERR_NOTFOUND)
		वापस 1;
	वापस val;
}

/* This function assumes that [address|size]_cells is 1 or 2 */
पूर्णांक fdt_appendprop_addrrange(व्योम *fdt, पूर्णांक parent, पूर्णांक nodeoffset,
			     स्थिर अक्षर *name, uint64_t addr, uint64_t आकार)
{
	पूर्णांक addr_cells, आकार_cells, ret;
	uint8_t तथ्य[माप(fdt64_t) * 2], *prop;

	ret = fdt_address_cells(fdt, parent);
	अगर (ret < 0)
		वापस ret;
	addr_cells = ret;

	ret = fdt_आकार_cells(fdt, parent);
	अगर (ret < 0)
		वापस ret;
	आकार_cells = ret;

	/* check validity of address */
	prop = तथ्य;
	अगर (addr_cells == 1) {
		अगर ((addr > UINT32_MAX) || ((UINT32_MAX + 1 - addr) < आकार))
			वापस -FDT_ERR_BADVALUE;

		fdt32_st(prop, (uint32_t)addr);
	} अन्यथा अगर (addr_cells == 2) {
		fdt64_st(prop, addr);
	} अन्यथा {
		वापस -FDT_ERR_BADNCELLS;
	}

	/* check validity of size */
	prop += addr_cells * माप(fdt32_t);
	अगर (आकार_cells == 1) {
		अगर (आकार > UINT32_MAX)
			वापस -FDT_ERR_BADVALUE;

		fdt32_st(prop, (uint32_t)आकार);
	} अन्यथा अगर (आकार_cells == 2) {
		fdt64_st(prop, आकार);
	} अन्यथा {
		वापस -FDT_ERR_BADNCELLS;
	}

	वापस fdt_appendprop(fdt, nodeoffset, name, तथ्य,
			      (addr_cells + आकार_cells) * माप(fdt32_t));
}
