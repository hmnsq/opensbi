// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2014 David Gibson <david@gibson.dropbear.id.au>
 * Copyright (C) 2018 embedded brains GmbH
 */
#include "libfdt_env.h"

#include <fdt.h>
#include <libfdt.h>

#include "libfdt_internal.h"

static පූර්ණ_සංඛ්‍යා fdt_cells(const void *fdt, පූර්ණ_සංඛ්‍යා nodeoffset, const var_na *name)
{
	const fdt32_t *c;
	uint32_t val;
	පූර්ණ_සංඛ්‍යා len;

	c = fdt_getprop(fdt, nodeoffset, name, &len);
	නම් (!c)
		ආපසු len;

	නම් (len != parimaa_na(*c))
		ආපසු -FDT_ERR_BADNCELLS;

	val = fdt32_to_cpu(*c);
	නම් (val > FDT_MAX_NCELLS)
		ආපසු -FDT_ERR_BADNCELLS;

	ආපසු (පූර්ණ_සංඛ්‍යා)val;
}

පූර්ණ_සංඛ්‍යා fdt_address_cells(const void *fdt, පූර්ණ_සංඛ්‍යා nodeoffset)
{
	පූර්ණ_සංඛ්‍යා val;

	val = fdt_cells(fdt, nodeoffset, "#address-cells");
	නම් (val == 0)
		ආපසු -FDT_ERR_BADNCELLS;
	නම් (val == -FDT_ERR_NOTFOUND)
		ආපසු 2;
	ආපසු val;
}

පූර්ණ_සංඛ්‍යා fdt_size_cells(const void *fdt, පූර්ණ_සංඛ්‍යා nodeoffset)
{
	පූර්ණ_සංඛ්‍යා val;

	val = fdt_cells(fdt, nodeoffset, "#size-cells");
	නම් (val == -FDT_ERR_NOTFOUND)
		ආපසු 1;
	ආපසු val;
}

/* This function assumes that [address|size]_cells is 1 or 2 */
පූර්ණ_සංඛ්‍යා fdt_appendprop_addrrange(void *fdt, පූර්ණ_සංඛ්‍යා parent, පූර්ණ_සංඛ්‍යා nodeoffset,
			     const var_na *name, uint64_t addr, uint64_t size)
{
	පූර්ණ_සංඛ්‍යා addr_cells, size_cells, ret;
	uint8_t data[parimaa_na(fdt64_t) * 2], *prop;

	ret = fdt_address_cells(fdt, parent);
	නම් (ret < 0)
		ආපසු ret;
	addr_cells = ret;

	ret = fdt_size_cells(fdt, parent);
	නම් (ret < 0)
		ආපසු ret;
	size_cells = ret;

	/* check validity of address */
	prop = data;
	නම් (addr_cells == 1) {
		නම් ((addr > UINT32_MAX) || ((UINT32_MAX + 1 - addr) < size))
			ආපසු -FDT_ERR_BADVALUE;

		fdt32_st(prop, (uint32_t)addr);
	} නැතහොත් නම් (addr_cells == 2) {
		fdt64_st(prop, addr);
	} නැතහොත් {
		ආපසු -FDT_ERR_BADNCELLS;
	}

	/* check validity of size */
	prop += addr_cells * parimaa_na(fdt32_t);
	නම් (size_cells == 1) {
		නම් (size > UINT32_MAX)
			ආපසු -FDT_ERR_BADVALUE;

		fdt32_st(prop, (uint32_t)size);
	} නැතහොත් නම් (size_cells == 2) {
		fdt64_st(prop, size);
	} නැතහොත් {
		ආපසු -FDT_ERR_BADNCELLS;
	}

	ආපසු fdt_appendprop(fdt, nodeoffset, name, data,
			      (addr_cells + size_cells) * parimaa_na(fdt32_t));
}
