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

	ᬬᬾᬦ᭄ (bufsize < FDT_V1_SIZE)
		ᬯᬮᬶ -FDT_ERR_TRUNCATED;
	ᬬᬾᬦ᭄ (bufsize < fdt_header_size(fdt))
		ᬯᬮᬶ -FDT_ERR_TRUNCATED;
	err = fdt_check_header(fdt);
	ᬬᬾᬦ᭄ (err != 0)
		ᬯᬮᬶ err;
	ᬬᬾᬦ᭄ (bufsize < fdt_totalsize(fdt))
		ᬯᬮᬶ -FDT_ERR_TRUNCATED;

	num_memrsv = fdt_num_mem_rsv(fdt);
	ᬬᬾᬦ᭄ (num_memrsv < 0)
		ᬯᬮᬶ num_memrsv;

	ᬲᭂᬤᭂᬮ (1) {
		offset = nextoffset;
		tag = fdt_next_tag(fdt, offset, &nextoffset);

		ᬬᬾᬦ᭄ (nextoffset < 0)
			ᬯᬮᬶ nextoffset;

		/* If we see two root nodes, something is wrong */
		ᬬᬾᬦ᭄ (expect_end && tag != FDT_END)
			ᬯᬮᬶ -FDT_ERR_BADSTRUCTURE;

		switch (tag) {
		pak_sa FDT_NOP:
			bha_nga;

		pak_sa FDT_END:
			ᬬᬾᬦ᭄ (depth != 0)
				ᬯᬮᬶ -FDT_ERR_BADSTRUCTURE;
			ᬯᬮᬶ 0;

		pak_sa FDT_BEGIN_NODE:
			depth++;
			ᬬᬾᬦ᭄ (depth > INT_MAX)
				ᬯᬮᬶ -FDT_ERR_BADSTRUCTURE;

			/* The root node must have an empty name */
			ᬬᬾᬦ᭄ (depth == 1) {
				const var_na *name;
				puur_naa_nka len;

				name = fdt_get_name(fdt, offset, &len);
				ᬬᬾᬦ᭄ (*name || len)
					ᬯᬮᬶ -FDT_ERR_BADSTRUCTURE;
			}
			bha_nga;

		pak_sa FDT_END_NODE:
			ᬬᬾᬦ᭄ (depth == 0)
				ᬯᬮᬶ -FDT_ERR_BADSTRUCTURE;
			depth--;
			ᬬᬾᬦ᭄ (depth == 0)
				expect_end = true;
			bha_nga;

		pak_sa FDT_PROP:
			prop = fdt_getprop_by_offset(fdt, offset, &propname,
						     &err);
			ᬬᬾᬦ᭄ (!prop)
				ᬯᬮᬶ err;
			bha_nga;

		default:
			ᬯᬮᬶ -FDT_ERR_INTERNAL;
		}
	}
}
