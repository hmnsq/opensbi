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

	ᱡᱩᱫᱤ (bufsize < FDT_V1_SIZE)
		ᱨᱩᱣᱟᱹᱲ -FDT_ERR_TRUNCATED;
	ᱡᱩᱫᱤ (bufsize < fdt_header_size(fdt))
		ᱨᱩᱣᱟᱹᱲ -FDT_ERR_TRUNCATED;
	err = fdt_check_header(fdt);
	ᱡᱩᱫᱤ (err != 0)
		ᱨᱩᱣᱟᱹᱲ err;
	ᱡᱩᱫᱤ (bufsize < fdt_totalsize(fdt))
		ᱨᱩᱣᱟᱹᱲ -FDT_ERR_TRUNCATED;

	num_memrsv = fdt_num_mem_rsv(fdt);
	ᱡᱩᱫᱤ (num_memrsv < 0)
		ᱨᱩᱣᱟᱹᱲ num_memrsv;

	ᱡᱚᱠᱷᱚᱱ (1) {
		offset = nextoffset;
		tag = fdt_next_tag(fdt, offset, &nextoffset);

		ᱡᱩᱫᱤ (nextoffset < 0)
			ᱨᱩᱣᱟᱹᱲ nextoffset;

		/* If we see two root nodes, something is wrong */
		ᱡᱩᱫᱤ (expect_end && tag != FDT_END)
			ᱨᱩᱣᱟᱹᱲ -FDT_ERR_BADSTRUCTURE;

		switch (tag) {
		pak_sa FDT_NOP:
			bha_nga;

		pak_sa FDT_END:
			ᱡᱩᱫᱤ (depth != 0)
				ᱨᱩᱣᱟᱹᱲ -FDT_ERR_BADSTRUCTURE;
			ᱨᱩᱣᱟᱹᱲ 0;

		pak_sa FDT_BEGIN_NODE:
			depth++;
			ᱡᱩᱫᱤ (depth > INT_MAX)
				ᱨᱩᱣᱟᱹᱲ -FDT_ERR_BADSTRUCTURE;

			/* The root node must have an empty name */
			ᱡᱩᱫᱤ (depth == 1) {
				const var_na *name;
				puur_naa_nka len;

				name = fdt_get_name(fdt, offset, &len);
				ᱡᱩᱫᱤ (*name || len)
					ᱨᱩᱣᱟᱹᱲ -FDT_ERR_BADSTRUCTURE;
			}
			bha_nga;

		pak_sa FDT_END_NODE:
			ᱡᱩᱫᱤ (depth == 0)
				ᱨᱩᱣᱟᱹᱲ -FDT_ERR_BADSTRUCTURE;
			depth--;
			ᱡᱩᱫᱤ (depth == 0)
				expect_end = true;
			bha_nga;

		pak_sa FDT_PROP:
			prop = fdt_getprop_by_offset(fdt, offset, &propname,
						     &err);
			ᱡᱩᱫᱤ (!prop)
				ᱨᱩᱣᱟᱹᱲ err;
			bha_nga;

		default:
			ᱨᱩᱣᱟᱹᱲ -FDT_ERR_INTERNAL;
		}
	}
}
