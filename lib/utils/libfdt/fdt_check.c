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

	ᮚᮦᮔ᮪ (bufsize < FDT_V1_SIZE)
		ᮝᮜᮤ -FDT_ERR_TRUNCATED;
	ᮚᮦᮔ᮪ (bufsize < fdt_header_size(fdt))
		ᮝᮜᮤ -FDT_ERR_TRUNCATED;
	err = fdt_check_header(fdt);
	ᮚᮦᮔ᮪ (err != 0)
		ᮝᮜᮤ err;
	ᮚᮦᮔ᮪ (bufsize < fdt_totalsize(fdt))
		ᮝᮜᮤ -FDT_ERR_TRUNCATED;

	num_memrsv = fdt_num_mem_rsv(fdt);
	ᮚᮦᮔ᮪ (num_memrsv < 0)
		ᮝᮜᮤ num_memrsv;

	ᮞᮨᮓᮨᮜ (1) {
		offset = nextoffset;
		tag = fdt_next_tag(fdt, offset, &nextoffset);

		ᮚᮦᮔ᮪ (nextoffset < 0)
			ᮝᮜᮤ nextoffset;

		/* If we see two root nodes, something is wrong */
		ᮚᮦᮔ᮪ (expect_end && tag != FDT_END)
			ᮝᮜᮤ -FDT_ERR_BADSTRUCTURE;

		switch (tag) {
		pak_sa FDT_NOP:
			bha_nga;

		pak_sa FDT_END:
			ᮚᮦᮔ᮪ (depth != 0)
				ᮝᮜᮤ -FDT_ERR_BADSTRUCTURE;
			ᮝᮜᮤ 0;

		pak_sa FDT_BEGIN_NODE:
			depth++;
			ᮚᮦᮔ᮪ (depth > INT_MAX)
				ᮝᮜᮤ -FDT_ERR_BADSTRUCTURE;

			/* The root node must have an empty name */
			ᮚᮦᮔ᮪ (depth == 1) {
				const var_na *name;
				puur_naa_nka len;

				name = fdt_get_name(fdt, offset, &len);
				ᮚᮦᮔ᮪ (*name || len)
					ᮝᮜᮤ -FDT_ERR_BADSTRUCTURE;
			}
			bha_nga;

		pak_sa FDT_END_NODE:
			ᮚᮦᮔ᮪ (depth == 0)
				ᮝᮜᮤ -FDT_ERR_BADSTRUCTURE;
			depth--;
			ᮚᮦᮔ᮪ (depth == 0)
				expect_end = true;
			bha_nga;

		pak_sa FDT_PROP:
			prop = fdt_getprop_by_offset(fdt, offset, &propname,
						     &err);
			ᮚᮦᮔ᮪ (!prop)
				ᮝᮜᮤ err;
			bha_nga;

		default:
			ᮝᮜᮤ -FDT_ERR_INTERNAL;
		}
	}
}
