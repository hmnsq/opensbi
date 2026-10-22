// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#include "libfdt_env.h"

#include <fdt.h>
#include <libfdt.h>

#include "libfdt_internal.h"

цяло fdt_check_full(const void *fdt, size_t bufsize)
{
	цяло err;
	цяло num_memrsv;
	цяло offset, nextoffset = 0;
	uint32_t tag;
	unsigned цяло depth = 0;
	const void *prop;
	const char *propname;
	bool expect_end = false;

	ако (bufsize < FDT_V1_SIZE)
		върни -FDT_ERR_TRUNCATED;
	ако (bufsize < fdt_header_size(fdt))
		върни -FDT_ERR_TRUNCATED;
	err = fdt_check_header(fdt);
	ако (err != 0)
		върни err;
	ако (bufsize < fdt_totalsize(fdt))
		върни -FDT_ERR_TRUNCATED;

	num_memrsv = fdt_num_mem_rsv(fdt);
	ако (num_memrsv < 0)
		върни num_memrsv;

	докато (1) {
		offset = nextoffset;
		tag = fdt_next_tag(fdt, offset, &nextoffset);

		ако (nextoffset < 0)
			върни nextoffset;

		/* If we see two root nodes, something is wrong */
		ако (expect_end && tag != FDT_END)
			върни -FDT_ERR_BADSTRUCTURE;

		switch (tag) {
		case FDT_NOP:
			break;

		case FDT_END:
			ако (depth != 0)
				върни -FDT_ERR_BADSTRUCTURE;
			върни 0;

		case FDT_BEGIN_NODE:
			depth++;
			ако (depth > INT_MAX)
				върни -FDT_ERR_BADSTRUCTURE;

			/* The root node must have an empty name */
			ако (depth == 1) {
				const char *name;
				цяло len;

				name = fdt_get_name(fdt, offset, &len);
				ако (*name || len)
					върни -FDT_ERR_BADSTRUCTURE;
			}
			break;

		case FDT_END_NODE:
			ако (depth == 0)
				върни -FDT_ERR_BADSTRUCTURE;
			depth--;
			ако (depth == 0)
				expect_end = true;
			break;

		case FDT_PROP:
			prop = fdt_getprop_by_offset(fdt, offset, &propname,
						     &err);
			ако (!prop)
				върни err;
			break;

		default:
			върни -FDT_ERR_INTERNAL;
		}
	}
}
