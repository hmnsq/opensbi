// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#अन्तर्भाव "libfdt_env.h"

#अन्तर्भाव <fdt.h>
#अन्तर्भाव <libfdt.h>

#अन्तर्भाव "libfdt_internal.h"

पूर्णांक fdt_setprop_inplace_namelen_partial(शून्य *fdt, पूर्णांक nodeoffset,
					स्थिरांक अक्षर *name, पूर्णांक namelen,
					uint32_t idx, स्थिरांक शून्य *val,
					पूर्णांक len)
{
	शून्य *propval;
	पूर्णांक proplen;

	propval = fdt_getprop_namelen_w(fdt, nodeoffset, name, namelen,
					&proplen);
	जर (!propval)
		परत proplen;

	जर ((अचिन्हित)proplen < (len + idx))
		परत -FDT_ERR_NOSPACE;

	स_नकल((अक्षर *)propval + idx, val, len);
	परत 0;
}

पूर्णांक fdt_setprop_inplace(शून्य *fdt, पूर्णांक nodeoffset, स्थिरांक अक्षर *name,
			स्थिरांक शून्य *val, पूर्णांक len)
{
	स्थिरांक शून्य *propval;
	पूर्णांक proplen;

	propval = fdt_getprop(fdt, nodeoffset, name, &proplen);
	जर (!propval)
		परत proplen;

	जर (proplen != len)
		परत -FDT_ERR_NOSPACE;

	परत fdt_setprop_inplace_namelen_partial(fdt, nodeoffset, name,
						   म_माप(name), 0,
						   val, len);
}

स्थिर शून्य fdt_nop_region_(शून्य *start, पूर्णांक len)
{
	fdt32_t *p;

	साठी (p = start; (अक्षर *)p < ((अक्षर *)start + len); p++)
		*p = cpu_to_fdt32(FDT_NOP);
}

पूर्णांक fdt_nop_property(शून्य *fdt, पूर्णांक nodeoffset, स्थिरांक अक्षर *name)
{
	संरचना fdt_property *prop;
	पूर्णांक len;

	prop = fdt_get_property_w(fdt, nodeoffset, name, &len);
	जर (!prop)
		परत len;

	fdt_nop_region_(prop, len + आकार(*prop));

	परत 0;
}

पूर्णांक fdt_गाठ_end_offset(शून्य *fdt, पूर्णांक offset)
{
	पूर्णांक depth = 0;

	पर्यंत ((offset >= 0) && (depth >= 0))
		offset = fdt_next_गाठ(fdt, offset, &depth);

	परत offset;
}

पूर्णांक fdt_nop_गाठ(शून्य *fdt, पूर्णांक nodeoffset)
{
	पूर्णांक endoffset;

	endoffset = fdt_गाठ_end_offset(fdt, nodeoffset);
	जर (endoffset < 0)
		परत endoffset;

	fdt_nop_region_(fdt_offset_ptr_w(fdt, nodeoffset, 0),
			endoffset - nodeoffset);
	परत 0;
}
