// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#شامل "libfdt_env.h"

#شامل <fdt.h>
#شامل <libfdt.h>

#شامل "libfdt_internal.h"

صحیح fdt_setprop_inplace_namelen_partial(تهی *fdt, صحیح nodeoffset,
					ثابت نویسه *name, صحیح namelen,
					uint32_t idx, ثابت تهی *val,
					صحیح len)
{
	تهی *propval;
	صحیح proplen;

	propval = fdt_getprop_namelen_w(fdt, nodeoffset, name, namelen,
					&proplen);
	اگر (!propval)
		بازگشت proplen;

	اگر ((unsigned)proplen < (len + idx))
		بازگشت -FDT_ERR_NOSPACE;

	memcpy((نویسه *)propval + idx, val, len);
	بازگشت 0;
}

صحیح fdt_setprop_inplace(تهی *fdt, صحیح nodeoffset, ثابت نویسه *name,
			ثابت تهی *val, صحیح len)
{
	ثابت تهی *propval;
	صحیح proplen;

	propval = fdt_getprop(fdt, nodeoffset, name, &proplen);
	اگر (!propval)
		بازگشت proplen;

	اگر (proplen != len)
		بازگشت -FDT_ERR_NOSPACE;

	بازگشت fdt_setprop_inplace_namelen_partial(fdt, nodeoffset, name,
						   strlen(name), 0,
						   val, len);
}

ایستا تهی fdt_nop_region_(تهی *start, صحیح len)
{
	fdt32_t *p;

	برای (p = start; (نویسه *)p < ((نویسه *)start + len); p++)
		*p = cpu_to_fdt32(FDT_NOP);
}

صحیح fdt_nop_property(تهی *fdt, صحیح nodeoffset, ثابت نویسه *name)
{
	ساختار fdt_property *prop;
	صحیح len;

	prop = fdt_get_property_w(fdt, nodeoffset, name, &len);
	اگر (!prop)
		بازگشت len;

	fdt_nop_region_(prop, len + اندازه(*prop));

	بازگشت 0;
}

صحیح fdt_گره_end_offset(تهی *fdt, صحیح offset)
{
	صحیح depth = 0;

	وقتی ((offset >= 0) && (depth >= 0))
		offset = fdt_next_گره(fdt, offset, &depth);

	بازگشت offset;
}

صحیح fdt_nop_گره(تهی *fdt, صحیح nodeoffset)
{
	صحیح endoffset;

	endoffset = fdt_گره_end_offset(fdt, nodeoffset);
	اگر (endoffset < 0)
		بازگشت endoffset;

	fdt_nop_region_(fdt_offset_ptr_w(fdt, nodeoffset, 0),
			endoffset - nodeoffset);
	بازگشت 0;
}
