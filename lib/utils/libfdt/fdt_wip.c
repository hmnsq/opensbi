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
	إذا (!propval)
		أرجع proplen;

	إذا ((unsigned)proplen < (len + idx))
		أرجع -FDT_ERR_NOSPACE;

	memcpy((حرف *)propval + idx, val, len);
	أرجع 0;
}

صحيح fdt_setprop_inplace(فراغ *fdt, صحيح nodeoffset, ثابت حرف *name,
			ثابت فراغ *val, صحيح len)
{
	ثابت فراغ *propval;
	صحيح proplen;

	propval = fdt_getprop(fdt, nodeoffset, name, &proplen);
	إذا (!propval)
		أرجع proplen;

	إذا (proplen != len)
		أرجع -FDT_ERR_NOSPACE;

	أرجع fdt_setprop_inplace_namelen_partial(fdt, nodeoffset, name,
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
	إذا (!prop)
		أرجع len;

	fdt_nop_region_(prop, len + حجم(*prop));

	أرجع 0;
}

صحيح fdt_نود_end_offset(فراغ *fdt, صحيح offset)
{
	صحيح depth = 0;

	طالما ((offset >= 0) && (depth >= 0))
		offset = fdt_next_نود(fdt, offset, &depth);

	أرجع offset;
}

صحيح fdt_nop_نود(فراغ *fdt, صحيح nodeoffset)
{
	صحيح endoffset;

	endoffset = fdt_نود_end_offset(fdt, nodeoffset);
	إذا (endoffset < 0)
		أرجع endoffset;

	fdt_nop_region_(fdt_offset_ptr_w(fdt, nodeoffset, 0),
			endoffset - nodeoffset);
	أرجع 0;
}
