// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#অন্তর্ভাব "libfdt_env.h"

#অন্তর্ভাব <fdt.h>
#অন্তর্ভাব <libfdt.h>

#অন্তর্ভাব "libfdt_internal.h"

পূর্ণসংখ্যা fdt_check_full(ধ্রুবক শূন্য *fdt, size_t bufsize)
{
	পূর্ণসংখ্যা err;
	পূর্ণসংখ্যা অঙ্ক_memrsv;
	পূর্ণসংখ্যা offset, nextoffset = 0;
	uint32_t tag;
	unsigned পূর্ণসংখ্যা depth = 0;
	ধ্রুবক শূন্য *prop;
	ধ্রুবক অক্ষর *propname;
	bool expect_end = false;

	যদি (bufsize < FDT_V1_আকার)
		ফেরত -FDT_ERR_TRUNCATED;
	যদি (bufsize < fdt_header_আকার_2(fdt))
		ফেরত -FDT_ERR_TRUNCATED;
	err = fdt_check_header(fdt);
	যদি (err != 0)
		ফেরত err;
	যদি (bufsize < fdt_totalsize(fdt))
		ফেরত -FDT_ERR_TRUNCATED;

	অঙ্ক_memrsv = fdt_অঙ্ক_mem_rsv(fdt);
	যদি (অঙ্ক_memrsv < 0)
		ফেরত অঙ্ক_memrsv;

	যখন (1) {
		offset = nextoffset;
		tag = fdt_পরবর্তী_tag(fdt, offset, &nextoffset);

		যদি (nextoffset < 0)
			ফেরত nextoffset;

		/* If we see two root nodes, something is wrong */
		যদি (expect_end && tag != FDT_END)
			ফেরত -FDT_ERR_BADSTRUCTURE;

		নির্বাচন (tag) {
		ক্ষেত্র FDT_NOP:
			বিরতি;

		ক্ষেত্র FDT_END:
			যদি (depth != 0)
				ফেরত -FDT_ERR_BADSTRUCTURE;
			ফেরত 0;

		ক্ষেত্র FDT_BEGIN_গ্রন্থি:
			depth++;
			যদি (depth > INT_MAX)
				ফেরত -FDT_ERR_BADSTRUCTURE;

			/* The root node must have an empty name */
			যদি (depth == 1) {
				ধ্রুবক অক্ষর *name;
				পূর্ণসংখ্যা len;

				name = fdt_get_name(fdt, offset, &len);
				যদি (*name || len)
					ফেরত -FDT_ERR_BADSTRUCTURE;
			}
			বিরতি;

		ক্ষেত্র FDT_END_গ্রন্থি:
			যদি (depth == 0)
				ফেরত -FDT_ERR_BADSTRUCTURE;
			depth--;
			যদি (depth == 0)
				expect_end = true;
			বিরতি;

		ক্ষেত্র FDT_PROP:
			prop = fdt_getprop_by_offset(fdt, offset, &propname,
						     &err);
			যদি (!prop)
				ফেরত err;
			বিরতি;

		পূর্বনির্ধারিত:
			ফেরত -FDT_ERR_INTERNAL;
		}
	}
}
