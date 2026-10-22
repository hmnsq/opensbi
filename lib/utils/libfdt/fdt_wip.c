// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#شامل "libfdt_env.h"

#شامل <fdt.h>
#شامل <libfdt.h>

#شامل "libfdt_internal.h"

صحیح fdt_setprop_inplace_namelen_partial(خالی *fdt, صحیح nodeoffset,
					ثابت حرف *name, صحیح namelen,
					uint32_t idx, ثابت خالی *val,
					صحیح len)
{
	خالی *propval;
	صحیح proplen;

	propval = fdt_getprop_namelen_w(fdt, nodeoffset, name, namelen,
					&proplen);
	اگر (!propval)
		واپس proplen;

	اگر ((غیرعلامتی)proplen < (len + idx))
		واپس -FDT_ERR_NOSPACE;

	حافظہ_نقل((حرف *)propval + idx, val, len);
	واپس 0;
}

صحیح fdt_setprop_inplace(خالی *fdt, صحیح nodeoffset, ثابت حرف *name,
			ثابت خالی *val, صحیح len)
{
	ثابت خالی *propval;
	صحیح proplen;

	propval = fdt_getprop(fdt, nodeoffset, name, &proplen);
	اگر (!propval)
		واپس proplen;

	اگر (proplen != len)
		واپس -FDT_ERR_NOSPACE;

	واپس fdt_setprop_inplace_namelen_partial(fdt, nodeoffset, name,
						   لڑی_لمبائی(name), 0,
						   val, len);
}

ساکن خالی fdt_nop_region_(خالی *آغاز, صحیح len)
{
	fdt32_t *p;

	برائے (p = آغاز; (حرف *)p < ((حرف *)آغاز + len); p++)
		*p = cpu_مقصد_fdt32(FDT_NOP);
}

صحیح fdt_nop_property(خالی *fdt, صحیح nodeoffset, ثابت حرف *name)
{
	ساخت fdt_property *prop;
	صحیح len;

	prop = fdt_get_property_w(fdt, nodeoffset, name, &len);
	اگر (!prop)
		واپس len;

	fdt_nop_region_(prop, len + حجم(*prop));

	واپس 0;
}

صحیح fdt_گرہ_انتہا_انحراف(خالی *fdt, صحیح انحراف)
{
	صحیح گہرائی = 0;

	جب ((انحراف >= 0) && (گہرائی >= 0))
		انحراف = fdt_اگلا_گرہ(fdt, انحراف, &گہرائی);

	واپس انحراف;
}

صحیح fdt_nop_گرہ(خالی *fdt, صحیح nodeoffset)
{
	صحیح endoffset;

	endoffset = fdt_گرہ_انتہا_انحراف(fdt, nodeoffset);
	اگر (endoffset < 0)
		واپس endoffset;

	fdt_nop_region_(fdt_انحراف_ptr_w_2(fdt, nodeoffset, 0),
			endoffset - nodeoffset);
	واپس 0;
}
