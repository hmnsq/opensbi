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

	אם (bufsize < FDT_V1_SIZE)
		החזר -FDT_ERR_TRUNCATED;
	אם (bufsize < fdt_header_size(fdt))
		החזר -FDT_ERR_TRUNCATED;
	err = fdt_check_header(fdt);
	אם (err != 0)
		החזר err;
	אם (bufsize < fdt_totalsize(fdt))
		החזר -FDT_ERR_TRUNCATED;

	num_memrsv = fdt_num_mem_rsv(fdt);
	אם (num_memrsv < 0)
		החזר num_memrsv;

	כלעוד (1) {
		offset = nextoffset;
		tag = fdt_next_tag(fdt, offset, &nextoffset);

		אם (nextoffset < 0)
			החזר nextoffset;

		/* If we see two root nodes, something is wrong */
		אם (expect_end && tag != FDT_END)
			החזר -FDT_ERR_BADSTRUCTURE;

		بدل (tag) {
		حالة FDT_NOP:
			اكسر;

		حالة FDT_END:
			אם (depth != 0)
				החזר -FDT_ERR_BADSTRUCTURE;
			החזר 0;

		حالة FDT_BEGIN_NODE:
			depth++;
			אם (depth > INT_MAX)
				החזר -FDT_ERR_BADSTRUCTURE;

			/* The root node must have an empty name */
			אם (depth == 1) {
				ثابت حرف *name;
				صحيح len;

				name = fdt_get_name(fdt, offset, &len);
				אם (*name || len)
					החזר -FDT_ERR_BADSTRUCTURE;
			}
			اكسر;

		حالة FDT_END_NODE:
			אם (depth == 0)
				החזר -FDT_ERR_BADSTRUCTURE;
			depth--;
			אם (depth == 0)
				expect_end = true;
			اكسر;

		حالة FDT_PROP:
			prop = fdt_getprop_by_offset(fdt, offset, &propname,
						     &err);
			אם (!prop)
				החזר err;
			اكسر;

		افتراضي:
			החזר -FDT_ERR_INTERNAL;
		}
	}
}
