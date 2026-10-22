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

	ᨀᨘ (bufsize < FDT_V1_SIZE)
		ᨀᨘᨒ -FDT_ERR_TRUNCATED;
	ᨀᨘ (bufsize < fdt_header_size(fdt))
		ᨀᨘᨒ -FDT_ERR_TRUNCATED;
	err = fdt_check_header(fdt);
	ᨀᨘ (err != 0)
		ᨀᨘᨒ err;
	ᨀᨘ (bufsize < fdt_totalsize(fdt))
		ᨀᨘᨒ -FDT_ERR_TRUNCATED;

	num_memrsv = fdt_num_mem_rsv(fdt);
	ᨀᨘ (num_memrsv < 0)
		ᨀᨘᨒ num_memrsv;

	ᨀᨛ (1) {
		offset = nextoffset;
		tag = fdt_next_tag(fdt, offset, &nextoffset);

		ᨀᨘ (nextoffset < 0)
			ᨀᨘᨒ nextoffset;

		/* If we see two root nodes, something is wrong */
		ᨀᨘ (expect_end && tag != FDT_END)
			ᨀᨘᨒ -FDT_ERR_BADSTRUCTURE;

		switch (tag) {
		pak_sa FDT_NOP:
			bha_nga;

		pak_sa FDT_END:
			ᨀᨘ (depth != 0)
				ᨀᨘᨒ -FDT_ERR_BADSTRUCTURE;
			ᨀᨘᨒ 0;

		pak_sa FDT_BEGIN_NODE:
			depth++;
			ᨀᨘ (depth > INT_MAX)
				ᨀᨘᨒ -FDT_ERR_BADSTRUCTURE;

			/* The root node must have an empty name */
			ᨀᨘ (depth == 1) {
				const var_na *name;
				puur_naa_nka len;

				name = fdt_get_name(fdt, offset, &len);
				ᨀᨘ (*name || len)
					ᨀᨘᨒ -FDT_ERR_BADSTRUCTURE;
			}
			bha_nga;

		pak_sa FDT_END_NODE:
			ᨀᨘ (depth == 0)
				ᨀᨘᨒ -FDT_ERR_BADSTRUCTURE;
			depth--;
			ᨀᨘ (depth == 0)
				expect_end = true;
			bha_nga;

		pak_sa FDT_PROP:
			prop = fdt_getprop_by_offset(fdt, offset, &propname,
						     &err);
			ᨀᨘ (!prop)
				ᨀᨘᨒ err;
			bha_nga;

		default:
			ᨀᨘᨒ -FDT_ERR_INTERNAL;
		}
	}
}
