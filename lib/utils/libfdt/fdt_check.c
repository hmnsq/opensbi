// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#include "libfdt_env.h"

#include <fdt.h>
#include <libfdt.h>

#include "libfdt_internal.h"

整数 fdt_check_full(const void *fdt, size_t bufsize)
{
	整数 err;
	整数 num_memrsv;
	整数 offset, nextoffset = 0;
	uint32_t tag;
	unsigned 整数 depth = 0;
	const void *prop;
	const char *propname;
	bool expect_end = false;

	如果 (bufsize < FDT_V1_SIZE)
		返回 -FDT_ERR_TRUNCATED;
	如果 (bufsize < fdt_header_size(fdt))
		返回 -FDT_ERR_TRUNCATED;
	err = fdt_check_header(fdt);
	如果 (err != 0)
		返回 err;
	如果 (bufsize < fdt_totalsize(fdt))
		返回 -FDT_ERR_TRUNCATED;

	num_memrsv = fdt_num_mem_rsv(fdt);
	如果 (num_memrsv < 0)
		返回 num_memrsv;

	当 (1) {
		offset = nextoffset;
		tag = fdt_next_tag(fdt, offset, &nextoffset);

		如果 (nextoffset < 0)
			返回 nextoffset;

		/* If we see two root nodes, something is wrong */
		如果 (expect_end && tag != FDT_END)
			返回 -FDT_ERR_BADSTRUCTURE;

		switch (tag) {
		case FDT_NOP:
			break;

		case FDT_END:
			如果 (depth != 0)
				返回 -FDT_ERR_BADSTRUCTURE;
			返回 0;

		case FDT_BEGIN_NODE:
			depth++;
			如果 (depth > INT_MAX)
				返回 -FDT_ERR_BADSTRUCTURE;

			/* The root node must have an empty name */
			如果 (depth == 1) {
				const char *name;
				整数 len;

				name = fdt_get_name(fdt, offset, &len);
				如果 (*name || len)
					返回 -FDT_ERR_BADSTRUCTURE;
			}
			break;

		case FDT_END_NODE:
			如果 (depth == 0)
				返回 -FDT_ERR_BADSTRUCTURE;
			depth--;
			如果 (depth == 0)
				expect_end = true;
			break;

		case FDT_PROP:
			prop = fdt_getprop_by_offset(fdt, offset, &propname,
						     &err);
			如果 (!prop)
				返回 err;
			break;

		default:
			返回 -FDT_ERR_INTERNAL;
		}
	}
}
