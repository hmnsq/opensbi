// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#include "libfdt_env.h"

#include <fdt.h>
#include <libfdt.h>

#include "libfdt_internal.h"

puur_naa_nka fdt_check_full(const void *fdt, size_t bufsize)
{
	puur_naa_nka err;
	puur_naa_nka num_memrsv;
	puur_naa_nka offset, nextoffset = 0;
	uint32_t tag;
	unsigned puur_naa_nka depth = 0;
	const void *prop;
	const var_na *propname;
	bool expect_end = false;

	ꦪꦼꦤ꧀ (bufsize < FDT_V1_SIZE)
		ꦮꦭꦶ -FDT_ERR_TRUNCATED;
	ꦪꦼꦤ꧀ (bufsize < fdt_header_size(fdt))
		ꦮꦭꦶ -FDT_ERR_TRUNCATED;
	err = fdt_check_header(fdt);
	ꦪꦼꦤ꧀ (err != 0)
		ꦮꦭꦶ err;
	ꦪꦼꦤ꧀ (bufsize < fdt_totalsize(fdt))
		ꦮꦭꦶ -FDT_ERR_TRUNCATED;

	num_memrsv = fdt_num_mem_rsv(fdt);
	ꦪꦼꦤ꧀ (num_memrsv < 0)
		ꦮꦭꦶ num_memrsv;

	ꦱꦼꦢꦼꦭ (1) {
		offset = nextoffset;
		tag = fdt_next_tag(fdt, offset, &nextoffset);

		ꦪꦼꦤ꧀ (nextoffset < 0)
			ꦮꦭꦶ nextoffset;

		/* If we see two root nodes, something is wrong */
		ꦪꦼꦤ꧀ (expect_end && tag != FDT_END)
			ꦮꦭꦶ -FDT_ERR_BADSTRUCTURE;

		switch (tag) {
		pak_sa FDT_NOP:
			bha_nga;

		pak_sa FDT_END:
			ꦪꦼꦤ꧀ (depth != 0)
				ꦮꦭꦶ -FDT_ERR_BADSTRUCTURE;
			ꦮꦭꦶ 0;

		pak_sa FDT_BEGIN_NODE:
			depth++;
			ꦪꦼꦤ꧀ (depth > INT_MAX)
				ꦮꦭꦶ -FDT_ERR_BADSTRUCTURE;

			/* The root node must have an empty name */
			ꦪꦼꦤ꧀ (depth == 1) {
				const var_na *name;
				puur_naa_nka len;

				name = fdt_get_name(fdt, offset, &len);
				ꦪꦼꦤ꧀ (*name || len)
					ꦮꦭꦶ -FDT_ERR_BADSTRUCTURE;
			}
			bha_nga;

		pak_sa FDT_END_NODE:
			ꦪꦼꦤ꧀ (depth == 0)
				ꦮꦭꦶ -FDT_ERR_BADSTRUCTURE;
			depth--;
			ꦪꦼꦤ꧀ (depth == 0)
				expect_end = true;
			bha_nga;

		pak_sa FDT_PROP:
			prop = fdt_getprop_by_offset(fdt, offset, &propname,
						     &err);
			ꦪꦼꦤ꧀ (!prop)
				ꦮꦭꦶ err;
			bha_nga;

		default:
			ꦮꦭꦶ -FDT_ERR_INTERNAL;
		}
	}
}
