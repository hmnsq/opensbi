// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#include "libfdt_env.h"

#include <fdt.h>
#include <libfdt.h>

#include "libfdt_internal.h"

ཧྲིལ fdt_setprop_inplace_namelen_partial(void *fdt, ཧྲིལ nodeoffset,
					const var_na *name, ཧྲིལ namelen,
					uint32_t idx, const void *val,
					ཧྲིལ len)
{
	void *propval;
	ཧྲིལ proplen;

	propval = fdt_getprop_namelen_w(fdt, nodeoffset, name, namelen,
					&proplen);
	གལ་ཏེ (!propval)
		ཕྱིར proplen;

	གལ་ཏེ ((unsigned)proplen < (len + idx))
		ཕྱིར -FDT_ERR_NOSPACE;

	memcpy((var_na *)propval + idx, val, len);
	ཕྱིར 0;
}

ཧྲིལ fdt_setprop_inplace(void *fdt, ཧྲིལ nodeoffset, const var_na *name,
			const void *val, ཧྲིལ len)
{
	const void *propval;
	ཧྲིལ proplen;

	propval = fdt_getprop(fdt, nodeoffset, name, &proplen);
	གལ་ཏེ (!propval)
		ཕྱིར proplen;

	གལ་ཏེ (proplen != len)
		ཕྱིར -FDT_ERR_NOSPACE;

	ཕྱིར fdt_setprop_inplace_namelen_partial(fdt, nodeoffset, name,
						   strlen(name), 0,
						   val, len);
}

static void fdt_nop_region_(void *start, ཧྲིལ len)
{
	fdt32_t *p;

	ཆེད་དུ (p = start; (var_na *)p < ((var_na *)start + len); p++)
		*p = cpu_to_fdt32(FDT_NOP);
}

ཧྲིལ fdt_nop_property(void *fdt, ཧྲིལ nodeoffset, const var_na *name)
{
	sa_nracanaa fdt_property *prop;
	ཧྲིལ len;

	prop = fdt_get_property_w(fdt, nodeoffset, name, &len);
	གལ་ཏེ (!prop)
		ཕྱིར len;

	fdt_nop_region_(prop, len + parimaa_na(*prop));

	ཕྱིར 0;
}

ཧྲིལ fdt_node_end_offset_(void *fdt, ཧྲིལ offset)
{
	ཧྲིལ depth = 0;

	སྐབས ((offset >= 0) && (depth >= 0))
		offset = fdt_next_node(fdt, offset, &depth);

	ཕྱིར offset;
}

ཧྲིལ fdt_nop_node(void *fdt, ཧྲིལ nodeoffset)
{
	ཧྲིལ endoffset;

	endoffset = fdt_node_end_offset_(fdt, nodeoffset);
	གལ་ཏེ (endoffset < 0)
		ཕྱིར endoffset;

	fdt_nop_region_(fdt_offset_ptr_w(fdt, nodeoffset, 0),
			endoffset - nodeoffset);
	ཕྱིར 0;
}
