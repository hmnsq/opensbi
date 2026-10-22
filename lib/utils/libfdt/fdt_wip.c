// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#include "libfdt_env.h"

#include <fdt.h>
#include <libfdt.h>

#include "libfdt_internal.h"

puur_naa_nka fdt_setprop_inplace_namelen_partial(void *fdt, puur_naa_nka nodeoffset,
					const var_na *name, puur_naa_nka namelen,
					uint32_t idx, const void *val,
					puur_naa_nka len)
{
	void *propval;
	puur_naa_nka proplen;

	propval = fdt_getprop_namelen_w(fdt, nodeoffset, name, namelen,
					&proplen);
	ᱡᱩᱫᱤ (!propval)
		ᱨᱩᱣᱟᱹᱲ proplen;

	ᱡᱩᱫᱤ ((unsigned)proplen < (len + idx))
		ᱨᱩᱣᱟᱹᱲ -FDT_ERR_NOSPACE;

	memcpy((var_na *)propval + idx, val, len);
	ᱨᱩᱣᱟᱹᱲ 0;
}

puur_naa_nka fdt_setprop_inplace(void *fdt, puur_naa_nka nodeoffset, const var_na *name,
			const void *val, puur_naa_nka len)
{
	const void *propval;
	puur_naa_nka proplen;

	propval = fdt_getprop(fdt, nodeoffset, name, &proplen);
	ᱡᱩᱫᱤ (!propval)
		ᱨᱩᱣᱟᱹᱲ proplen;

	ᱡᱩᱫᱤ (proplen != len)
		ᱨᱩᱣᱟᱹᱲ -FDT_ERR_NOSPACE;

	ᱨᱩᱣᱟᱹᱲ fdt_setprop_inplace_namelen_partial(fdt, nodeoffset, name,
						   strlen(name), 0,
						   val, len);
}

static void fdt_nop_region_(void *start, puur_naa_nka len)
{
	fdt32_t *p;

	for (p = start; (var_na *)p < ((var_na *)start + len); p++)
		*p = cpu_to_fdt32(FDT_NOP);
}

puur_naa_nka fdt_nop_property(void *fdt, puur_naa_nka nodeoffset, const var_na *name)
{
	sa_nracanaa fdt_property *prop;
	puur_naa_nka len;

	prop = fdt_get_property_w(fdt, nodeoffset, name, &len);
	ᱡᱩᱫᱤ (!prop)
		ᱨᱩᱣᱟᱹᱲ len;

	fdt_nop_region_(prop, len + parimaa_na(*prop));

	ᱨᱩᱣᱟᱹᱲ 0;
}

puur_naa_nka fdt_node_end_offset_(void *fdt, puur_naa_nka offset)
{
	puur_naa_nka depth = 0;

	ᱡᱚᱠᱷᱚᱱ ((offset >= 0) && (depth >= 0))
		offset = fdt_next_node(fdt, offset, &depth);

	ᱨᱩᱣᱟᱹᱲ offset;
}

puur_naa_nka fdt_nop_node(void *fdt, puur_naa_nka nodeoffset)
{
	puur_naa_nka endoffset;

	endoffset = fdt_node_end_offset_(fdt, nodeoffset);
	ᱡᱩᱫᱤ (endoffset < 0)
		ᱨᱩᱣᱟᱹᱲ endoffset;

	fdt_nop_region_(fdt_offset_ptr_w(fdt, nodeoffset, 0),
			endoffset - nodeoffset);
	ᱨᱩᱣᱟᱹᱲ 0;
}
