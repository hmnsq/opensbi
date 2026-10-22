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

	सचे (bufsize < FDT_V1_SIZE)
		पटिनिवत्त -FDT_ERR_TRUNCATED;
	सचे (bufsize < fdt_header_size(fdt))
		पटिनिवत्त -FDT_ERR_TRUNCATED;
	err = fdt_check_header(fdt);
	सचे (err != 0)
		पटिनिवत्त err;
	सचे (bufsize < fdt_totalsize(fdt))
		पटिनिवत्त -FDT_ERR_TRUNCATED;

	अङ्कः_memrsv = fdt_अङ्कः_mem_rsv(fdt);
	सचे (अङ्कः_memrsv < 0)
		पटिनिवत्त अङ्कः_memrsv;

	याव (1) {
		offset = nextoffset;
		tag = fdt_next_tag(fdt, offset, &nextoffset);

		सचे (nextoffset < 0)
			पटिनिवत्त nextoffset;

		/* If we see two root nodes, something is wrong */
		सचे (expect_end && tag != FDT_END)
			पटिनिवत्त -FDT_ERR_BADSTRUCTURE;

		विकल्प (tag) {
		पक्ष FDT_NOP:
			भङ्ग;

		पक्ष FDT_END:
			सचे (depth != 0)
				पटिनिवत्त -FDT_ERR_BADSTRUCTURE;
			पटिनिवत्त 0;

		पक्ष FDT_BEGIN_ग्रन्थिः:
			depth++;
			सचे (depth > INT_MAX)
				पटिनिवत्त -FDT_ERR_BADSTRUCTURE;

			/* The root node must have an empty name */
			सचे (depth == 1) {
				ध्रुव वर्ण *name;
				पूर्णाङ्क len;

				name = fdt_get_name(fdt, offset, &len);
				सचे (*name || len)
					पटिनिवत्त -FDT_ERR_BADSTRUCTURE;
			}
			भङ्ग;

		पक्ष FDT_END_ग्रन्थिः:
			सचे (depth == 0)
				पटिनिवत्त -FDT_ERR_BADSTRUCTURE;
			depth--;
			सचे (depth == 0)
				expect_end = true;
			भङ्ग;

		पक्ष FDT_PROP:
			prop = fdt_getprop_by_offset(fdt, offset, &propname,
						     &err);
			सचे (!prop)
				पटिनिवत्त err;
			भङ्ग;

		स्वभाव:
			पटिनिवत्त -FDT_ERR_INTERNAL;
		}
	}
}
