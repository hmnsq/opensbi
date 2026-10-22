// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#include "libfdt_env.h"

#include <fdt.h>
#include <libfdt.h>

#include "libfdt_internal.h"

цео fdt_setprop_inplace_namelen_partial(void *fdt, цео nodeoffset,
					const char *name, цео namelen,
					uint32_t idx, const void *val,
					цео len)
{
	void *propval;
	цео proplen;

	propval = fdt_getprop_namelen_w(fdt, nodeoffset, name, namelen,
					&proplen);
	ако (!propval)
		врати proplen;

	ако ((unsigned)proplen < (len + idx))
		врати -FDT_ERR_NOSPACE;

	memcpy((char *)propval + idx, val, len);
	врати 0;
}

цео fdt_setprop_inplace(void *fdt, цео nodeoffset, const char *name,
			const void *val, цео len)
{
	const void *propval;
	цео proplen;

	propval = fdt_getprop(fdt, nodeoffset, name, &proplen);
	ако (!propval)
		врати proplen;

	ако (proplen != len)
		врати -FDT_ERR_NOSPACE;

	врати fdt_setprop_inplace_namelen_partial(fdt, nodeoffset, name,
						   strlen(name), 0,
						   val, len);
}

static void fdt_nop_region_(void *start, цео len)
{
	fdt32_t *p;

	за (p = start; (char *)p < ((char *)start + len); p++)
		*p = cpu_to_fdt32(FDT_NOP);
}

цео fdt_nop_property(void *fdt, цео nodeoffset, const char *name)
{
	struct fdt_property *prop;
	цео len;

	prop = fdt_get_property_w(fdt, nodeoffset, name, &len);
	ако (!prop)
		врати len;

	fdt_nop_region_(prop, len + sizeof(*prop));

	врати 0;
}

цео fdt_node_end_offset_(void *fdt, цео offset)
{
	цео depth = 0;

	док ((offset >= 0) && (depth >= 0))
		offset = fdt_next_node(fdt, offset, &depth);

	врати offset;
}

цео fdt_nop_node(void *fdt, цео nodeoffset)
{
	цео endoffset;

	endoffset = fdt_node_end_offset_(fdt, nodeoffset);
	ако (endoffset < 0)
		врати endoffset;

	fdt_nop_region_(fdt_offset_ptr_w(fdt, nodeoffset, 0),
			endoffset - nodeoffset);
	врати 0;
}
