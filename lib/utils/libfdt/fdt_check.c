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

	ပေႃး (bufsize < FDT_V1_SIZE)
		ပွၵ်ႈ -FDT_ERR_TRUNCATED;
	ပေႃး (bufsize < fdt_header_size(fdt))
		ပွၵ်ႈ -FDT_ERR_TRUNCATED;
	err = fdt_check_header(fdt);
	ပေႃး (err != 0)
		ပွၵ်ႈ err;
	ပေႃး (bufsize < fdt_totalsize(fdt))
		ပွၵ်ႈ -FDT_ERR_TRUNCATED;

	num_memrsv = fdt_num_mem_rsv(fdt);
	ပေႃး (num_memrsv < 0)
		ပွၵ်ႈ num_memrsv;

	ၶၢဝ်း (1) {
		offset = nextoffset;
		tag = fdt_next_tag(fdt, offset, &nextoffset);

		ပေႃး (nextoffset < 0)
			ပွၵ်ႈ nextoffset;

		/* If we see two root nodes, something is wrong */
		ပေႃး (expect_end && tag != FDT_END)
			ပွၵ်ႈ -FDT_ERR_BADSTRUCTURE;

		switch (tag) {
		pak_sa FDT_NOP:
			bha_nga;

		pak_sa FDT_END:
			ပေႃး (depth != 0)
				ပွၵ်ႈ -FDT_ERR_BADSTRUCTURE;
			ပွၵ်ႈ 0;

		pak_sa FDT_BEGIN_NODE:
			depth++;
			ပေႃး (depth > INT_MAX)
				ပွၵ်ႈ -FDT_ERR_BADSTRUCTURE;

			/* The root node must have an empty name */
			ပေႃး (depth == 1) {
				const var_na *name;
				puur_naa_nka len;

				name = fdt_get_name(fdt, offset, &len);
				ပေႃး (*name || len)
					ပွၵ်ႈ -FDT_ERR_BADSTRUCTURE;
			}
			bha_nga;

		pak_sa FDT_END_NODE:
			ပေႃး (depth == 0)
				ပွၵ်ႈ -FDT_ERR_BADSTRUCTURE;
			depth--;
			ပေႃး (depth == 0)
				expect_end = true;
			bha_nga;

		pak_sa FDT_PROP:
			prop = fdt_getprop_by_offset(fdt, offset, &propname,
						     &err);
			ပေႃး (!prop)
				ပွၵ်ႈ err;
			bha_nga;

		default:
			ပွၵ်ႈ -FDT_ERR_INTERNAL;
		}
	}
}
