// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#include "libfdt_env.h"

#include <fdt.h>
#include <libfdt.h>

#include "libfdt_internal.h"

정수 fdt_check_full(const void *fdt, size_t bufsize)
{
	정수 err;
	정수 num_memrsv;
	정수 offset, nextoffset = 0;
	uint32_t tag;
	unsigned 정수 depth = 0;
	const void *prop;
	const char *propname;
	bool expect_end = false;

	만약 (bufsize < FDT_V1_SIZE)
		반환 -FDT_ERR_TRUNCATED;
	만약 (bufsize < fdt_header_size(fdt))
		반환 -FDT_ERR_TRUNCATED;
	err = fdt_check_header(fdt);
	만약 (err != 0)
		반환 err;
	만약 (bufsize < fdt_totalsize(fdt))
		반환 -FDT_ERR_TRUNCATED;

	num_memrsv = fdt_num_mem_rsv(fdt);
	만약 (num_memrsv < 0)
		반환 num_memrsv;

	동안 (1) {
		offset = nextoffset;
		tag = fdt_next_tag(fdt, offset, &nextoffset);

		만약 (nextoffset < 0)
			반환 nextoffset;

		/* If we see two root nodes, something is wrong */
		만약 (expect_end && tag != FDT_END)
			반환 -FDT_ERR_BADSTRUCTURE;

		switch (tag) {
		case FDT_NOP:
			break;

		case FDT_END:
			만약 (depth != 0)
				반환 -FDT_ERR_BADSTRUCTURE;
			반환 0;

		case FDT_BEGIN_NODE:
			depth++;
			만약 (depth > INT_MAX)
				반환 -FDT_ERR_BADSTRUCTURE;

			/* The root node must have an empty name */
			만약 (depth == 1) {
				const char *name;
				정수 len;

				name = fdt_get_name(fdt, offset, &len);
				만약 (*name || len)
					반환 -FDT_ERR_BADSTRUCTURE;
			}
			break;

		case FDT_END_NODE:
			만약 (depth == 0)
				반환 -FDT_ERR_BADSTRUCTURE;
			depth--;
			만약 (depth == 0)
				expect_end = true;
			break;

		case FDT_PROP:
			prop = fdt_getprop_by_offset(fdt, offset, &propname,
						     &err);
			만약 (!prop)
				반환 err;
			break;

		default:
			반환 -FDT_ERR_INTERNAL;
		}
	}
}
