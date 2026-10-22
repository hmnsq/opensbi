// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#include "libfdt_env.h"

#include <fdt.h>
#include <libfdt.h>

#include "libfdt_internal.h"

ціле fdt_setprop_inplace_namelen_partial(void *fdt, ціле nodeoffset,
					const char *name, ціле namelen,
					uint32_t idx, const void *val,
					ціле len)
{
	void *propval;
	ціле proplen;

	propval = fdt_getprop_namelen_w(fdt, nodeoffset, name, namelen,
					&proplen);
	якщо (!propval)
		повернути proplen;

	якщо ((unsigned)proplen < (len + idx))
		повернути -FDT_ERR_NOSPACE;

	memcpy((char *)propval + idx, val, len);
	повернути 0;
}

ціле fdt_setprop_inplace(void *fdt, ціле nodeoffset, const char *name,
			const void *val, ціле len)
{
	const void *propval;
	ціле proplen;

	propval = fdt_getprop(fdt, nodeoffset, name, &proplen);
	якщо (!propval)
		повернути proplen;

	якщо (proplen != len)
		повернути -FDT_ERR_NOSPACE;

	повернути fdt_setprop_inplace_namelen_partial(fdt, nodeoffset, name,
						   strlen(name), 0,
						   val, len);
}

static void fdt_nop_region_(void *start, ціле len)
{
	fdt32_t *p;

	для (p = start; (char *)p < ((char *)start + len); p++)
		*p = cpu_to_fdt32(FDT_NOP);
}

ціле fdt_nop_property(void *fdt, ціле nodeoffset, const char *name)
{
	struct fdt_property *prop;
	ціле len;

	prop = fdt_get_property_w(fdt, nodeoffset, name, &len);
	якщо (!prop)
		повернути len;

	fdt_nop_region_(prop, len + sizeof(*prop));

	повернути 0;
}

ціле fdt_node_end_offset_(void *fdt, ціле offset)
{
	ціле depth = 0;

	поки ((offset >= 0) && (depth >= 0))
		offset = fdt_next_node(fdt, offset, &depth);

	повернути offset;
}

ціле fdt_nop_node(void *fdt, ціле nodeoffset)
{
	ціле endoffset;

	endoffset = fdt_node_end_offset_(fdt, nodeoffset);
	якщо (endoffset < 0)
		повернути endoffset;

	fdt_nop_region_(fdt_offset_ptr_w(fdt, nodeoffset, 0),
			endoffset - nodeoffset);
	повернути 0;
}
