// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#अन्तर्भाव "libfdt_env.h"

#अन्तर्भाव <fdt.h>
#अन्तर्भाव <libfdt.h>

#अन्तर्भाव "libfdt_internal.h"

पूर्णाङ्क fdt_check_full(ध्रुव शून्य *fdt, size_t bufsize)
{
	पूर्णाङ्क err;
	पूर्णाङ्क अंक_memrsv;
	पूर्णाङ्क offset, nextoffset = 0;
	uint32_t tag;
	unsigned पूर्णाङ्क depth = 0;
	ध्रुव शून्य *prop;
	ध्रुव वर्ण *propname;
	bool expect_end = false;

	यदि (bufsize < FDT_V1_SIZE)
		प्रत्यागम -FDT_ERR_TRUNCATED;
	यदि (bufsize < fdt_header_size(fdt))
		प्रत्यागम -FDT_ERR_TRUNCATED;
	err = fdt_check_header(fdt);
	यदि (err != 0)
		प्रत्यागम err;
	यदि (bufsize < fdt_totalsize(fdt))
		प्रत्यागम -FDT_ERR_TRUNCATED;

	अंक_memrsv = fdt_अंक_mem_rsv(fdt);
	यदि (अंक_memrsv < 0)
		प्रत्यागम अंक_memrsv;

	यावत् (1) {
		offset = nextoffset;
		tag = fdt_next_tag(fdt, offset, &nextoffset);

		यदि (nextoffset < 0)
			प्रत्यागम nextoffset;

		/* If we see two root nodes, something is wrong */
		यदि (expect_end && tag != FDT_END)
			प्रत्यागम -FDT_ERR_BADSTRUCTURE;

		विकल्प (tag) {
		पक्ष FDT_NOP:
			भङ्ग;

		पक्ष FDT_END:
			यदि (depth != 0)
				प्रत्यागम -FDT_ERR_BADSTRUCTURE;
			प्रत्यागम 0;

		पक्ष FDT_BEGIN_गाठ:
			depth++;
			यदि (depth > INT_MAX)
				प्रत्यागम -FDT_ERR_BADSTRUCTURE;

			/* The root node must have an empty name */
			यदि (depth == 1) {
				ध्रुव वर्ण *name;
				पूर्णाङ्क len;

				name = fdt_get_name(fdt, offset, &len);
				यदि (*name || len)
					प्रत्यागम -FDT_ERR_BADSTRUCTURE;
			}
			भङ्ग;

		पक्ष FDT_END_गाठ:
			यदि (depth == 0)
				प्रत्यागम -FDT_ERR_BADSTRUCTURE;
			depth--;
			यदि (depth == 0)
				expect_end = true;
			भङ्ग;

		पक्ष FDT_PROP:
			prop = fdt_getprop_by_offset(fdt, offset, &propname,
						     &err);
			यदि (!prop)
				प्रत्यागम err;
			भङ्ग;

		स्वभाव:
			प्रत्यागम -FDT_ERR_INTERNAL;
		}
	}
}
