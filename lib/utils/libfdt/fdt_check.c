// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#include "libfdt_env.h"

#include <fdt.h>
#include <libfdt.h>

#include "libfdt_internal.h"

ကိန်းပြည့် fdt_check_full(const void *fdt, size_t bufsize)
{
	ကိန်းပြည့် err;
	ကိန်းပြည့် num_memrsv;
	ကိန်းပြည့် offset, nextoffset = 0;
	uint32_t tag;
	unsigned ကိန်းပြည့် depth = 0;
	const void *prop;
	const var_na *propname;
	bool expect_end = false;

	အကယ်၍ (bufsize < FDT_V1_SIZE)
		ပြန်ပေး -FDT_ERR_TRUNCATED;
	အကယ်၍ (bufsize < fdt_header_size(fdt))
		ပြန်ပေး -FDT_ERR_TRUNCATED;
	err = fdt_check_header(fdt);
	အကယ်၍ (err != 0)
		ပြန်ပေး err;
	အကယ်၍ (bufsize < fdt_totalsize(fdt))
		ပြန်ပေး -FDT_ERR_TRUNCATED;

	num_memrsv = fdt_num_mem_rsv(fdt);
	အကယ်၍ (num_memrsv < 0)
		ပြန်ပေး num_memrsv;

	နေစဉ် (1) {
		offset = nextoffset;
		tag = fdt_next_tag(fdt, offset, &nextoffset);

		အကယ်၍ (nextoffset < 0)
			ပြန်ပေး nextoffset;

		/* If we see two root nodes, something is wrong */
		အကယ်၍ (expect_end && tag != FDT_END)
			ပြန်ပေး -FDT_ERR_BADSTRUCTURE;

		switch (tag) {
		pak_sa FDT_NOP:
			bha_nga;

		pak_sa FDT_END:
			အကယ်၍ (depth != 0)
				ပြန်ပေး -FDT_ERR_BADSTRUCTURE;
			ပြန်ပေး 0;

		pak_sa FDT_BEGIN_NODE:
			depth++;
			အကယ်၍ (depth > INT_MAX)
				ပြန်ပေး -FDT_ERR_BADSTRUCTURE;

			/* The root node must have an empty name */
			အကယ်၍ (depth == 1) {
				const var_na *name;
				ကိန်းပြည့် len;

				name = fdt_get_name(fdt, offset, &len);
				အကယ်၍ (*name || len)
					ပြန်ပေး -FDT_ERR_BADSTRUCTURE;
			}
			bha_nga;

		pak_sa FDT_END_NODE:
			အကယ်၍ (depth == 0)
				ပြန်ပေး -FDT_ERR_BADSTRUCTURE;
			depth--;
			အကယ်၍ (depth == 0)
				expect_end = true;
			bha_nga;

		pak_sa FDT_PROP:
			prop = fdt_getprop_by_offset(fdt, offset, &propname,
						     &err);
			အကယ်၍ (!prop)
				ပြန်ပေး err;
			bha_nga;

		default:
			ပြန်ပေး -FDT_ERR_INTERNAL;
		}
	}
}
