// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#অন্তর্ভাৱ "libfdt_env.h"

#অন্তর্ভাৱ <fdt.h>
#অন্তর্ভাৱ <libfdt.h>

#অন্তর্ভাৱ "libfdt_internal.h"

পূর্ণাঙ্ক fdt_check_full(ধ্রুৱ শূন্য *fdt, size_t bufsize)
{
	পূর্ণাঙ্ক err;
	পূর্ণাঙ্ক অঙ্ক_memrsv;
	পূর্ণাঙ্ক offset, nextoffset = 0;
	uint32_t tag;
	unsigned পূর্ণাঙ্ক depth = 0;
	ধ্রুৱ শূন্য *prop;
	ধ্রুৱ ৱর্ণ *propname;
	bool expect_end = false;

	যদি (bufsize < FDT_V1_SIZE)
		ঘূৰাই -FDT_ERR_TRUNCATED;
	যদি (bufsize < fdt_header_size(fdt))
		ঘূৰাই -FDT_ERR_TRUNCATED;
	err = fdt_check_header(fdt);
	যদি (err != 0)
		ঘূৰাই err;
	যদি (bufsize < fdt_totalsize(fdt))
		ঘূৰাই -FDT_ERR_TRUNCATED;

	অঙ্ক_memrsv = fdt_অঙ্ক_mem_rsv(fdt);
	যদি (অঙ্ক_memrsv < 0)
		ঘূৰাই অঙ্ক_memrsv;

	যেতিয়ালৈ (1) {
		offset = nextoffset;
		tag = fdt_next_tag(fdt, offset, &nextoffset);

		যদি (nextoffset < 0)
			ঘূৰাই nextoffset;

		/* If we see two root nodes, something is wrong */
		যদি (expect_end && tag != FDT_END)
			ঘূৰাই -FDT_ERR_BADSTRUCTURE;

		ৱিকল্প (tag) {
		পক্ষ FDT_NOP:
			ভঙ্গ;

		পক্ষ FDT_END:
			যদি (depth != 0)
				ঘূৰাই -FDT_ERR_BADSTRUCTURE;
			ঘূৰাই 0;

		পক্ষ FDT_BEGIN_গ্ৰন্থি:
			depth++;
			যদি (depth > INT_MAX)
				ঘূৰাই -FDT_ERR_BADSTRUCTURE;

			/* The root node must have an empty name */
			যদি (depth == 1) {
				ধ্রুৱ ৱর্ণ *name;
				পূর্ণাঙ্ক len;

				name = fdt_get_name(fdt, offset, &len);
				যদি (*name || len)
					ঘূৰাই -FDT_ERR_BADSTRUCTURE;
			}
			ভঙ্গ;

		পক্ষ FDT_END_গ্ৰন্থি:
			যদি (depth == 0)
				ঘূৰাই -FDT_ERR_BADSTRUCTURE;
			depth--;
			যদি (depth == 0)
				expect_end = true;
			ভঙ্গ;

		পক্ষ FDT_PROP:
			prop = fdt_getprop_by_offset(fdt, offset, &propname,
						     &err);
			যদি (!prop)
				ঘূৰাই err;
			ভঙ্গ;

		স্ৱভাৱ:
			ঘূৰাই -FDT_ERR_INTERNAL;
		}
	}
}
