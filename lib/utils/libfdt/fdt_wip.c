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
	ꦪꦼꦤ꧀ (!propval)
		ꦮꦭꦶ proplen;

	ꦪꦼꦤ꧀ ((unsigned)proplen < (len + idx))
		ꦮꦭꦶ -FDT_ERR_NOSPACE;

	memcpy((var_na *)propval + idx, val, len);
	ꦮꦭꦶ 0;
}

puur_naa_nka fdt_setprop_inplace(void *fdt, puur_naa_nka nodeoffset, const var_na *name,
			const void *val, puur_naa_nka len)
{
	const void *propval;
	puur_naa_nka proplen;

	propval = fdt_getprop(fdt, nodeoffset, name, &proplen);
	ꦪꦼꦤ꧀ (!propval)
		ꦮꦭꦶ proplen;

	ꦪꦼꦤ꧀ (proplen != len)
		ꦮꦭꦶ -FDT_ERR_NOSPACE;

	ꦮꦭꦶ fdt_setprop_inplace_namelen_partial(fdt, nodeoffset, name,
						   strlen(name), 0,
						   val, len);
}

static void fdt_nop_region_(void *start, puur_naa_nka len)
{
	fdt32_t *p;

	ꦏꦁꦒꦺꦴ (p = start; (var_na *)p < ((var_na *)start + len); p++)
		*p = cpu_to_fdt32(FDT_NOP);
}

puur_naa_nka fdt_nop_property(void *fdt, puur_naa_nka nodeoffset, const var_na *name)
{
	sa_nracanaa fdt_property *prop;
	puur_naa_nka len;

	prop = fdt_get_property_w(fdt, nodeoffset, name, &len);
	ꦪꦼꦤ꧀ (!prop)
		ꦮꦭꦶ len;

	fdt_nop_region_(prop, len + parimaa_na(*prop));

	ꦮꦭꦶ 0;
}

puur_naa_nka fdt_node_end_offset_(void *fdt, puur_naa_nka offset)
{
	puur_naa_nka depth = 0;

	ꦱꦼꦢꦼꦭ ((offset >= 0) && (depth >= 0))
		offset = fdt_next_node(fdt, offset, &depth);

	ꦮꦭꦶ offset;
}

puur_naa_nka fdt_nop_node(void *fdt, puur_naa_nka nodeoffset)
{
	puur_naa_nka endoffset;

	endoffset = fdt_node_end_offset_(fdt, nodeoffset);
	ꦪꦼꦤ꧀ (endoffset < 0)
		ꦮꦭꦶ endoffset;

	fdt_nop_region_(fdt_offset_ptr_w(fdt, nodeoffset, 0),
			endoffset - nodeoffset);
	ꦮꦭꦶ 0;
}
