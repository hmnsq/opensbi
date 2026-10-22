// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2014 David Gibson <david@gibson.dropbear.id.au>
 * Copyright (C) 2018 embedded brains GmbH
 */
#ਅਨ੍ਤਰ੍ਭਾਵ "libfdt_env.h"

#ਅਨ੍ਤਰ੍ਭਾਵ <fdt.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <libfdt.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "libfdt_internal.h"

ਸਥਿਰ ਪੂਰਨ fdt_cells(ਅਚਲ ਖਾਲੀ *fdt, ਪੂਰਨ nodeoffset, ਅਚਲ ਅੱਖਰ *name)
{
	ਅਚਲ fdt32_t *c;
	uint32_t val;
	ਪੂਰਨ len;

	c = fdt_getprop(fdt, nodeoffset, name, &len);
	ਜੇ (!c)
		ਵਾਪਸ len;

	ਜੇ (len != ਆਕਾਰ(*c))
		ਵਾਪਸ -FDT_ERR_BADNCELLS;

	val = fdt32_ਮੰਜ਼ਿਲ_cpu(*c);
	ਜੇ (val > FDT_MAX_NCELLS)
		ਵਾਪਸ -FDT_ERR_BADNCELLS;

	ਵਾਪਸ (ਪੂਰਨ)val;
}

ਪੂਰਨ fdt_ਪਤਾ_cells(ਅਚਲ ਖਾਲੀ *fdt, ਪੂਰਨ nodeoffset)
{
	ਪੂਰਨ val;

	val = fdt_cells(fdt, nodeoffset, "#address-cells");
	ਜੇ (val == 0)
		ਵਾਪਸ -FDT_ERR_BADNCELLS;
	ਜੇ (val == -FDT_ERR_NOTFOUND)
		ਵਾਪਸ 2;
	ਵਾਪਸ val;
}

ਪੂਰਨ fdt_ਮਾਪ_cells(ਅਚਲ ਖਾਲੀ *fdt, ਪੂਰਨ nodeoffset)
{
	ਪੂਰਨ val;

	val = fdt_cells(fdt, nodeoffset, "#size-cells");
	ਜੇ (val == -FDT_ERR_NOTFOUND)
		ਵਾਪਸ 1;
	ਵਾਪਸ val;
}

/* This function assumes that [address|size]_cells is 1 or 2 */
ਪੂਰਨ fdt_appendprop_addrrange(ਖਾਲੀ *fdt, ਪੂਰਨ ਮਾਪਾ, ਪੂਰਨ nodeoffset,
			     ਅਚਲ ਅੱਖਰ *name, uint64_t addr, uint64_t ਮਾਪ)
{
	ਪੂਰਨ addr_cells, ਮਾਪ_cells, ret;
	uint8_t ਅੰਕੜੇ[ਆਕਾਰ(fdt64_t) * 2], *prop;

	ret = fdt_ਪਤਾ_cells(fdt, ਮਾਪਾ);
	ਜੇ (ret < 0)
		ਵਾਪਸ ret;
	addr_cells = ret;

	ret = fdt_ਮਾਪ_cells(fdt, ਮਾਪਾ);
	ਜੇ (ret < 0)
		ਵਾਪਸ ret;
	ਮਾਪ_cells = ret;

	/* check validity of address */
	prop = ਅੰਕੜੇ;
	ਜੇ (addr_cells == 1) {
		ਜੇ ((addr > UINT32_MAX) || ((UINT32_MAX + 1 - addr) < ਮਾਪ))
			ਵਾਪਸ -FDT_ERR_BADVALUE;

		fdt32_st(prop, (uint32_t)addr);
	} ਨਹੀਂਤਾਂ ਜੇ (addr_cells == 2) {
		fdt64_st(prop, addr);
	} ਨਹੀਂਤਾਂ {
		ਵਾਪਸ -FDT_ERR_BADNCELLS;
	}

	/* check validity of size */
	prop += addr_cells * ਆਕਾਰ(fdt32_t);
	ਜੇ (ਮਾਪ_cells == 1) {
		ਜੇ (ਮਾਪ > UINT32_MAX)
			ਵਾਪਸ -FDT_ERR_BADVALUE;

		fdt32_st(prop, (uint32_t)ਮਾਪ);
	} ਨਹੀਂਤਾਂ ਜੇ (ਮਾਪ_cells == 2) {
		fdt64_st(prop, ਮਾਪ);
	} ਨਹੀਂਤਾਂ {
		ਵਾਪਸ -FDT_ERR_BADNCELLS;
	}

	ਵਾਪਸ fdt_appendprop(fdt, nodeoffset, name, ਅੰਕੜੇ,
			      (addr_cells + ਮਾਪ_cells) * ਆਕਾਰ(fdt32_t));
}
