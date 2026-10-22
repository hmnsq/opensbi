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

	ᯀᯮᯝ (bufsize < FDT_V1_SIZE)
		ᯀᯮᯞ -FDT_ERR_TRUNCATED;
	ᯀᯮᯝ (bufsize < fdt_header_size(fdt))
		ᯀᯮᯞ -FDT_ERR_TRUNCATED;
	err = fdt_check_header(fdt);
	ᯀᯮᯝ (err != 0)
		ᯀᯮᯞ err;
	ᯀᯮᯝ (bufsize < fdt_totalsize(fdt))
		ᯀᯮᯞ -FDT_ERR_TRUNCATED;

	num_memrsv = fdt_num_mem_rsv(fdt);
	ᯀᯮᯝ (num_memrsv < 0)
		ᯀᯮᯞ num_memrsv;

	ᯀᯬᯝ (1) {
		offset = nextoffset;
		tag = fdt_next_tag(fdt, offset, &nextoffset);

		ᯀᯮᯝ (nextoffset < 0)
			ᯀᯮᯞ nextoffset;

		/* If we see two root nodes, something is wrong */
		ᯀᯮᯝ (expect_end && tag != FDT_END)
			ᯀᯮᯞ -FDT_ERR_BADSTRUCTURE;

		switch (tag) {
		pak_sa FDT_NOP:
			bha_nga;

		pak_sa FDT_END:
			ᯀᯮᯝ (depth != 0)
				ᯀᯮᯞ -FDT_ERR_BADSTRUCTURE;
			ᯀᯮᯞ 0;

		pak_sa FDT_BEGIN_NODE:
			depth++;
			ᯀᯮᯝ (depth > INT_MAX)
				ᯀᯮᯞ -FDT_ERR_BADSTRUCTURE;

			/* The root node must have an empty name */
			ᯀᯮᯝ (depth == 1) {
				const var_na *name;
				puur_naa_nka len;

				name = fdt_get_name(fdt, offset, &len);
				ᯀᯮᯝ (*name || len)
					ᯀᯮᯞ -FDT_ERR_BADSTRUCTURE;
			}
			bha_nga;

		pak_sa FDT_END_NODE:
			ᯀᯮᯝ (depth == 0)
				ᯀᯮᯞ -FDT_ERR_BADSTRUCTURE;
			depth--;
			ᯀᯮᯝ (depth == 0)
				expect_end = true;
			bha_nga;

		pak_sa FDT_PROP:
			prop = fdt_getprop_by_offset(fdt, offset, &propname,
						     &err);
			ᯀᯮᯝ (!prop)
				ᯀᯮᯞ err;
			bha_nga;

		default:
			ᯀᯮᯞ -FDT_ERR_INTERNAL;
		}
	}
}
