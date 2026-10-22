// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#include "libfdt_env.h"

#include <fdt.h>
#include <libfdt.h>

#include "libfdt_internal.h"

số_nguyên fdt_check_full(const void *fdt, size_t bufsize)
{
	số_nguyên err;
	số_nguyên num_memrsv;
	số_nguyên offset, nextoffset = 0;
	uint32_t tag;
	unsigned số_nguyên depth = 0;
	const void *prop;
	const char *propname;
	bool expect_end = false;

	nếu (bufsize < FDT_V1_SIZE)
		trả_về -FDT_ERR_TRUNCATED;
	nếu (bufsize < fdt_header_size(fdt))
		trả_về -FDT_ERR_TRUNCATED;
	err = fdt_check_header(fdt);
	nếu (err != 0)
		trả_về err;
	nếu (bufsize < fdt_totalsize(fdt))
		trả_về -FDT_ERR_TRUNCATED;

	num_memrsv = fdt_num_mem_rsv(fdt);
	nếu (num_memrsv < 0)
		trả_về num_memrsv;

	trong_khi (1) {
		offset = nextoffset;
		tag = fdt_next_tag(fdt, offset, &nextoffset);

		nếu (nextoffset < 0)
			trả_về nextoffset;

		/* If we see two root nodes, something is wrong */
		nếu (expect_end && tag != FDT_END)
			trả_về -FDT_ERR_BADSTRUCTURE;

		switch (tag) {
		case FDT_NOP:
			break;

		case FDT_END:
			nếu (depth != 0)
				trả_về -FDT_ERR_BADSTRUCTURE;
			trả_về 0;

		case FDT_BEGIN_NODE:
			depth++;
			nếu (depth > INT_MAX)
				trả_về -FDT_ERR_BADSTRUCTURE;

			/* The root node must have an empty name */
			nếu (depth == 1) {
				const char *name;
				số_nguyên len;

				name = fdt_get_name(fdt, offset, &len);
				nếu (*name || len)
					trả_về -FDT_ERR_BADSTRUCTURE;
			}
			break;

		case FDT_END_NODE:
			nếu (depth == 0)
				trả_về -FDT_ERR_BADSTRUCTURE;
			depth--;
			nếu (depth == 0)
				expect_end = true;
			break;

		case FDT_PROP:
			prop = fdt_getprop_by_offset(fdt, offset, &propname,
						     &err);
			nếu (!prop)
				trả_về err;
			break;

		default:
			trả_về -FDT_ERR_INTERNAL;
		}
	}
}
