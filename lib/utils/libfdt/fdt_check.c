// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#समावेश "libfdt_env.h"

#समावेश <fdt.h>
#समावेश <libfdt.h>

#समावेश "libfdt_internal.h"

पूर्णांक fdt_check_full(स्थिर व्योम *fdt, माप_प्रकार bufsize)
{
	पूर्णांक err;
	पूर्णांक अंक_memrsv;
	पूर्णांक offset, nextoffset = 0;
	uint32_t tag;
	अचिन्हित पूर्णांक depth = 0;
	स्थिर व्योम *prop;
	स्थिर अक्षर *propname;
	bool expect_end = false;

	अगर (bufsize < FDT_V1_आकार)
		वापस -FDT_ERR_TRUNCATED;
	अगर (bufsize < fdt_header_आकार_2(fdt))
		वापस -FDT_ERR_TRUNCATED;
	err = fdt_check_header(fdt);
	अगर (err != 0)
		वापस err;
	अगर (bufsize < fdt_totalsize(fdt))
		वापस -FDT_ERR_TRUNCATED;

	अंक_memrsv = fdt_अंक_mem_rsv(fdt);
	अगर (अंक_memrsv < 0)
		वापस अंक_memrsv;

	जबतक (1) {
		offset = nextoffset;
		tag = fdt_अग्र_tag(fdt, offset, &nextoffset);

		अगर (nextoffset < 0)
			वापस nextoffset;

		/* If we see two root nodes, something is wrong */
		अगर (expect_end && tag != FDT_END)
			वापस -FDT_ERR_BADSTRUCTURE;

		चयन (tag) {
		हाल FDT_NOP:
			अवरोध;

		हाल FDT_END:
			अगर (depth != 0)
				वापस -FDT_ERR_BADSTRUCTURE;
			वापस 0;

		हाल FDT_BEGIN_गाँठ:
			depth++;
			अगर (depth > पूर्णांक_उच्च)
				वापस -FDT_ERR_BADSTRUCTURE;

			/* The root node must have an empty name */
			अगर (depth == 1) {
				स्थिर अक्षर *name;
				पूर्णांक len;

				name = fdt_get_name(fdt, offset, &len);
				अगर (*name || len)
					वापस -FDT_ERR_BADSTRUCTURE;
			}
			अवरोध;

		हाल FDT_END_गाँठ:
			अगर (depth == 0)
				वापस -FDT_ERR_BADSTRUCTURE;
			depth--;
			अगर (depth == 0)
				expect_end = true;
			अवरोध;

		हाल FDT_PROP:
			prop = fdt_getprop_by_offset(fdt, offset, &propname,
						     &err);
			अगर (!prop)
				वापस err;
			अवरोध;

		शेष:
			वापस -FDT_ERR_INTERNAL;
		}
	}
}
