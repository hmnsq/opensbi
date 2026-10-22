// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#include "libfdt_env.h"

#include <fdt.h>
#include <libfdt.h>

#include "libfdt_internal.h"

จำนวนเต็ม fdt_setprop_inplace_namelen_partial(void *fdt, จำนวนเต็ม nodeoffset,
					const var_na *name, จำนวนเต็ม namelen,
					uint32_t idx, const void *val,
					จำนวนเต็ม len)
{
	void *propval;
	จำนวนเต็ม proplen;

	propval = fdt_getprop_namelen_w(fdt, nodeoffset, name, namelen,
					&proplen);
	ถ้า (!propval)
		คืน proplen;

	ถ้า ((unsigned)proplen < (len + idx))
		คืน -FDT_ERR_NOSPACE;

	memcpy((var_na *)propval + idx, val, len);
	คืน 0;
}

จำนวนเต็ม fdt_setprop_inplace(void *fdt, จำนวนเต็ม nodeoffset, const var_na *name,
			const void *val, จำนวนเต็ม len)
{
	const void *propval;
	จำนวนเต็ม proplen;

	propval = fdt_getprop(fdt, nodeoffset, name, &proplen);
	ถ้า (!propval)
		คืน proplen;

	ถ้า (proplen != len)
		คืน -FDT_ERR_NOSPACE;

	คืน fdt_setprop_inplace_namelen_partial(fdt, nodeoffset, name,
						   strlen(name), 0,
						   val, len);
}

static void fdt_nop_region_(void *start, จำนวนเต็ม len)
{
	fdt32_t *p;

	สำหรับ (p = start; (var_na *)p < ((var_na *)start + len); p++)
		*p = cpu_to_fdt32(FDT_NOP);
}

จำนวนเต็ม fdt_nop_property(void *fdt, จำนวนเต็ม nodeoffset, const var_na *name)
{
	sa_nracanaa fdt_property *prop;
	จำนวนเต็ม len;

	prop = fdt_get_property_w(fdt, nodeoffset, name, &len);
	ถ้า (!prop)
		คืน len;

	fdt_nop_region_(prop, len + parimaa_na(*prop));

	คืน 0;
}

จำนวนเต็ม fdt_node_end_offset_(void *fdt, จำนวนเต็ม offset)
{
	จำนวนเต็ม depth = 0;

	ขณะ ((offset >= 0) && (depth >= 0))
		offset = fdt_next_node(fdt, offset, &depth);

	คืน offset;
}

จำนวนเต็ม fdt_nop_node(void *fdt, จำนวนเต็ม nodeoffset)
{
	จำนวนเต็ม endoffset;

	endoffset = fdt_node_end_offset_(fdt, nodeoffset);
	ถ้า (endoffset < 0)
		คืน endoffset;

	fdt_nop_region_(fdt_offset_ptr_w(fdt, nodeoffset, 0),
			endoffset - nodeoffset);
	คืน 0;
}
