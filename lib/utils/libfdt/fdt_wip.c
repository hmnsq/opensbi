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

	यदि ((अचिह्नित)proplen < (len + idx))
		प्रत्यागम -FDT_ERR_NOSPACE;

	स्मृतिं_प्रतिलिख((वर्ण *)propval + idx, val, len);
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
						   वाक्यदैर्घ्यम्(name), 0,
						   val, len);
}

स्थिर शून्य fdt_nop_region_(शून्य *start, पूर्णाङ्क len)
{
	fdt32_t *p;

	प्रति (p = start; (वर्ण *)p < ((वर्ण *)start + len); p++)
		*p = cpu_गन्तव्यम्_fdt32(FDT_NOP);
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

पूर्णाङ्क fdt_ग्रन्थिः_अन्तः_विस्थापनम्(शून्य *fdt, पूर्णाङ्क विस्थापनम्)
{
	पूर्णाङ्क गभीरता = 0;

	यावत् ((विस्थापनम् >= 0) && (गभीरता >= 0))
		विस्थापनम् = fdt_अग्र_ग्रन्थिः(fdt, विस्थापनम्, &गभीरता);

	प्रत्यागम विस्थापनम्;
}

पूर्णाङ्क fdt_nop_ग्रन्थिः(शून्य *fdt, पूर्णाङ्क nodeoffset)
{
	पूर्णाङ्क endoffset;

	endoffset = fdt_ग्रन्थिः_अन्तः_विस्थापनम्(fdt, nodeoffset);
	यदि (endoffset < 0)
		प्रत्यागम endoffset;

	fdt_nop_region_(fdt_विस्थापनम्_ptr_w_2(fdt, nodeoffset, 0),
			endoffset - nodeoffset);
	प्रत्यागम 0;
}
