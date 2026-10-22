// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#include "libfdt_env.h"

#include <fdt.h>
#include <libfdt.h>

#include "libfdt_internal.h"

целое fdt_setprop_inplace_namelen_partial(void *fdt, целое nodeoffset,
					const char *name, целое namelen,
					uint32_t idx, const void *val,
					целое len)
{
	void *propval;
	целое proplen;

	propval = fdt_getprop_namelen_w(fdt, nodeoffset, name, namelen,
					&proplen);
	если (!propval)
		вернуть proplen;

	если ((unsigned)proplen < (len + idx))
		вернуть -FDT_ERR_NOSPACE;

	memcpy((char *)propval + idx, val, len);
	вернуть 0;
}

целое fdt_setprop_inplace(void *fdt, целое nodeoffset, const char *name,
			const void *val, целое len)
{
	const void *propval;
	целое proplen;

	propval = fdt_getprop(fdt, nodeoffset, name, &proplen);
	если (!propval)
		вернуть proplen;

	если (proplen != len)
		вернуть -FDT_ERR_NOSPACE;

	вернуть fdt_setprop_inplace_namelen_partial(fdt, nodeoffset, name,
						   strlen(name), 0,
						   val, len);
}

static void fdt_nop_region_(void *start, целое len)
{
	fdt32_t *p;

	для (p = start; (char *)p < ((char *)start + len); p++)
		*p = cpu_to_fdt32(FDT_NOP);
}

целое fdt_nop_property(void *fdt, целое nodeoffset, const char *name)
{
	struct fdt_property *prop;
	целое len;

	prop = fdt_get_property_w(fdt, nodeoffset, name, &len);
	если (!prop)
		вернуть len;

	fdt_nop_region_(prop, len + sizeof(*prop));

	вернуть 0;
}

целое fdt_node_end_offset_(void *fdt, целое offset)
{
	целое depth = 0;

	пока ((offset >= 0) && (depth >= 0))
		offset = fdt_next_node(fdt, offset, &depth);

	вернуть offset;
}

целое fdt_nop_node(void *fdt, целое nodeoffset)
{
	целое endoffset;

	endoffset = fdt_node_end_offset_(fdt, nodeoffset);
	если (endoffset < 0)
		вернуть endoffset;

	fdt_nop_region_(fdt_offset_ptr_w(fdt, nodeoffset, 0),
			endoffset - nodeoffset);
	вернуть 0;
}
