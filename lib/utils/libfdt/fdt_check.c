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
	पूर्णाङ्क अङ्कः_memrsv;
	पूर्णाङ्क offset, nextoffset = 0;
	uint32_t tag;
	unsigned पूर्णाङ्क depth = 0;
	ध्रुव शून्य *prop;
	ध्रुव वर्ण *propname;
	bool expect_end = false;

	जइ (bufsize < FDT_V1_SIZE)
		प्रत्यागम -FDT_ERR_TRUNCATED;
	जइ (bufsize < fdt_header_size(fdt))
		प्रत्यागम -FDT_ERR_TRUNCATED;
	err = fdt_check_header(fdt);
	जइ (err != 0)
		प्रत्यागम err;
	जइ (bufsize < fdt_totalsize(fdt))
		प्रत्यागम -FDT_ERR_TRUNCATED;

	अङ्कः_memrsv = fdt_अङ्कः_mem_rsv(fdt);
	जइ (अङ्कः_memrsv < 0)
		प्रत्यागम अङ्कः_memrsv;

	जाव (1) {
		offset = nextoffset;
		tag = fdt_next_tag(fdt, offset, &nextoffset);

		जइ (nextoffset < 0)
			प्रत्यागम nextoffset;

		/* If we see two root nodes, something is wrong */
		जइ (expect_end && tag != FDT_END)
			प्रत्यागम -FDT_ERR_BADSTRUCTURE;

		विकल्प (tag) {
		पक्ष FDT_NOP:
			भङ्ग;

		पक्ष FDT_END:
			जइ (depth != 0)
				प्रत्यागम -FDT_ERR_BADSTRUCTURE;
			प्रत्यागम 0;

		पक्ष FDT_BEGIN_ग्रन्थिः:
			depth++;
			जइ (depth > INT_MAX)
				प्रत्यागम -FDT_ERR_BADSTRUCTURE;

			/* The root node must have an empty name */
			जइ (depth == 1) {
				ध्रुव वर्ण *name;
				पूर्णाङ्क len;

				name = fdt_get_name(fdt, offset, &len);
				जइ (*name || len)
					प्रत्यागम -FDT_ERR_BADSTRUCTURE;
			}
			भङ्ग;

		पक्ष FDT_END_ग्रन्थिः:
			जइ (depth == 0)
				प्रत्यागम -FDT_ERR_BADSTRUCTURE;
			depth--;
			जइ (depth == 0)
				expect_end = true;
			भङ्ग;

		पक्ष FDT_PROP:
			prop = fdt_getprop_by_offset(fdt, offset, &propname,
						     &err);
			जइ (!prop)
				प्रत्यागम err;
			भङ्ग;

		स्वभाव:
			प्रत्यागम -FDT_ERR_INTERNAL;
		}
	}
}
