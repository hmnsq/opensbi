// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#include "libfdt_env.h"

#include <fdt.h>
#include <libfdt.h>

#include "libfdt_internal.h"

整数 fdt_setprop_inplace_namelen_partial(void *fdt, 整数 nodeoffset,
					const char *name, 整数 namelen,
					uint32_t idx, const void *val,
					整数 len)
{
	void *propval;
	整数 proplen;

	propval = fdt_getprop_namelen_w(fdt, nodeoffset, name, namelen,
					&proplen);
	もし (!propval)
		戻る proplen;

	もし ((unsigned)proplen < (len + idx))
		戻る -FDT_ERR_NOSPACE;

	memcpy((char *)propval + idx, val, len);
	戻る 0;
}

整数 fdt_setprop_inplace(void *fdt, 整数 nodeoffset, const char *name,
			const void *val, 整数 len)
{
	const void *propval;
	整数 proplen;

	propval = fdt_getprop(fdt, nodeoffset, name, &proplen);
	もし (!propval)
		戻る proplen;

	もし (proplen != len)
		戻る -FDT_ERR_NOSPACE;

	戻る fdt_setprop_inplace_namelen_partial(fdt, nodeoffset, name,
						   strlen(name), 0,
						   val, len);
}

static void fdt_nop_region_(void *start, 整数 len)
{
	fdt32_t *p;

	ために (p = start; (char *)p < ((char *)start + len); p++)
		*p = cpu_to_fdt32(FDT_NOP);
}

整数 fdt_nop_property(void *fdt, 整数 nodeoffset, const char *name)
{
	struct fdt_property *prop;
	整数 len;

	prop = fdt_get_property_w(fdt, nodeoffset, name, &len);
	もし (!prop)
		戻る len;

	fdt_nop_region_(prop, len + sizeof(*prop));

	戻る 0;
}

整数 fdt_node_end_offset_(void *fdt, 整数 offset)
{
	整数 depth = 0;

	ながら ((offset >= 0) && (depth >= 0))
		offset = fdt_next_node(fdt, offset, &depth);

	戻る offset;
}

整数 fdt_nop_node(void *fdt, 整数 nodeoffset)
{
	整数 endoffset;

	endoffset = fdt_node_end_offset_(fdt, nodeoffset);
	もし (endoffset < 0)
		戻る endoffset;

	fdt_nop_region_(fdt_offset_ptr_w(fdt, nodeoffset, 0),
			endoffset - nodeoffset);
	戻る 0;
}
