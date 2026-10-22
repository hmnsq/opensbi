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
	यदि (!c)
		प्रत्यागम len;

	यदि (len != परिमाण(*c))
		प्रत्यागम -FDT_ERR_BADNCELLS;

	val = fdt32_गन्तव्यम्_cpu(*c);
	यदि (val > FDT_MAX_NCELLS)
		प्रत्यागम -FDT_ERR_BADNCELLS;

	प्रत्यागम (पूर्णाङ्क)val;
}

पूर्णाङ्क fdt_स्थानाङ्कः_cells(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoffset)
{
	पूर्णाङ्क val;

	val = fdt_cells(fdt, nodeoffset, "#address-cells");
	यदि (val == 0)
		प्रत्यागम -FDT_ERR_BADNCELLS;
	यदि (val == -FDT_ERR_NOTFOUND)
		प्रत्यागम 2;
	प्रत्यागम val;
}

पूर्णाङ्क fdt_आकार_cells(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoffset)
{
	पूर्णाङ्क val;

	val = fdt_cells(fdt, nodeoffset, "#size-cells");
	यदि (val == -FDT_ERR_NOTFOUND)
		प्रत्यागम 1;
	प्रत्यागम val;
}

/* This function assumes that [address|size]_cells is 1 or 2 */
पूर्णाङ्क fdt_appendprop_addrrange(शून्य *fdt, पूर्णाङ्क parent, पूर्णाङ्क nodeoffset,
			     ध्रुव वर्ण *name, uint64_t addr, uint64_t आकार)
{
	पूर्णाङ्क addr_cells, आकार_cells, ret;
	uint8_t दत्त[परिमाण(fdt64_t) * 2], *prop;

	ret = fdt_स्थानाङ्कः_cells(fdt, parent);
	यदि (ret < 0)
		प्रत्यागम ret;
	addr_cells = ret;

	ret = fdt_आकार_cells(fdt, parent);
	यदि (ret < 0)
		प्रत्यागम ret;
	आकार_cells = ret;

	/* check validity of address */
	prop = दत्त;
	यदि (addr_cells == 1) {
		यदि ((addr > UINT32_MAX) || ((UINT32_MAX + 1 - addr) < आकार))
			प्रत्यागम -FDT_ERR_BADVALUE;

		fdt32_st(prop, (uint32_t)addr);
	} अन्यथा यदि (addr_cells == 2) {
		fdt64_st(prop, addr);
	} अन्यथा {
		प्रत्यागम -FDT_ERR_BADNCELLS;
	}

	/* check validity of size */
	prop += addr_cells * परिमाण(fdt32_t);
	यदि (आकार_cells == 1) {
		यदि (आकार > UINT32_MAX)
			प्रत्यागम -FDT_ERR_BADVALUE;

		fdt32_st(prop, (uint32_t)आकार);
	} अन्यथा यदि (आकार_cells == 2) {
		fdt64_st(prop, आकार);
	} अन्यथा {
		प्रत्यागम -FDT_ERR_BADNCELLS;
	}

	प्रत्यागम fdt_appendprop(fdt, nodeoffset, name, दत्त,
			      (addr_cells + आकार_cells) * परिमाण(fdt32_t));
}
