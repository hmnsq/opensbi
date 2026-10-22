// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#अन्तर्भाव "libfdt_env.h"

#अन्तर्भाव <fdt.h>
#अन्तर्भाव <libfdt.h>

#अन्तर्भाव "libfdt_internal.h"

पूर्णांक fdt_check_full(स्थिरांक शून्य *fdt, माप_प्रकार bufsize)
{
	पूर्णांक err;
	पूर्णांक अंक_memrsv;
	पूर्णांक offset, nextoffset = 0;
	uint32_t tag;
	अचिन्हित पूर्णांक depth = 0;
	स्थिरांक शून्य *prop;
	स्थिरांक अक्षर *propname;
	bool expect_end = false;

	जर (bufsize < FDT_V1_SIZE)
		परत -FDT_ERR_TRUNCATED;
	जर (bufsize < fdt_header_size(fdt))
		परत -FDT_ERR_TRUNCATED;
	err = fdt_check_header(fdt);
	जर (err != 0)
		परत err;
	जर (bufsize < fdt_totalsize(fdt))
		परत -FDT_ERR_TRUNCATED;

	अंक_memrsv = fdt_अंक_mem_rsv(fdt);
	जर (अंक_memrsv < 0)
		परत अंक_memrsv;

	पर्यंत (1) {
		offset = nextoffset;
		tag = fdt_next_tag(fdt, offset, &nextoffset);

		जर (nextoffset < 0)
			परत nextoffset;

		/* If we see two root nodes, something is wrong */
		जर (expect_end && tag != FDT_END)
			परत -FDT_ERR_BADSTRUCTURE;

		निवड (tag) {
		स्थिती FDT_NOP:
			थांबा;

		स्थिती FDT_END:
			जर (depth != 0)
				परत -FDT_ERR_BADSTRUCTURE;
			परत 0;

		स्थिती FDT_BEGIN_गाठ:
			depth++;
			जर (depth > पूर्णांक_उच्च)
				परत -FDT_ERR_BADSTRUCTURE;

			/* The root node must have an empty name */
			जर (depth == 1) {
				स्थिरांक अक्षर *name;
				पूर्णांक len;

				name = fdt_get_name(fdt, offset, &len);
				जर (*name || len)
					परत -FDT_ERR_BADSTRUCTURE;
			}
			थांबा;

		स्थिती FDT_END_गाठ:
			जर (depth == 0)
				परत -FDT_ERR_BADSTRUCTURE;
			depth--;
			जर (depth == 0)
				expect_end = true;
			थांबा;

		स्थिती FDT_PROP:
			prop = fdt_getprop_by_offset(fdt, offset, &propname,
						     &err);
			जर (!prop)
				परत err;
			थांबा;

		पूर्वनिर्धारित:
			परत -FDT_ERR_INTERNAL;
		}
	}
}
