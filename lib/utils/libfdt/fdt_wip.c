// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#अन्तर्भाव "libfdt_env.h"

#अन्तर्भाव <fdt.h>
#अन्तर्भाव <libfdt.h>

#अन्तर्भाव "libfdt_internal.h"

पूर्णाङ्क fdt_setprop_inplace_namelen_partial(शून्य *fdt, पूर्णाङ्क nodeoffset,
					ध्रुव वर्ण *name, पूर्णाङ्क namelen,
					uint32_t idx, ध्रुव शून्य *val,
					पूर्णाङ्क len)
{
	शून्य *propval;
	पूर्णाङ्क proplen;

	propval = fdt_getprop_namelen_w(fdt, nodeoffset, name, namelen,
					&proplen);
	यदि (!propval)
		प्रत्यागम proplen;

	यदि ((अचिन्हित)proplen < (len + idx))
		प्रत्यागम -FDT_ERR_NOSPACE;

	स_नकल((वर्ण *)propval + idx, val, len);
	प्रत्यागम 0;
}

पूर्णाङ्क fdt_setprop_inplace(शून्य *fdt, पूर्णाङ्क nodeoffset, ध्रुव वर्ण *name,
			ध्रुव शून्य *val, पूर्णाङ्क len)
{
	ध्रुव शून्य *propval;
	पूर्णाङ्क proplen;

	propval = fdt_getprop(fdt, nodeoffset, name, &proplen);
	यदि (!propval)
		प्रत्यागम proplen;

	यदि (proplen != len)
		प्रत्यागम -FDT_ERR_NOSPACE;

	प्रत्यागम fdt_setprop_inplace_namelen_partial(fdt, nodeoffset, name,
						   म_माप(name), 0,
						   val, len);
}

स्थिर शून्य fdt_nop_region_(शून्य *start, पूर्णाङ्क len)
{
	fdt32_t *p;

	प्रति (p = start; (वर्ण *)p < ((वर्ण *)start + len); p++)
		*p = cpu_to_fdt32(FDT_NOP);
}

पूर्णाङ्क fdt_nop_property(शून्य *fdt, पूर्णाङ्क nodeoffset, ध्रुव वर्ण *name)
{
	संरचना fdt_property *prop;
	पूर्णाङ्क len;

	prop = fdt_get_property_w(fdt, nodeoffset, name, &len);
	यदि (!prop)
		प्रत्यागम len;

	fdt_nop_region_(prop, len + परिमाण(*prop));

	प्रत्यागम 0;
}

पूर्णाङ्क fdt_गाँठ_end_offset(शून्य *fdt, पूर्णाङ्क offset)
{
	पूर्णाङ्क depth = 0;

	यावत् ((offset >= 0) && (depth >= 0))
		offset = fdt_next_गाँठ(fdt, offset, &depth);

	प्रत्यागम offset;
}

पूर्णाङ्क fdt_nop_गाँठ(शून्य *fdt, पूर्णाङ्क nodeoffset)
{
	पूर्णाङ्क endoffset;

	endoffset = fdt_गाँठ_end_offset(fdt, nodeoffset);
	यदि (endoffset < 0)
		प्रत्यागम endoffset;

	fdt_nop_region_(fdt_offset_ptr_w(fdt, nodeoffset, 0),
			endoffset - nodeoffset);
	प्रत्यागम 0;
}
