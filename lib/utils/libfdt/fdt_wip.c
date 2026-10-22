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
	सचे (!propval)
		पटिनिवत्त proplen;

	सचे ((unsigned)proplen < (len + idx))
		पटिनिवत्त -FDT_ERR_NOSPACE;

	memcpy((वर्ण *)propval + idx, val, len);
	पटिनिवत्त 0;
}

पूर्णाङ्क fdt_setprop_inplace(शून्य *fdt, पूर्णाङ्क nodeoffset, ध्रुव वर्ण *name,
			ध्रुव शून्य *val, पूर्णाङ्क len)
{
	ध्रुव शून्य *propval;
	पूर्णाङ्क proplen;

	propval = fdt_getprop(fdt, nodeoffset, name, &proplen);
	सचे (!propval)
		पटिनिवत्त proplen;

	सचे (proplen != len)
		पटिनिवत्त -FDT_ERR_NOSPACE;

	पटिनिवत्त fdt_setprop_inplace_namelen_partial(fdt, nodeoffset, name,
						   strlen(name), 0,
						   val, len);
}

स्थिर शून्य fdt_nop_region_(शून्य *start, पूर्णाङ्क len)
{
	fdt32_t *p;

	अत्थाय (p = start; (वर्ण *)p < ((वर्ण *)start + len); p++)
		*p = cpu_to_fdt32(FDT_NOP);
}

पूर्णाङ्क fdt_nop_property(शून्य *fdt, पूर्णाङ्क nodeoffset, ध्रुव वर्ण *name)
{
	संरचना fdt_property *prop;
	पूर्णाङ्क len;

	prop = fdt_get_property_w(fdt, nodeoffset, name, &len);
	सचे (!prop)
		पटिनिवत्त len;

	fdt_nop_region_(prop, len + परिमाण(*prop));

	पटिनिवत्त 0;
}

पूर्णाङ्क fdt_ग्रन्थिः_end_offset(शून्य *fdt, पूर्णाङ्क offset)
{
	पूर्णाङ्क depth = 0;

	याव ((offset >= 0) && (depth >= 0))
		offset = fdt_next_ग्रन्थिः(fdt, offset, &depth);

	पटिनिवत्त offset;
}

पूर्णाङ्क fdt_nop_ग्रन्थिः(शून्य *fdt, पूर्णाङ्क nodeoffset)
{
	पूर्णाङ्क endoffset;

	endoffset = fdt_ग्रन्थिः_end_offset(fdt, nodeoffset);
	सचे (endoffset < 0)
		पटिनिवत्त endoffset;

	fdt_nop_region_(fdt_offset_ptr_w(fdt, nodeoffset, 0),
			endoffset - nodeoffset);
	पटिनिवत्त 0;
}
