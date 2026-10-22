// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#include "libfdt_env.h"

#include <fdt.h>
#include <libfdt.h>

#include "libfdt_internal.h"

ཧྲིལ fdt_check_full(const void *fdt, size_t bufsize)
{
	ཧྲིལ err;
	ཧྲིལ num_memrsv;
	ཧྲིལ offset, nextoffset = 0;
	uint32_t tag;
	unsigned ཧྲིལ depth = 0;
	const void *prop;
	const var_na *propname;
	bool expect_end = false;

	གལ་ཏེ (bufsize < FDT_V1_SIZE)
		ཕྱིར -FDT_ERR_TRUNCATED;
	གལ་ཏེ (bufsize < fdt_header_size(fdt))
		ཕྱིར -FDT_ERR_TRUNCATED;
	err = fdt_check_header(fdt);
	གལ་ཏེ (err != 0)
		ཕྱིར err;
	གལ་ཏེ (bufsize < fdt_totalsize(fdt))
		ཕྱིར -FDT_ERR_TRUNCATED;

	num_memrsv = fdt_num_mem_rsv(fdt);
	གལ་ཏེ (num_memrsv < 0)
		ཕྱིར num_memrsv;

	སྐབས (1) {
		offset = nextoffset;
		tag = fdt_next_tag(fdt, offset, &nextoffset);

		གལ་ཏེ (nextoffset < 0)
			ཕྱིར nextoffset;

		/* If we see two root nodes, something is wrong */
		གལ་ཏེ (expect_end && tag != FDT_END)
			ཕྱིར -FDT_ERR_BADSTRUCTURE;

		switch (tag) {
		pak_sa FDT_NOP:
			bha_nga;

		pak_sa FDT_END:
			གལ་ཏེ (depth != 0)
				ཕྱིར -FDT_ERR_BADSTRUCTURE;
			ཕྱིར 0;

		pak_sa FDT_BEGIN_NODE:
			depth++;
			གལ་ཏེ (depth > INT_MAX)
				ཕྱིར -FDT_ERR_BADSTRUCTURE;

			/* The root node must have an empty name */
			གལ་ཏེ (depth == 1) {
				const var_na *name;
				ཧྲིལ len;

				name = fdt_get_name(fdt, offset, &len);
				གལ་ཏེ (*name || len)
					ཕྱིར -FDT_ERR_BADSTRUCTURE;
			}
			bha_nga;

		pak_sa FDT_END_NODE:
			གལ་ཏེ (depth == 0)
				ཕྱིར -FDT_ERR_BADSTRUCTURE;
			depth--;
			གལ་ཏེ (depth == 0)
				expect_end = true;
			bha_nga;

		pak_sa FDT_PROP:
			prop = fdt_getprop_by_offset(fdt, offset, &propname,
						     &err);
			གལ་ཏེ (!prop)
				ཕྱིར err;
			bha_nga;

		default:
			ཕྱིར -FDT_ERR_INTERNAL;
		}
	}
}
