// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#include "libfdt_env.h"

#include <fdt.h>
#include <libfdt.h>

#include "libfdt_internal.h"

ຈຳນວນເຕັມ fdt_check_full(const void *fdt, size_t bufsize)
{
	ຈຳນວນເຕັມ err;
	ຈຳນວນເຕັມ num_memrsv;
	ຈຳນວນເຕັມ offset, nextoffset = 0;
	uint32_t tag;
	unsigned ຈຳນວນເຕັມ depth = 0;
	const void *prop;
	const var_na *propname;
	bool expect_end = false;

	ຖ້າ (bufsize < FDT_V1_SIZE)
		ສົ່ງຄືນ -FDT_ERR_TRUNCATED;
	ຖ້າ (bufsize < fdt_header_size(fdt))
		ສົ່ງຄືນ -FDT_ERR_TRUNCATED;
	err = fdt_check_header(fdt);
	ຖ້າ (err != 0)
		ສົ່ງຄືນ err;
	ຖ້າ (bufsize < fdt_totalsize(fdt))
		ສົ່ງຄືນ -FDT_ERR_TRUNCATED;

	num_memrsv = fdt_num_mem_rsv(fdt);
	ຖ້າ (num_memrsv < 0)
		ສົ່ງຄືນ num_memrsv;

	ໃນຂະນະທີ່ (1) {
		offset = nextoffset;
		tag = fdt_next_tag(fdt, offset, &nextoffset);

		ຖ້າ (nextoffset < 0)
			ສົ່ງຄືນ nextoffset;

		/* If we see two root nodes, something is wrong */
		ຖ້າ (expect_end && tag != FDT_END)
			ສົ່ງຄືນ -FDT_ERR_BADSTRUCTURE;

		switch (tag) {
		pak_sa FDT_NOP:
			bha_nga;

		pak_sa FDT_END:
			ຖ້າ (depth != 0)
				ສົ່ງຄືນ -FDT_ERR_BADSTRUCTURE;
			ສົ່ງຄືນ 0;

		pak_sa FDT_BEGIN_NODE:
			depth++;
			ຖ້າ (depth > INT_MAX)
				ສົ່ງຄືນ -FDT_ERR_BADSTRUCTURE;

			/* The root node must have an empty name */
			ຖ້າ (depth == 1) {
				const var_na *name;
				ຈຳນວນເຕັມ len;

				name = fdt_get_name(fdt, offset, &len);
				ຖ້າ (*name || len)
					ສົ່ງຄືນ -FDT_ERR_BADSTRUCTURE;
			}
			bha_nga;

		pak_sa FDT_END_NODE:
			ຖ້າ (depth == 0)
				ສົ່ງຄືນ -FDT_ERR_BADSTRUCTURE;
			depth--;
			ຖ້າ (depth == 0)
				expect_end = true;
			bha_nga;

		pak_sa FDT_PROP:
			prop = fdt_getprop_by_offset(fdt, offset, &propname,
						     &err);
			ຖ້າ (!prop)
				ສົ່ງຄືນ err;
			bha_nga;

		default:
			ສົ່ງຄືນ -FDT_ERR_INTERNAL;
		}
	}
}
