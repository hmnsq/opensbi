// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#include "libfdt_env.h"

#include <fdt.h>
#include <libfdt.h>

#include "libfdt_internal.h"

целое fdt_check_full(const void *fdt, size_t bufsize)
{
	целое err;
	целое num_memrsv;
	целое offset, nextoffset = 0;
	uint32_t tag;
	unsigned целое depth = 0;
	const void *prop;
	const char *propname;
	bool expect_end = false;

	если (bufsize < FDT_V1_SIZE)
		вернуть -FDT_ERR_TRUNCATED;
	если (bufsize < fdt_header_size(fdt))
		вернуть -FDT_ERR_TRUNCATED;
	err = fdt_check_header(fdt);
	если (err != 0)
		вернуть err;
	если (bufsize < fdt_totalsize(fdt))
		вернуть -FDT_ERR_TRUNCATED;

	num_memrsv = fdt_num_mem_rsv(fdt);
	если (num_memrsv < 0)
		вернуть num_memrsv;

	пока (1) {
		offset = nextoffset;
		tag = fdt_next_tag(fdt, offset, &nextoffset);

		если (nextoffset < 0)
			вернуть nextoffset;

		/* If we see two root nodes, something is wrong */
		если (expect_end && tag != FDT_END)
			вернуть -FDT_ERR_BADSTRUCTURE;

		switch (tag) {
		case FDT_NOP:
			break;

		case FDT_END:
			если (depth != 0)
				вернуть -FDT_ERR_BADSTRUCTURE;
			вернуть 0;

		case FDT_BEGIN_NODE:
			depth++;
			если (depth > INT_MAX)
				вернуть -FDT_ERR_BADSTRUCTURE;

			/* The root node must have an empty name */
			если (depth == 1) {
				const char *name;
				целое len;

				name = fdt_get_name(fdt, offset, &len);
				если (*name || len)
					вернуть -FDT_ERR_BADSTRUCTURE;
			}
			break;

		case FDT_END_NODE:
			если (depth == 0)
				вернуть -FDT_ERR_BADSTRUCTURE;
			depth--;
			если (depth == 0)
				expect_end = true;
			break;

		case FDT_PROP:
			prop = fdt_getprop_by_offset(fdt, offset, &propname,
						     &err);
			если (!prop)
				вернуть err;
			break;

		default:
			вернуть -FDT_ERR_INTERNAL;
		}
	}
}
