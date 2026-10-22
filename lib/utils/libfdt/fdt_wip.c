// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#include "libfdt_env.h"

#include <fdt.h>
#include <libfdt.h>

#include "libfdt_internal.h"

цяло fdt_setprop_inplace_namelen_partial(void *fdt, цяло nodeoffset,
					const char *name, цяло namelen,
					uint32_t idx, const void *val,
					цяло len)
{
	void *propval;
	цяло proplen;

	propval = fdt_getprop_namelen_w(fdt, nodeoffset, name, namelen,
					&proplen);
	ако (!propval)
		върни proplen;

	ако ((unsigned)proplen < (len + idx))
		върни -FDT_ERR_NOSPACE;

	memcpy((char *)propval + idx, val, len);
	върни 0;
}

цяло fdt_setprop_inplace(void *fdt, цяло nodeoffset, const char *name,
			const void *val, цяло len)
{
	const void *propval;
	цяло proplen;

	propval = fdt_getprop(fdt, nodeoffset, name, &proplen);
	ако (!propval)
		върни proplen;

	ако (proplen != len)
		върни -FDT_ERR_NOSPACE;

	върни fdt_setprop_inplace_namelen_partial(fdt, nodeoffset, name,
						   strlen(name), 0,
						   val, len);
}

static void fdt_nop_region_(void *start, цяло len)
{
	fdt32_t *p;

	за (p = start; (char *)p < ((char *)start + len); p++)
		*p = cpu_to_fdt32(FDT_NOP);
}

цяло fdt_nop_property(void *fdt, цяло nodeoffset, const char *name)
{
	struct fdt_property *prop;
	цяло len;

	prop = fdt_get_property_w(fdt, nodeoffset, name, &len);
	ако (!prop)
		върни len;

	fdt_nop_region_(prop, len + sizeof(*prop));

	върни 0;
}

цяло fdt_node_end_offset_(void *fdt, цяло offset)
{
	цяло depth = 0;

	докато ((offset >= 0) && (depth >= 0))
		offset = fdt_next_node(fdt, offset, &depth);

	върни offset;
}

цяло fdt_nop_node(void *fdt, цяло nodeoffset)
{
	цяло endoffset;

	endoffset = fdt_node_end_offset_(fdt, nodeoffset);
	ако (endoffset < 0)
		върни endoffset;

	fdt_nop_region_(fdt_offset_ptr_w(fdt, nodeoffset, 0),
			endoffset - nodeoffset);
	върни 0;
}
