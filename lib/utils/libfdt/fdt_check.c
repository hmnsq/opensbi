// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#অন্তর্ভাব "libfdt_env.h"

#অন্তর্ভাব <fdt.h>
#অন্তর্ভাব <libfdt.h>

#অন্তর্ভাব "libfdt_internal.h"

পূর্ণাঙ্ক fdt_check_full(ধ্রুব শূন্য *fdt, size_t bufsize)
{
	পূর্ণাঙ্ক err;
	পূর্ণাঙ্ক অঙ্ক_memrsv;
	পূর্ণাঙ্ক offset, nextoffset = 0;
	uint32_t tag;
	unsigned পূর্ণাঙ্ক depth = 0;
	ধ্রুব শূন্য *prop;
	ধ্রুব বর্ণ *propname;
	bool expect_end = false;

	যদি (bufsize < FDT_V1_SIZE)
		প্রত্যাগম -FDT_ERR_TRUNCATED;
	যদি (bufsize < fdt_header_size(fdt))
		প্রত্যাগম -FDT_ERR_TRUNCATED;
	err = fdt_check_header(fdt);
	যদি (err != 0)
		প্রত্যাগম err;
	যদি (bufsize < fdt_totalsize(fdt))
		প্রত্যাগম -FDT_ERR_TRUNCATED;

	অঙ্ক_memrsv = fdt_অঙ্ক_mem_rsv(fdt);
	যদি (অঙ্ক_memrsv < 0)
		প্রত্যাগম অঙ্ক_memrsv;

	যাবত্ (1) {
		offset = nextoffset;
		tag = fdt_next_tag(fdt, offset, &nextoffset);

		যদি (nextoffset < 0)
			প্রত্যাগম nextoffset;

		/* If we see two root nodes, something is wrong */
		যদি (expect_end && tag != FDT_END)
			প্রত্যাগম -FDT_ERR_BADSTRUCTURE;

		বিকল্প (tag) {
		পক্ষ FDT_NOP:
			ভঙ্গ;

		পক্ষ FDT_END:
			যদি (depth != 0)
				প্রত্যাগম -FDT_ERR_BADSTRUCTURE;
			প্রত্যাগম 0;

		পক্ষ FDT_BEGIN_গ্রন্থি:
			depth++;
			যদি (depth > INT_MAX)
				প্রত্যাগম -FDT_ERR_BADSTRUCTURE;

			/* The root node must have an empty name */
			যদি (depth == 1) {
				ধ্রুব বর্ণ *name;
				পূর্ণাঙ্ক len;

				name = fdt_get_name(fdt, offset, &len);
				যদি (*name || len)
					প্রত্যাগম -FDT_ERR_BADSTRUCTURE;
			}
			ভঙ্গ;

		পক্ষ FDT_END_গ্রন্থি:
			যদি (depth == 0)
				প্রত্যাগম -FDT_ERR_BADSTRUCTURE;
			depth--;
			যদি (depth == 0)
				expect_end = true;
			ভঙ্গ;

		পক্ষ FDT_PROP:
			prop = fdt_getprop_by_offset(fdt, offset, &propname,
						     &err);
			যদি (!prop)
				প্রত্যাগম err;
			ভঙ্গ;

		স্বভাব:
			প্রত্যাগম -FDT_ERR_INTERNAL;
		}
	}
}
