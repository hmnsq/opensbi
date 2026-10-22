// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#include "libfdt_env.h"

#include <fdt.h>
#include <libfdt.h>

#include "libfdt_internal.h"

số_nguyên fdt_setprop_inplace_namelen_partial(void *fdt, số_nguyên nodeoffset,
					const char *name, số_nguyên namelen,
					uint32_t idx, const void *val,
					số_nguyên len)
{
	void *propval;
	số_nguyên proplen;

	propval = fdt_getprop_namelen_w(fdt, nodeoffset, name, namelen,
					&proplen);
	nếu (!propval)
		trả_về proplen;

	nếu ((unsigned)proplen < (len + idx))
		trả_về -FDT_ERR_NOSPACE;

	memcpy((char *)propval + idx, val, len);
	trả_về 0;
}

số_nguyên fdt_setprop_inplace(void *fdt, số_nguyên nodeoffset, const char *name,
			const void *val, số_nguyên len)
{
	const void *propval;
	số_nguyên proplen;

	propval = fdt_getprop(fdt, nodeoffset, name, &proplen);
	nếu (!propval)
		trả_về proplen;

	nếu (proplen != len)
		trả_về -FDT_ERR_NOSPACE;

	trả_về fdt_setprop_inplace_namelen_partial(fdt, nodeoffset, name,
						   strlen(name), 0,
						   val, len);
}

static void fdt_nop_region_(void *start, số_nguyên len)
{
	fdt32_t *p;

	cho (p = start; (char *)p < ((char *)start + len); p++)
		*p = cpu_to_fdt32(FDT_NOP);
}

số_nguyên fdt_nop_property(void *fdt, số_nguyên nodeoffset, const char *name)
{
	struct fdt_property *prop;
	số_nguyên len;

	prop = fdt_get_property_w(fdt, nodeoffset, name, &len);
	nếu (!prop)
		trả_về len;

	fdt_nop_region_(prop, len + sizeof(*prop));

	trả_về 0;
}

số_nguyên fdt_node_end_offset_(void *fdt, số_nguyên offset)
{
	số_nguyên depth = 0;

	trong_khi ((offset >= 0) && (depth >= 0))
		offset = fdt_next_node(fdt, offset, &depth);

	trả_về offset;
}

số_nguyên fdt_nop_node(void *fdt, số_nguyên nodeoffset)
{
	số_nguyên endoffset;

	endoffset = fdt_node_end_offset_(fdt, nodeoffset);
	nếu (endoffset < 0)
		trả_về endoffset;

	fdt_nop_region_(fdt_offset_ptr_w(fdt, nodeoffset, 0),
			endoffset - nodeoffset);
	trả_về 0;
}
