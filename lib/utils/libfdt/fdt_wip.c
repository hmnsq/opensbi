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

	إذا ((بیعلامت)proplen < (len + idx))
		أرجع -FDT_ERR_NOSPACE;

	کپی_حافظه((حرف *)propval + idx, val, len);
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
						   طول_رشته(name), 0,
						   val, len);
}

ساكن فراغ fdt_nop_region_(فراغ *شروع, صحيح len)
{
	fdt32_t *p;

	لكل (p = شروع; (حرف *)p < ((حرف *)شروع + len); p++)
		*p = cpu_مقصد_fdt32(FDT_NOP);
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

صحيح fdt_گره_انتها_جابه_جایی(فراغ *fdt, صحيح جابه_جایی)
{
	صحيح عمق = 0;

	طالما ((جابه_جایی >= 0) && (عمق >= 0))
		جابه_جایی = fdt_بعدی_گره(fdt, جابه_جایی, &عمق);

	أرجع جابه_جایی;
}

صحيح fdt_nop_گره(فراغ *fdt, صحيح nodeoffset)
{
	صحيح endoffset;

	endoffset = fdt_گره_انتها_جابه_جایی(fdt, nodeoffset);
	إذا (endoffset < 0)
		أرجع endoffset;

	fdt_nop_region_(fdt_جابه_جایی_ptr_w_2(fdt, nodeoffset, 0),
			endoffset - nodeoffset);
	أرجع 0;
}
