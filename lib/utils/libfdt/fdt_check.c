// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#अन्तर्भाव "libfdt_env.h"

#अन्तर्भाव <fdt.h>
#अन्तर्भाव <libfdt.h>

#अन्तर्भाव "libfdt_internal.h"

पूर्णाङ्क fdt_check_full(अचर शून्य *fdt, माप_प्रकार bufsize)
{
	पूर्णाङ्क err;
	पूर्णाङ्क अङ्क_memrsv;
	पूर्णाङ्क offset, nextoffset = 0;
	uint32_t tag;
	अचिन्हित पूर्णाङ्क depth = 0;
	अचर शून्य *prop;
	अचर वर्ण *propname;
	bool expect_end = false;

	यदि (bufsize < FDT_V1_SIZE)
		फर्काऊ -FDT_ERR_TRUNCATED;
	यदि (bufsize < fdt_header_size(fdt))
		फर्काऊ -FDT_ERR_TRUNCATED;
	err = fdt_check_header(fdt);
	यदि (err != 0)
		फर्काऊ err;
	यदि (bufsize < fdt_totalsize(fdt))
		फर्काऊ -FDT_ERR_TRUNCATED;

	अङ्क_memrsv = fdt_अङ्क_mem_rsv(fdt);
	यदि (अङ्क_memrsv < 0)
		फर्काऊ अङ्क_memrsv;

	जबसम्म (1) {
		offset = nextoffset;
		tag = fdt_next_tag(fdt, offset, &nextoffset);

		यदि (nextoffset < 0)
			फर्काऊ nextoffset;

		/* If we see two root nodes, something is wrong */
		यदि (expect_end && tag != FDT_END)
			फर्काऊ -FDT_ERR_BADSTRUCTURE;

		विकल्प (tag) {
		पक्ष FDT_NOP:
			रोक;

		पक्ष FDT_END:
			यदि (depth != 0)
				फर्काऊ -FDT_ERR_BADSTRUCTURE;
			फर्काऊ 0;

		पक्ष FDT_BEGIN_गाँठो:
			depth++;
			यदि (depth > पूर्णांक_उच्च)
				फर्काऊ -FDT_ERR_BADSTRUCTURE;

			/* The root node must have an empty name */
			यदि (depth == 1) {
				अचर वर्ण *name;
				पूर्णाङ्क len;

				name = fdt_get_name(fdt, offset, &len);
				यदि (*name || len)
					फर्काऊ -FDT_ERR_BADSTRUCTURE;
			}
			रोक;

		पक्ष FDT_END_गाँठो:
			यदि (depth == 0)
				फर्काऊ -FDT_ERR_BADSTRUCTURE;
			depth--;
			यदि (depth == 0)
				expect_end = true;
			रोक;

		पक्ष FDT_PROP:
			prop = fdt_getprop_by_offset(fdt, offset, &propname,
						     &err);
			यदि (!prop)
				फर्काऊ err;
			रोक;

		स्वभाव:
			फर्काऊ -FDT_ERR_INTERNAL;
		}
	}
}
