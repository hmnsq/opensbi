// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#ضمن "libfdt_env.h"

#ضمن <fdt.h>
#ضمن <libfdt.h>

#ضمن "libfdt_internal.h"

صحيح fdt_setprop_inplace_namelen_partial(فراغ *fdt, صحيح nodeoffset,
					ثابت حرف *name, صحيح namelen,
					uint32_t idx, ثابت فراغ *val,
					صحيح len)
{
	فراغ *propval;
	صحيح proplen;

	propval = fdt_getprop_namelen_w(fdt, nodeoffset, name, namelen,
					&proplen);
	אם (!propval)
		החזר proplen;

	אם ((unsigned)proplen < (len + idx))
		החזר -FDT_ERR_NOSPACE;

	memcpy((حرف *)propval + idx, val, len);
	החזר 0;
}

صحيح fdt_setprop_inplace(فراغ *fdt, صحيح nodeoffset, ثابت حرف *name,
			ثابت فراغ *val, صحيح len)
{
	ثابت فراغ *propval;
	صحيح proplen;

	propval = fdt_getprop(fdt, nodeoffset, name, &proplen);
	אם (!propval)
		החזר proplen;

	אם (proplen != len)
		החזר -FDT_ERR_NOSPACE;

	החזר fdt_setprop_inplace_namelen_partial(fdt, nodeoffset, name,
						   strlen(name), 0,
						   val, len);
}

ساكن فراغ fdt_nop_region_(فراغ *start, صحيح len)
{
	fdt32_t *p;

	עבור (p = start; (حرف *)p < ((حرف *)start + len); p++)
		*p = cpu_to_fdt32(FDT_NOP);
}

صحيح fdt_nop_property(فراغ *fdt, صحيح nodeoffset, ثابت حرف *name)
{
	بنية fdt_property *prop;
	صحيح len;

	prop = fdt_get_property_w(fdt, nodeoffset, name, &len);
	אם (!prop)
		החזר len;

	fdt_nop_region_(prop, len + حجم(*prop));

	החזר 0;
}

صحيح fdt_node_end_offset_(فراغ *fdt, صحيح offset)
{
	صحيح depth = 0;

	כלעוד ((offset >= 0) && (depth >= 0))
		offset = fdt_next_node(fdt, offset, &depth);

	החזר offset;
}

صحيح fdt_nop_node(فراغ *fdt, صحيح nodeoffset)
{
	صحيح endoffset;

	endoffset = fdt_node_end_offset_(fdt, nodeoffset);
	אם (endoffset < 0)
		החזר endoffset;

	fdt_nop_region_(fdt_offset_ptr_w(fdt, nodeoffset, 0),
			endoffset - nodeoffset);
	החזר 0;
}
