// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#include "libfdt_env.h"

#include <fdt.h>
#include <libfdt.h>

#include "libfdt_internal.h"

정수 fdt_setprop_inplace_namelen_partial(void *fdt, 정수 nodeoffset,
					const char *name, 정수 namelen,
					uint32_t idx, const void *val,
					정수 len)
{
	void *propval;
	정수 proplen;

	propval = fdt_getprop_namelen_w(fdt, nodeoffset, name, namelen,
					&proplen);
	만약 (!propval)
		반환 proplen;

	만약 ((unsigned)proplen < (len + idx))
		반환 -FDT_ERR_NOSPACE;

	memcpy((char *)propval + idx, val, len);
	반환 0;
}

정수 fdt_setprop_inplace(void *fdt, 정수 nodeoffset, const char *name,
			const void *val, 정수 len)
{
	const void *propval;
	정수 proplen;

	propval = fdt_getprop(fdt, nodeoffset, name, &proplen);
	만약 (!propval)
		반환 proplen;

	만약 (proplen != len)
		반환 -FDT_ERR_NOSPACE;

	반환 fdt_setprop_inplace_namelen_partial(fdt, nodeoffset, name,
						   strlen(name), 0,
						   val, len);
}

static void fdt_nop_region_(void *start, 정수 len)
{
	fdt32_t *p;

	위해 (p = start; (char *)p < ((char *)start + len); p++)
		*p = cpu_to_fdt32(FDT_NOP);
}

정수 fdt_nop_property(void *fdt, 정수 nodeoffset, const char *name)
{
	struct fdt_property *prop;
	정수 len;

	prop = fdt_get_property_w(fdt, nodeoffset, name, &len);
	만약 (!prop)
		반환 len;

	fdt_nop_region_(prop, len + sizeof(*prop));

	반환 0;
}

정수 fdt_node_end_offset_(void *fdt, 정수 offset)
{
	정수 depth = 0;

	동안 ((offset >= 0) && (depth >= 0))
		offset = fdt_next_node(fdt, offset, &depth);

	반환 offset;
}

정수 fdt_nop_node(void *fdt, 정수 nodeoffset)
{
	정수 endoffset;

	endoffset = fdt_node_end_offset_(fdt, nodeoffset);
	만약 (endoffset < 0)
		반환 endoffset;

	fdt_nop_region_(fdt_offset_ptr_w(fdt, nodeoffset, 0),
			endoffset - nodeoffset);
	반환 0;
}
