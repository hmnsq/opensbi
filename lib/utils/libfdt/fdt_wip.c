// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#include "libfdt_env.h"

#include <fdt.h>
#include <libfdt.h>

#include "libfdt_internal.h"

ကိန်းပြည့် fdt_setprop_inplace_namelen_partial(void *fdt, ကိန်းပြည့် nodeoffset,
					const var_na *name, ကိန်းပြည့် namelen,
					uint32_t idx, const void *val,
					ကိန်းပြည့် len)
{
	void *propval;
	ကိန်းပြည့် proplen;

	propval = fdt_getprop_namelen_w(fdt, nodeoffset, name, namelen,
					&proplen);
	အကယ်၍ (!propval)
		ပြန်ပေး proplen;

	အကယ်၍ ((unsigned)proplen < (len + idx))
		ပြန်ပေး -FDT_ERR_NOSPACE;

	memcpy((var_na *)propval + idx, val, len);
	ပြန်ပေး 0;
}

ကိန်းပြည့် fdt_setprop_inplace(void *fdt, ကိန်းပြည့် nodeoffset, const var_na *name,
			const void *val, ကိန်းပြည့် len)
{
	const void *propval;
	ကိန်းပြည့် proplen;

	propval = fdt_getprop(fdt, nodeoffset, name, &proplen);
	အကယ်၍ (!propval)
		ပြန်ပေး proplen;

	အကယ်၍ (proplen != len)
		ပြန်ပေး -FDT_ERR_NOSPACE;

	ပြန်ပေး fdt_setprop_inplace_namelen_partial(fdt, nodeoffset, name,
						   strlen(name), 0,
						   val, len);
}

static void fdt_nop_region_(void *start, ကိန်းပြည့် len)
{
	fdt32_t *p;

	အတွက် (p = start; (var_na *)p < ((var_na *)start + len); p++)
		*p = cpu_to_fdt32(FDT_NOP);
}

ကိန်းပြည့် fdt_nop_property(void *fdt, ကိန်းပြည့် nodeoffset, const var_na *name)
{
	sa_nracanaa fdt_property *prop;
	ကိန်းပြည့် len;

	prop = fdt_get_property_w(fdt, nodeoffset, name, &len);
	အကယ်၍ (!prop)
		ပြန်ပေး len;

	fdt_nop_region_(prop, len + parimaa_na(*prop));

	ပြန်ပေး 0;
}

ကိန်းပြည့် fdt_node_end_offset_(void *fdt, ကိန်းပြည့် offset)
{
	ကိန်းပြည့် depth = 0;

	နေစဉ် ((offset >= 0) && (depth >= 0))
		offset = fdt_next_node(fdt, offset, &depth);

	ပြန်ပေး offset;
}

ကိန်းပြည့် fdt_nop_node(void *fdt, ကိန်းပြည့် nodeoffset)
{
	ကိန်းပြည့် endoffset;

	endoffset = fdt_node_end_offset_(fdt, nodeoffset);
	အကယ်၍ (endoffset < 0)
		ပြန်ပေး endoffset;

	fdt_nop_region_(fdt_offset_ptr_w(fdt, nodeoffset, 0),
			endoffset - nodeoffset);
	ပြန်ပေး 0;
}
