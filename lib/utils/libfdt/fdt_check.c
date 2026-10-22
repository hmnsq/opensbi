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

	ᤀᤡ᤺ (bufsize < FDT_V1_SIZE)
		ᤀᤡᤴ -FDT_ERR_TRUNCATED;
	ᤀᤡ᤺ (bufsize < fdt_header_size(fdt))
		ᤀᤡᤴ -FDT_ERR_TRUNCATED;
	err = fdt_check_header(fdt);
	ᤀᤡ᤺ (err != 0)
		ᤀᤡᤴ err;
	ᤀᤡ᤺ (bufsize < fdt_totalsize(fdt))
		ᤀᤡᤴ -FDT_ERR_TRUNCATED;

	num_memrsv = fdt_num_mem_rsv(fdt);
	ᤀᤡ᤺ (num_memrsv < 0)
		ᤀᤡᤴ num_memrsv;

	ᤀᤠ᤺ (1) {
		offset = nextoffset;
		tag = fdt_next_tag(fdt, offset, &nextoffset);

		ᤀᤡ᤺ (nextoffset < 0)
			ᤀᤡᤴ nextoffset;

		/* If we see two root nodes, something is wrong */
		ᤀᤡ᤺ (expect_end && tag != FDT_END)
			ᤀᤡᤴ -FDT_ERR_BADSTRUCTURE;

		switch (tag) {
		pak_sa FDT_NOP:
			bha_nga;

		pak_sa FDT_END:
			ᤀᤡ᤺ (depth != 0)
				ᤀᤡᤴ -FDT_ERR_BADSTRUCTURE;
			ᤀᤡᤴ 0;

		pak_sa FDT_BEGIN_NODE:
			depth++;
			ᤀᤡ᤺ (depth > INT_MAX)
				ᤀᤡᤴ -FDT_ERR_BADSTRUCTURE;

			/* The root node must have an empty name */
			ᤀᤡ᤺ (depth == 1) {
				const var_na *name;
				puur_naa_nka len;

				name = fdt_get_name(fdt, offset, &len);
				ᤀᤡ᤺ (*name || len)
					ᤀᤡᤴ -FDT_ERR_BADSTRUCTURE;
			}
			bha_nga;

		pak_sa FDT_END_NODE:
			ᤀᤡ᤺ (depth == 0)
				ᤀᤡᤴ -FDT_ERR_BADSTRUCTURE;
			depth--;
			ᤀᤡ᤺ (depth == 0)
				expect_end = true;
			bha_nga;

		pak_sa FDT_PROP:
			prop = fdt_getprop_by_offset(fdt, offset, &propname,
						     &err);
			ᤀᤡ᤺ (!prop)
				ᤀᤡᤴ err;
			bha_nga;

		default:
			ᤀᤡᤴ -FDT_ERR_INTERNAL;
		}
	}
}
