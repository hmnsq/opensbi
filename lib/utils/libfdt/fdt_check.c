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

	ܐܢ (bufsize < FDT_V1_SIZE)
		ܦܢܐ -FDT_ERR_TRUNCATED;
	ܐܢ (bufsize < fdt_header_size(fdt))
		ܦܢܐ -FDT_ERR_TRUNCATED;
	err = fdt_check_header(fdt);
	ܐܢ (err != 0)
		ܦܢܐ err;
	ܐܢ (bufsize < fdt_totalsize(fdt))
		ܦܢܐ -FDT_ERR_TRUNCATED;

	num_memrsv = fdt_num_mem_rsv(fdt);
	ܐܢ (num_memrsv < 0)
		ܦܢܐ num_memrsv;

	ܟܡܐ (1) {
		offset = nextoffset;
		tag = fdt_next_tag(fdt, offset, &nextoffset);

		ܐܢ (nextoffset < 0)
			ܦܢܐ nextoffset;

		/* If we see two root nodes, something is wrong */
		ܐܢ (expect_end && tag != FDT_END)
			ܦܢܐ -FDT_ERR_BADSTRUCTURE;

		بدل (tag) {
		حالة FDT_NOP:
			اكسر;

		حالة FDT_END:
			ܐܢ (depth != 0)
				ܦܢܐ -FDT_ERR_BADSTRUCTURE;
			ܦܢܐ 0;

		حالة FDT_BEGIN_NODE:
			depth++;
			ܐܢ (depth > INT_MAX)
				ܦܢܐ -FDT_ERR_BADSTRUCTURE;

			/* The root node must have an empty name */
			ܐܢ (depth == 1) {
				ثابت حرف *name;
				صحيح len;

				name = fdt_get_name(fdt, offset, &len);
				ܐܢ (*name || len)
					ܦܢܐ -FDT_ERR_BADSTRUCTURE;
			}
			اكسر;

		حالة FDT_END_NODE:
			ܐܢ (depth == 0)
				ܦܢܐ -FDT_ERR_BADSTRUCTURE;
			depth--;
			ܐܢ (depth == 0)
				expect_end = true;
			اكسر;

		حالة FDT_PROP:
			prop = fdt_getprop_by_offset(fdt, offset, &propname,
						     &err);
			ܐܢ (!prop)
				ܦܢܐ err;
			اكسر;

		افتراضي:
			ܦܢܐ -FDT_ERR_INTERNAL;
		}
	}
}
