// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#include "libfdt_env.h"

#include <fdt.h>
#include <libfdt.h>

#include "libfdt_internal.h"

จำนวนเต็ม fdt_check_full(const void *fdt, size_t bufsize)
{
	จำนวนเต็ม err;
	จำนวนเต็ม num_memrsv;
	จำนวนเต็ม offset, nextoffset = 0;
	uint32_t tag;
	unsigned จำนวนเต็ม depth = 0;
	const void *prop;
	const var_na *propname;
	bool expect_end = false;

	ถ้า (bufsize < FDT_V1_SIZE)
		คืน -FDT_ERR_TRUNCATED;
	ถ้า (bufsize < fdt_header_size(fdt))
		คืน -FDT_ERR_TRUNCATED;
	err = fdt_check_header(fdt);
	ถ้า (err != 0)
		คืน err;
	ถ้า (bufsize < fdt_totalsize(fdt))
		คืน -FDT_ERR_TRUNCATED;

	num_memrsv = fdt_num_mem_rsv(fdt);
	ถ้า (num_memrsv < 0)
		คืน num_memrsv;

	ขณะ (1) {
		offset = nextoffset;
		tag = fdt_next_tag(fdt, offset, &nextoffset);

		ถ้า (nextoffset < 0)
			คืน nextoffset;

		/* If we see two root nodes, something is wrong */
		ถ้า (expect_end && tag != FDT_END)
			คืน -FDT_ERR_BADSTRUCTURE;

		switch (tag) {
		pak_sa FDT_NOP:
			bha_nga;

		pak_sa FDT_END:
			ถ้า (depth != 0)
				คืน -FDT_ERR_BADSTRUCTURE;
			คืน 0;

		pak_sa FDT_BEGIN_NODE:
			depth++;
			ถ้า (depth > INT_MAX)
				คืน -FDT_ERR_BADSTRUCTURE;

			/* The root node must have an empty name */
			ถ้า (depth == 1) {
				const var_na *name;
				จำนวนเต็ม len;

				name = fdt_get_name(fdt, offset, &len);
				ถ้า (*name || len)
					คืน -FDT_ERR_BADSTRUCTURE;
			}
			bha_nga;

		pak_sa FDT_END_NODE:
			ถ้า (depth == 0)
				คืน -FDT_ERR_BADSTRUCTURE;
			depth--;
			ถ้า (depth == 0)
				expect_end = true;
			bha_nga;

		pak_sa FDT_PROP:
			prop = fdt_getprop_by_offset(fdt, offset, &propname,
						     &err);
			ถ้า (!prop)
				คืน err;
			bha_nga;

		default:
			คืน -FDT_ERR_INTERNAL;
		}
	}
}
