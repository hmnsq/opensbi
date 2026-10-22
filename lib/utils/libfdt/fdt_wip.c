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
	ܐܢ (!propval)
		ܦܢܐ proplen;

	ܐܢ ((unsigned)proplen < (len + idx))
		ܦܢܐ -FDT_ERR_NOSPACE;

	memcpy((حرف *)propval + idx, val, len);
	ܦܢܐ 0;
}

صحيح fdt_setprop_inplace(فراغ *fdt, صحيح nodeoffset, ثابت حرف *name,
			ثابت فراغ *val, صحيح len)
{
	ثابت فراغ *propval;
	صحيح proplen;

	propval = fdt_getprop(fdt, nodeoffset, name, &proplen);
	ܐܢ (!propval)
		ܦܢܐ proplen;

	ܐܢ (proplen != len)
		ܦܢܐ -FDT_ERR_NOSPACE;

	ܦܢܐ fdt_setprop_inplace_namelen_partial(fdt, nodeoffset, name,
						   strlen(name), 0,
						   val, len);
}

ساكن فراغ fdt_nop_region_(فراغ *start, صحيح len)
{
	fdt32_t *p;

	لكل (p = start; (حرف *)p < ((حرف *)start + len); p++)
		*p = cpu_to_fdt32(FDT_NOP);
}

صحيح fdt_nop_property(فراغ *fdt, صحيح nodeoffset, ثابت حرف *name)
{
	بنية fdt_property *prop;
	صحيح len;

	prop = fdt_get_property_w(fdt, nodeoffset, name, &len);
	ܐܢ (!prop)
		ܦܢܐ len;

	fdt_nop_region_(prop, len + حجم(*prop));

	ܦܢܐ 0;
}

صحيح fdt_node_end_offset_(فراغ *fdt, صحيح offset)
{
	صحيح depth = 0;

	ܟܡܐ ((offset >= 0) && (depth >= 0))
		offset = fdt_next_node(fdt, offset, &depth);

	ܦܢܐ offset;
}

صحيح fdt_nop_node(فراغ *fdt, صحيح nodeoffset)
{
	صحيح endoffset;

	endoffset = fdt_node_end_offset_(fdt, nodeoffset);
	ܐܢ (endoffset < 0)
		ܦܢܐ endoffset;

	fdt_nop_region_(fdt_offset_ptr_w(fdt, nodeoffset, 0),
			endoffset - nodeoffset);
	ܦܢܐ 0;
}
