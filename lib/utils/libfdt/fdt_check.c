// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#અન્તર્ભાવ "libfdt_env.h"

#અન્તર્ભાવ <fdt.h>
#અન્તર્ભાવ <libfdt.h>

#અન્તર્ભાવ "libfdt_internal.h"

પૂર્ણાંક fdt_check_full(અચલ શૂન્ય *fdt, size_t bufsize)
{
	પૂર્ણાંક err;
	પૂર્ણાંક અંક_memrsv;
	પૂર્ણાંક offset, nextoffset = 0;
	uint32_t tag;
	unsigned પૂર્ણાંક depth = 0;
	અચલ શૂન્ય *prop;
	અચલ અક્ષર *propname;
	bool expect_end = false;

	જો (bufsize < FDT_V1_SIZE)
		પાછા -FDT_ERR_TRUNCATED;
	જો (bufsize < fdt_header_size(fdt))
		પાછા -FDT_ERR_TRUNCATED;
	err = fdt_check_header(fdt);
	જો (err != 0)
		પાછા err;
	જો (bufsize < fdt_totalsize(fdt))
		પાછા -FDT_ERR_TRUNCATED;

	અંક_memrsv = fdt_અંક_mem_rsv(fdt);
	જો (અંક_memrsv < 0)
		પાછા અંક_memrsv;

	જ્યારે (1) {
		offset = nextoffset;
		tag = fdt_next_tag(fdt, offset, &nextoffset);

		જો (nextoffset < 0)
			પાછા nextoffset;

		/* If we see two root nodes, something is wrong */
		જો (expect_end && tag != FDT_END)
			પાછા -FDT_ERR_BADSTRUCTURE;

		પસંદગી (tag) {
		સ્થિતિ FDT_NOP:
			તોડો;

		સ્થિતિ FDT_END:
			જો (depth != 0)
				પાછા -FDT_ERR_BADSTRUCTURE;
			પાછા 0;

		સ્થિતિ FDT_BEGIN_ગાંઠ:
			depth++;
			જો (depth > INT_MAX)
				પાછા -FDT_ERR_BADSTRUCTURE;

			/* The root node must have an empty name */
			જો (depth == 1) {
				અચલ અક્ષર *name;
				પૂર્ણાંક len;

				name = fdt_get_name(fdt, offset, &len);
				જો (*name || len)
					પાછા -FDT_ERR_BADSTRUCTURE;
			}
			તોડો;

		સ્થિતિ FDT_END_ગાંઠ:
			જો (depth == 0)
				પાછા -FDT_ERR_BADSTRUCTURE;
			depth--;
			જો (depth == 0)
				expect_end = true;
			તોડો;

		સ્થિતિ FDT_PROP:
			prop = fdt_getprop_by_offset(fdt, offset, &propname,
						     &err);
			જો (!prop)
				પાછા err;
			તોડો;

		મૂળભૂત:
			પાછા -FDT_ERR_INTERNAL;
		}
	}
}
