// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2014 David Gibson <david@gibson.dropbear.id.au>
 * Copyright (C) 2018 embedded brains GmbH
 */
#અન્તર્ભાવ "libfdt_env.h"

#અન્તર્ભાવ <fdt.h>
#અન્તર્ભાવ <libfdt.h>

#અન્તર્ભાવ "libfdt_internal.h"

સ્થિર પૂર્ણાંક fdt_cells(અચલ શૂન્ય *fdt, પૂર્ણાંક nodeoffset, અચલ અક્ષર *name)
{
	અચલ fdt32_t *c;
	uint32_t val;
	પૂર્ણાંક len;

	c = fdt_getprop(fdt, nodeoffset, name, &len);
	જો (!c)
		પાછા len;

	જો (len != કદ(*c))
		પાછા -FDT_ERR_BADNCELLS;

	val = fdt32_to_cpu(*c);
	જો (val > FDT_MAX_NCELLS)
		પાછા -FDT_ERR_BADNCELLS;

	પાછા (પૂર્ણાંક)val;
}

પૂર્ણાંક fdt_address_cells(અચલ શૂન્ય *fdt, પૂર્ણાંક nodeoffset)
{
	પૂર્ણાંક val;

	val = fdt_cells(fdt, nodeoffset, "#address-cells");
	જો (val == 0)
		પાછા -FDT_ERR_BADNCELLS;
	જો (val == -FDT_ERR_NOTFOUND)
		પાછા 2;
	પાછા val;
}

પૂર્ણાંક fdt_size_cells(અચલ શૂન્ય *fdt, પૂર્ણાંક nodeoffset)
{
	પૂર્ણાંક val;

	val = fdt_cells(fdt, nodeoffset, "#size-cells");
	જો (val == -FDT_ERR_NOTFOUND)
		પાછા 1;
	પાછા val;
}

/* This function assumes that [address|size]_cells is 1 or 2 */
પૂર્ણાંક fdt_appendprop_addrrange(શૂન્ય *fdt, પૂર્ણાંક parent, પૂર્ણાંક nodeoffset,
			     અચલ અક્ષર *name, uint64_t addr, uint64_t size)
{
	પૂર્ણાંક addr_cells, size_cells, ret;
	uint8_t data[કદ(fdt64_t) * 2], *prop;

	ret = fdt_address_cells(fdt, parent);
	જો (ret < 0)
		પાછા ret;
	addr_cells = ret;

	ret = fdt_size_cells(fdt, parent);
	જો (ret < 0)
		પાછા ret;
	size_cells = ret;

	/* check validity of address */
	prop = data;
	જો (addr_cells == 1) {
		જો ((addr > UINT32_MAX) || ((UINT32_MAX + 1 - addr) < size))
			પાછા -FDT_ERR_BADVALUE;

		fdt32_st(prop, (uint32_t)addr);
	} નહીંતર જો (addr_cells == 2) {
		fdt64_st(prop, addr);
	} નહીંતર {
		પાછા -FDT_ERR_BADNCELLS;
	}

	/* check validity of size */
	prop += addr_cells * કદ(fdt32_t);
	જો (size_cells == 1) {
		જો (size > UINT32_MAX)
			પાછા -FDT_ERR_BADVALUE;

		fdt32_st(prop, (uint32_t)size);
	} નહીંતર જો (size_cells == 2) {
		fdt64_st(prop, size);
	} નહીંતર {
		પાછા -FDT_ERR_BADNCELLS;
	}

	પાછા fdt_appendprop(fdt, nodeoffset, name, data,
			      (addr_cells + size_cells) * કદ(fdt32_t));
}
