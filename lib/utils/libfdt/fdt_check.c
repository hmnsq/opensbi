// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#ضمن "libfdt_env.h"

#ضمن <fdt.h>
#ضمن <libfdt.h>

#ضمن "libfdt_internal.h"

صحيح fdt_check_full(ثابت فراغ *fdt, size_t bufsize)
{
	صحيح err;
	صحيح num_memrsv;
	صحيح offset, nextoffset = 0;
	uint32_t tag;
	unsigned صحيح depth = 0;
	ثابت فراغ *prop;
	ثابت حرف *propname;
	bool expect_end = false;

	𐤀𐤌 (bufsize < FDT_V1_SIZE)
		أرجع -FDT_ERR_TRUNCATED;
	𐤀𐤌 (bufsize < fdt_header_size(fdt))
		أرجع -FDT_ERR_TRUNCATED;
	err = fdt_check_header(fdt);
	𐤀𐤌 (err != 0)
		أرجع err;
	𐤀𐤌 (bufsize < fdt_totalsize(fdt))
		أرجع -FDT_ERR_TRUNCATED;

	num_memrsv = fdt_num_mem_rsv(fdt);
	𐤀𐤌 (num_memrsv < 0)
		أرجع num_memrsv;

	طالما (1) {
		offset = nextoffset;
		tag = fdt_next_tag(fdt, offset, &nextoffset);

		𐤀𐤌 (nextoffset < 0)
			أرجع nextoffset;

		/* If we see two root nodes, something is wrong */
		𐤀𐤌 (expect_end && tag != FDT_END)
			أرجع -FDT_ERR_BADSTRUCTURE;

		بدل (tag) {
		حالة FDT_NOP:
			اكسر;

		حالة FDT_END:
			𐤀𐤌 (depth != 0)
				أرجع -FDT_ERR_BADSTRUCTURE;
			أرجع 0;

		حالة FDT_BEGIN_NODE:
			depth++;
			𐤀𐤌 (depth > INT_MAX)
				أرجع -FDT_ERR_BADSTRUCTURE;

			/* The root node must have an empty name */
			𐤀𐤌 (depth == 1) {
				ثابت حرف *name;
				صحيح len;

				name = fdt_get_name(fdt, offset, &len);
				𐤀𐤌 (*name || len)
					أرجع -FDT_ERR_BADSTRUCTURE;
			}
			اكسر;

		حالة FDT_END_NODE:
			𐤀𐤌 (depth == 0)
				أرجع -FDT_ERR_BADSTRUCTURE;
			depth--;
			𐤀𐤌 (depth == 0)
				expect_end = true;
			اكسر;

		حالة FDT_PROP:
			prop = fdt_getprop_by_offset(fdt, offset, &propname,
						     &err);
			𐤀𐤌 (!prop)
				أرجع err;
			اكسر;

		افتراضي:
			أرجع -FDT_ERR_INTERNAL;
		}
	}
}
