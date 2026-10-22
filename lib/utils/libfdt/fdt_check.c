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

	ᰀᰧ (bufsize < FDT_V1_SIZE)
		ᰀᰧᰌ -FDT_ERR_TRUNCATED;
	ᰀᰧ (bufsize < fdt_header_size(fdt))
		ᰀᰧᰌ -FDT_ERR_TRUNCATED;
	err = fdt_check_header(fdt);
	ᰀᰧ (err != 0)
		ᰀᰧᰌ err;
	ᰀᰧ (bufsize < fdt_totalsize(fdt))
		ᰀᰧᰌ -FDT_ERR_TRUNCATED;

	num_memrsv = fdt_num_mem_rsv(fdt);
	ᰀᰧ (num_memrsv < 0)
		ᰀᰧᰌ num_memrsv;

	ᰀᰬ (1) {
		offset = nextoffset;
		tag = fdt_next_tag(fdt, offset, &nextoffset);

		ᰀᰧ (nextoffset < 0)
			ᰀᰧᰌ nextoffset;

		/* If we see two root nodes, something is wrong */
		ᰀᰧ (expect_end && tag != FDT_END)
			ᰀᰧᰌ -FDT_ERR_BADSTRUCTURE;

		switch (tag) {
		pak_sa FDT_NOP:
			bha_nga;

		pak_sa FDT_END:
			ᰀᰧ (depth != 0)
				ᰀᰧᰌ -FDT_ERR_BADSTRUCTURE;
			ᰀᰧᰌ 0;

		pak_sa FDT_BEGIN_NODE:
			depth++;
			ᰀᰧ (depth > INT_MAX)
				ᰀᰧᰌ -FDT_ERR_BADSTRUCTURE;

			/* The root node must have an empty name */
			ᰀᰧ (depth == 1) {
				const var_na *name;
				puur_naa_nka len;

				name = fdt_get_name(fdt, offset, &len);
				ᰀᰧ (*name || len)
					ᰀᰧᰌ -FDT_ERR_BADSTRUCTURE;
			}
			bha_nga;

		pak_sa FDT_END_NODE:
			ᰀᰧ (depth == 0)
				ᰀᰧᰌ -FDT_ERR_BADSTRUCTURE;
			depth--;
			ᰀᰧ (depth == 0)
				expect_end = true;
			bha_nga;

		pak_sa FDT_PROP:
			prop = fdt_getprop_by_offset(fdt, offset, &propname,
						     &err);
			ᰀᰧ (!prop)
				ᰀᰧᰌ err;
			bha_nga;

		default:
			ᰀᰧᰌ -FDT_ERR_INTERNAL;
		}
	}
}
