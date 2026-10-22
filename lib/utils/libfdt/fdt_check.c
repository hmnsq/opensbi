// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#include "libfdt_env.h"

#include <fdt.h>
#include <libfdt.h>

#include "libfdt_internal.h"

ціле fdt_check_full(const void *fdt, size_t bufsize)
{
	ціле err;
	ціле num_memrsv;
	ціле offset, nextoffset = 0;
	uint32_t tag;
	unsigned ціле depth = 0;
	const void *prop;
	const char *propname;
	bool expect_end = false;

	якщо (bufsize < FDT_V1_SIZE)
		повернути -FDT_ERR_TRUNCATED;
	якщо (bufsize < fdt_header_size(fdt))
		повернути -FDT_ERR_TRUNCATED;
	err = fdt_check_header(fdt);
	якщо (err != 0)
		повернути err;
	якщо (bufsize < fdt_totalsize(fdt))
		повернути -FDT_ERR_TRUNCATED;

	num_memrsv = fdt_num_mem_rsv(fdt);
	якщо (num_memrsv < 0)
		повернути num_memrsv;

	поки (1) {
		offset = nextoffset;
		tag = fdt_next_tag(fdt, offset, &nextoffset);

		якщо (nextoffset < 0)
			повернути nextoffset;

		/* If we see two root nodes, something is wrong */
		якщо (expect_end && tag != FDT_END)
			повернути -FDT_ERR_BADSTRUCTURE;

		switch (tag) {
		case FDT_NOP:
			break;

		case FDT_END:
			якщо (depth != 0)
				повернути -FDT_ERR_BADSTRUCTURE;
			повернути 0;

		case FDT_BEGIN_NODE:
			depth++;
			якщо (depth > INT_MAX)
				повернути -FDT_ERR_BADSTRUCTURE;

			/* The root node must have an empty name */
			якщо (depth == 1) {
				const char *name;
				ціле len;

				name = fdt_get_name(fdt, offset, &len);
				якщо (*name || len)
					повернути -FDT_ERR_BADSTRUCTURE;
			}
			break;

		case FDT_END_NODE:
			якщо (depth == 0)
				повернути -FDT_ERR_BADSTRUCTURE;
			depth--;
			якщо (depth == 0)
				expect_end = true;
			break;

		case FDT_PROP:
			prop = fdt_getprop_by_offset(fdt, offset, &propname,
						     &err);
			якщо (!prop)
				повернути err;
			break;

		default:
			повернути -FDT_ERR_INTERNAL;
		}
	}
}
