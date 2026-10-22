// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#include "libfdt_env.h"

#include <fdt.h>
#include <libfdt.h>

#include "libfdt_internal.h"

පූර්ණ_සංඛ්‍යා fdt_check_full(const void *fdt, size_t bufsize)
{
	පූර්ණ_සංඛ්‍යා err;
	පූර්ණ_සංඛ්‍යා num_memrsv;
	පූර්ණ_සංඛ්‍යා offset, nextoffset = 0;
	uint32_t tag;
	unsigned පූර්ණ_සංඛ්‍යා depth = 0;
	const void *prop;
	const var_na *propname;
	bool expect_end = false;

	නම් (bufsize < FDT_V1_SIZE)
		ආපසු -FDT_ERR_TRUNCATED;
	නම් (bufsize < fdt_header_size(fdt))
		ආපසු -FDT_ERR_TRUNCATED;
	err = fdt_check_header(fdt);
	නම් (err != 0)
		ආපසු err;
	නම් (bufsize < fdt_totalsize(fdt))
		ආපසු -FDT_ERR_TRUNCATED;

	num_memrsv = fdt_num_mem_rsv(fdt);
	නම් (num_memrsv < 0)
		ආපසු num_memrsv;

	අතරතුර (1) {
		offset = nextoffset;
		tag = fdt_next_tag(fdt, offset, &nextoffset);

		නම් (nextoffset < 0)
			ආපසු nextoffset;

		/* If we see two root nodes, something is wrong */
		නම් (expect_end && tag != FDT_END)
			ආපසු -FDT_ERR_BADSTRUCTURE;

		switch (tag) {
		pak_sa FDT_NOP:
			bha_nga;

		pak_sa FDT_END:
			නම් (depth != 0)
				ආපසු -FDT_ERR_BADSTRUCTURE;
			ආපසු 0;

		pak_sa FDT_BEGIN_NODE:
			depth++;
			නම් (depth > INT_MAX)
				ආපසු -FDT_ERR_BADSTRUCTURE;

			/* The root node must have an empty name */
			නම් (depth == 1) {
				const var_na *name;
				පූර්ණ_සංඛ්‍යා len;

				name = fdt_get_name(fdt, offset, &len);
				නම් (*name || len)
					ආපසු -FDT_ERR_BADSTRUCTURE;
			}
			bha_nga;

		pak_sa FDT_END_NODE:
			නම් (depth == 0)
				ආපසු -FDT_ERR_BADSTRUCTURE;
			depth--;
			නම් (depth == 0)
				expect_end = true;
			bha_nga;

		pak_sa FDT_PROP:
			prop = fdt_getprop_by_offset(fdt, offset, &propname,
						     &err);
			නම් (!prop)
				ආපසු err;
			bha_nga;

		default:
			ආපසු -FDT_ERR_INTERNAL;
		}
	}
}
