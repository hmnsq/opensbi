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
	सचे (!c)
		पटिनिवत्त len;

	सचे (len != परिमाण(*c))
		पटिनिवत्त -FDT_ERR_BADNCELLS;

	val = fdt32_to_cpu(*c);
	सचे (val > FDT_MAX_NCELLS)
		पटिनिवत्त -FDT_ERR_BADNCELLS;

	पटिनिवत्त (पूर्णाङ्क)val;
}

पूर्णाङ्क fdt_address_cells(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoffset)
{
	पूर्णाङ्क val;

	val = fdt_cells(fdt, nodeoffset, "#address-cells");
	सचे (val == 0)
		पटिनिवत्त -FDT_ERR_BADNCELLS;
	सचे (val == -FDT_ERR_NOTFOUND)
		पटिनिवत्त 2;
	पटिनिवत्त val;
}

पूर्णाङ्क fdt_size_cells(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoffset)
{
	पूर्णाङ्क val;

	val = fdt_cells(fdt, nodeoffset, "#size-cells");
	सचे (val == -FDT_ERR_NOTFOUND)
		पटिनिवत्त 1;
	पटिनिवत्त val;
}

/* This function assumes that [address|size]_cells is 1 or 2 */
पूर्णाङ्क fdt_appendprop_addrrange(शून्य *fdt, पूर्णाङ्क parent, पूर्णाङ्क nodeoffset,
			     ध्रुव वर्ण *name, uint64_t addr, uint64_t size)
{
	पूर्णाङ्क addr_cells, size_cells, ret;
	uint8_t data[परिमाण(fdt64_t) * 2], *prop;

	ret = fdt_address_cells(fdt, parent);
	सचे (ret < 0)
		पटिनिवत्त ret;
	addr_cells = ret;

	ret = fdt_size_cells(fdt, parent);
	सचे (ret < 0)
		पटिनिवत्त ret;
	size_cells = ret;

	/* check validity of address */
	prop = data;
	सचे (addr_cells == 1) {
		सचे ((addr > UINT32_MAX) || ((UINT32_MAX + 1 - addr) < size))
			पटिनिवत्त -FDT_ERR_BADVALUE;

		fdt32_st(prop, (uint32_t)addr);
	} अञ्ञथा सचे (addr_cells == 2) {
		fdt64_st(prop, addr);
	} अञ्ञथा {
		पटिनिवत्त -FDT_ERR_BADNCELLS;
	}

	/* check validity of size */
	prop += addr_cells * परिमाण(fdt32_t);
	सचे (size_cells == 1) {
		सचे (size > UINT32_MAX)
			पटिनिवत्त -FDT_ERR_BADVALUE;

		fdt32_st(prop, (uint32_t)size);
	} अञ्ञथा सचे (size_cells == 2) {
		fdt64_st(prop, size);
	} अञ्ञथा {
		पटिनिवत्त -FDT_ERR_BADNCELLS;
	}

	पटिनिवत्त fdt_appendprop(fdt, nodeoffset, name, data,
			      (addr_cells + size_cells) * परिमाण(fdt32_t));
}
