// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2014 David Gibson <david@gibson.dropbear.id.au>
 * Copyright (C) 2018 embedded brains GmbH
 */
#അന്തര്ഭാവ "libfdt_env.h"

#അന്തര്ഭാവ <fdt.h>
#അന്തര്ഭാവ <libfdt.h>

#അന്തര്ഭാവ "libfdt_internal.h"

സ്ഥിതം പൂർണ്ണസംഖ്യ fdt_cells(സ്ഥിരം ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ nodeoffset, സ്ഥിരം അക്ഷരം *name)
{
	സ്ഥിരം fdt32_t *c;
	uint32_t val;
	പൂർണ്ണസംഖ്യ len;

	c = fdt_getprop(fdt, nodeoffset, name, &len);
	എങ്കിൽ (!c)
		തിരികെ len;

	എങ്കിൽ (len != വലുപ്പം(*c))
		തിരികെ -FDT_ERR_BADNCELLS;

	val = fdt32_to_cpu(*c);
	എങ്കിൽ (val > FDT_MAX_NCELLS)
		തിരികെ -FDT_ERR_BADNCELLS;

	തിരികെ (പൂർണ്ണസംഖ്യ)val;
}

പൂർണ്ണസംഖ്യ fdt_address_cells(സ്ഥിരം ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ nodeoffset)
{
	പൂർണ്ണസംഖ്യ val;

	val = fdt_cells(fdt, nodeoffset, "#address-cells");
	എങ്കിൽ (val == 0)
		തിരികെ -FDT_ERR_BADNCELLS;
	എങ്കിൽ (val == -FDT_ERR_NOTFOUND)
		തിരികെ 2;
	തിരികെ val;
}

പൂർണ്ണസംഖ്യ fdt_size_cells(സ്ഥിരം ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ nodeoffset)
{
	പൂർണ്ണസംഖ്യ val;

	val = fdt_cells(fdt, nodeoffset, "#size-cells");
	എങ്കിൽ (val == -FDT_ERR_NOTFOUND)
		തിരികെ 1;
	തിരികെ val;
}

/* This function assumes that [address|size]_cells is 1 or 2 */
പൂർണ്ണസംഖ്യ fdt_appendprop_addrrange(ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ parent, പൂർണ്ണസംഖ്യ nodeoffset,
			     സ്ഥിരം അക്ഷരം *name, uint64_t addr, uint64_t size)
{
	പൂർണ്ണസംഖ്യ addr_cells, size_cells, ret;
	uint8_t data[വലുപ്പം(fdt64_t) * 2], *prop;

	ret = fdt_address_cells(fdt, parent);
	എങ്കിൽ (ret < 0)
		തിരികെ ret;
	addr_cells = ret;

	ret = fdt_size_cells(fdt, parent);
	എങ്കിൽ (ret < 0)
		തിരികെ ret;
	size_cells = ret;

	/* check validity of address */
	prop = data;
	എങ്കിൽ (addr_cells == 1) {
		എങ്കിൽ ((addr > UINT32_MAX) || ((UINT32_MAX + 1 - addr) < size))
			തിരികെ -FDT_ERR_BADVALUE;

		fdt32_st(prop, (uint32_t)addr);
	} അല്ലെങ്കിൽ എങ്കിൽ (addr_cells == 2) {
		fdt64_st(prop, addr);
	} അല്ലെങ്കിൽ {
		തിരികെ -FDT_ERR_BADNCELLS;
	}

	/* check validity of size */
	prop += addr_cells * വലുപ്പം(fdt32_t);
	എങ്കിൽ (size_cells == 1) {
		എങ്കിൽ (size > UINT32_MAX)
			തിരികെ -FDT_ERR_BADVALUE;

		fdt32_st(prop, (uint32_t)size);
	} അല്ലെങ്കിൽ എങ്കിൽ (size_cells == 2) {
		fdt64_st(prop, size);
	} അല്ലെങ്കിൽ {
		തിരികെ -FDT_ERR_BADNCELLS;
	}

	തിരികെ fdt_appendprop(fdt, nodeoffset, name, data,
			      (addr_cells + size_cells) * വലുപ്പം(fdt32_t));
}
