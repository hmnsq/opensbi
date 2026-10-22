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

	もし (bufsize < FDT_V1_SIZE)
		戻る -FDT_ERR_TRUNCATED;
	もし (bufsize < fdt_header_size(fdt))
		戻る -FDT_ERR_TRUNCATED;
	err = fdt_check_header(fdt);
	もし (err != 0)
		戻る err;
	もし (bufsize < fdt_totalsize(fdt))
		戻る -FDT_ERR_TRUNCATED;

	num_memrsv = fdt_num_mem_rsv(fdt);
	もし (num_memrsv < 0)
		戻る num_memrsv;

	ながら (1) {
		offset = nextoffset;
		tag = fdt_next_tag(fdt, offset, &nextoffset);

		もし (nextoffset < 0)
			戻る nextoffset;

		/* If we see two root nodes, something is wrong */
		もし (expect_end && tag != FDT_END)
			戻る -FDT_ERR_BADSTRUCTURE;

		switch (tag) {
		case FDT_NOP:
			break;

		case FDT_END:
			もし (depth != 0)
				戻る -FDT_ERR_BADSTRUCTURE;
			戻る 0;

		case FDT_BEGIN_NODE:
			depth++;
			もし (depth > INT_MAX)
				戻る -FDT_ERR_BADSTRUCTURE;

			/* The root node must have an empty name */
			もし (depth == 1) {
				const char *name;
				整数 len;

				name = fdt_get_name(fdt, offset, &len);
				もし (*name || len)
					戻る -FDT_ERR_BADSTRUCTURE;
			}
			break;

		case FDT_END_NODE:
			もし (depth == 0)
				戻る -FDT_ERR_BADSTRUCTURE;
			depth--;
			もし (depth == 0)
				expect_end = true;
			break;

		case FDT_PROP:
			prop = fdt_getprop_by_offset(fdt, offset, &propname,
						     &err);
			もし (!prop)
				戻る err;
			break;

		default:
			戻る -FDT_ERR_INTERNAL;
		}
	}
}
