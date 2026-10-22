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

	إذا ((بنانشانی)proplen < (len + idx))
		أرجع -FDT_ERR_NOSPACE;

	حافظہ_نقل((حرف *)propval + idx, val, len);
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
						   لڑی_لمبائی(name), 0,
						   val, len);
}

ساكن فراغ fdt_nop_region_(فراغ *شروع, صحيح len)
{
	fdt32_t *p;

	لكل (p = شروع; (حرف *)p < ((حرف *)شروع + len); p++)
		*p = cpu_منزل_fdt32(FDT_NOP);
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

صحيح fdt_گرہ_سرا_وتھ(فراغ *fdt, صحيح وتھ)
{
	صحيح ڈونگھائی = 0;

	طالما ((وتھ >= 0) && (ڈونگھائی >= 0))
		وتھ = fdt_اگلا_گرہ(fdt, وتھ, &ڈونگھائی);

	أرجع وتھ;
}

صحيح fdt_nop_گرہ(فراغ *fdt, صحيح nodeoffset)
{
	صحيح endoffset;

	endoffset = fdt_گرہ_سرا_وتھ(fdt, nodeoffset);
	إذا (endoffset < 0)
		أرجع endoffset;

	fdt_nop_region_(fdt_وتھ_ptr_w_2(fdt, nodeoffset, 0),
			endoffset - nodeoffset);
	أرجع 0;
}
