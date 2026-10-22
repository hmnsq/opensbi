// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#ଅନ୍ତର୍ଭାଵ "libfdt_env.h"

#ଅନ୍ତର୍ଭାଵ <fdt.h>
#ଅନ୍ତର୍ଭାଵ <libfdt.h>

#ଅନ୍ତର୍ଭାଵ "libfdt_internal.h"

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_check_full(ଅଚଳ ଶୂନ୍ୟ *fdt, size_t bufsize)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା err;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅଙ୍କ_memrsv;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା offset, nextoffset = 0;
	uint32_t tag;
	unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା depth = 0;
	ଅଚଳ ଶୂନ୍ୟ *prop;
	ଅଚଳ ଅକ୍ଷର *propname;
	bool expect_end = false;

	ଯଦି (bufsize < FDT_V1_SIZE)
		ଫେରନ୍ତୁ -FDT_ERR_TRUNCATED;
	ଯଦି (bufsize < fdt_header_size(fdt))
		ଫେରନ୍ତୁ -FDT_ERR_TRUNCATED;
	err = fdt_check_header(fdt);
	ଯଦି (err != 0)
		ଫେରନ୍ତୁ err;
	ଯଦି (bufsize < fdt_totalsize(fdt))
		ଫେରନ୍ତୁ -FDT_ERR_TRUNCATED;

	ଅଙ୍କ_memrsv = fdt_ଅଙ୍କ_mem_rsv(fdt);
	ଯଦି (ଅଙ୍କ_memrsv < 0)
		ଫେରନ୍ତୁ ଅଙ୍କ_memrsv;

	ଯେତେବେଳେ (1) {
		offset = nextoffset;
		tag = fdt_next_tag(fdt, offset, &nextoffset);

		ଯଦି (nextoffset < 0)
			ଫେରନ୍ତୁ nextoffset;

		/* If we see two root nodes, something is wrong */
		ଯଦି (expect_end && tag != FDT_END)
			ଫେରନ୍ତୁ -FDT_ERR_BADSTRUCTURE;

		ବାଛ (tag) {
		ସ୍ଥିତି FDT_NOP:
			ଭାଙ୍ଗ;

		ସ୍ଥିତି FDT_END:
			ଯଦି (depth != 0)
				ଫେରନ୍ତୁ -FDT_ERR_BADSTRUCTURE;
			ଫେରନ୍ତୁ 0;

		ସ୍ଥିତି FDT_BEGIN_ଗଣ୍ଠି:
			depth++;
			ଯଦି (depth > INT_MAX)
				ଫେରନ୍ତୁ -FDT_ERR_BADSTRUCTURE;

			/* The root node must have an empty name */
			ଯଦି (depth == 1) {
				ଅଚଳ ଅକ୍ଷର *name;
				ପୂର୍ଣ୍ଣସଂଖ୍ୟା len;

				name = fdt_get_name(fdt, offset, &len);
				ଯଦି (*name || len)
					ଫେରନ୍ତୁ -FDT_ERR_BADSTRUCTURE;
			}
			ଭାଙ୍ଗ;

		ସ୍ଥିତି FDT_END_ଗଣ୍ଠି:
			ଯଦି (depth == 0)
				ଫେରନ୍ତୁ -FDT_ERR_BADSTRUCTURE;
			depth--;
			ଯଦି (depth == 0)
				expect_end = true;
			ଭାଙ୍ଗ;

		ସ୍ଥିତି FDT_PROP:
			prop = fdt_getprop_by_offset(fdt, offset, &propname,
						     &err);
			ଯଦି (!prop)
				ଫେରନ୍ତୁ err;
			ଭାଙ୍ଗ;

		ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
			ଫେରନ୍ତୁ -FDT_ERR_INTERNAL;
		}
	}
}
