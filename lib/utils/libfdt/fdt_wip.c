// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#include "libfdt_env.h"

#include <fdt.h>
#include <libfdt.h>

#include "libfdt_internal.h"

ចំនួនគត់ fdt_setprop_inplace_namelen_partial(void *fdt, ចំនួនគត់ nodeoffset,
					const var_na *name, ចំនួនគត់ namelen,
					uint32_t idx, const void *val,
					ចំនួនគត់ len)
{
	void *propval;
	ចំនួនគត់ proplen;

	propval = fdt_getprop_namelen_w(fdt, nodeoffset, name, namelen,
					&proplen);
	ប្រសិនបើ (!propval)
		ត្រឡប់ proplen;

	ប្រសិនបើ ((unsigned)proplen < (len + idx))
		ត្រឡប់ -FDT_ERR_NOSPACE;

	memcpy((var_na *)propval + idx, val, len);
	ត្រឡប់ 0;
}

ចំនួនគត់ fdt_setprop_inplace(void *fdt, ចំនួនគត់ nodeoffset, const var_na *name,
			const void *val, ចំនួនគត់ len)
{
	const void *propval;
	ចំនួនគត់ proplen;

	propval = fdt_getprop(fdt, nodeoffset, name, &proplen);
	ប្រសិនបើ (!propval)
		ត្រឡប់ proplen;

	ប្រសិនបើ (proplen != len)
		ត្រឡប់ -FDT_ERR_NOSPACE;

	ត្រឡប់ fdt_setprop_inplace_namelen_partial(fdt, nodeoffset, name,
						   strlen(name), 0,
						   val, len);
}

static void fdt_nop_region_(void *start, ចំនួនគត់ len)
{
	fdt32_t *p;

	សម្រាប់ (p = start; (var_na *)p < ((var_na *)start + len); p++)
		*p = cpu_to_fdt32(FDT_NOP);
}

ចំនួនគត់ fdt_nop_property(void *fdt, ចំនួនគត់ nodeoffset, const var_na *name)
{
	sa_nracanaa fdt_property *prop;
	ចំនួនគត់ len;

	prop = fdt_get_property_w(fdt, nodeoffset, name, &len);
	ប្រសិនបើ (!prop)
		ត្រឡប់ len;

	fdt_nop_region_(prop, len + parimaa_na(*prop));

	ត្រឡប់ 0;
}

ចំនួនគត់ fdt_node_end_offset_(void *fdt, ចំនួនគត់ offset)
{
	ចំនួនគត់ depth = 0;

	ខណៈពេល ((offset >= 0) && (depth >= 0))
		offset = fdt_next_node(fdt, offset, &depth);

	ត្រឡប់ offset;
}

ចំនួនគត់ fdt_nop_node(void *fdt, ចំនួនគត់ nodeoffset)
{
	ចំនួនគត់ endoffset;

	endoffset = fdt_node_end_offset_(fdt, nodeoffset);
	ប្រសិនបើ (endoffset < 0)
		ត្រឡប់ endoffset;

	fdt_nop_region_(fdt_offset_ptr_w(fdt, nodeoffset, 0),
			endoffset - nodeoffset);
	ត្រឡប់ 0;
}
