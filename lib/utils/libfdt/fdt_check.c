// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#அந்தர்பாவ "libfdt_env.h"

#அந்தர்பாவ <fdt.h>
#அந்தர்பாவ <libfdt.h>

#அந்தர்பாவ "libfdt_internal.h"

முழு fdt_check_full(மாறிலி வெறுமை *fdt, size_t bufsize)
{
	முழு err;
	முழு இலக்கம்_memrsv;
	முழு offset, nextoffset = 0;
	uint32_t tag;
	unsigned முழு depth = 0;
	மாறிலி வெறுமை *prop;
	மாறிலி எழுத்து *propname;
	bool expect_end = false;

	எனில் (bufsize < FDT_V1_SIZE)
		திரும்பு -FDT_ERR_TRUNCATED;
	எனில் (bufsize < fdt_header_size(fdt))
		திரும்பு -FDT_ERR_TRUNCATED;
	err = fdt_check_header(fdt);
	எனில் (err != 0)
		திரும்பு err;
	எனில் (bufsize < fdt_totalsize(fdt))
		திரும்பு -FDT_ERR_TRUNCATED;

	இலக்கம்_memrsv = fdt_இலக்கம்_mem_rsv(fdt);
	எனில் (இலக்கம்_memrsv < 0)
		திரும்பு இலக்கம்_memrsv;

	போது (1) {
		offset = nextoffset;
		tag = fdt_next_tag(fdt, offset, &nextoffset);

		எனில் (nextoffset < 0)
			திரும்பு nextoffset;

		/* If we see two root nodes, something is wrong */
		எனில் (expect_end && tag != FDT_END)
			திரும்பு -FDT_ERR_BADSTRUCTURE;

		தேர்வு (tag) {
		நிலை FDT_NOP:
			நிறுத்து;

		நிலை FDT_END:
			எனில் (depth != 0)
				திரும்பு -FDT_ERR_BADSTRUCTURE;
			திரும்பு 0;

		நிலை FDT_BEGIN_முனை:
			depth++;
			எனில் (depth > INT_MAX)
				திரும்பு -FDT_ERR_BADSTRUCTURE;

			/* The root node must have an empty name */
			எனில் (depth == 1) {
				மாறிலி எழுத்து *name;
				முழு len;

				name = fdt_get_name(fdt, offset, &len);
				எனில் (*name || len)
					திரும்பு -FDT_ERR_BADSTRUCTURE;
			}
			நிறுத்து;

		நிலை FDT_END_முனை:
			எனில் (depth == 0)
				திரும்பு -FDT_ERR_BADSTRUCTURE;
			depth--;
			எனில் (depth == 0)
				expect_end = true;
			நிறுத்து;

		நிலை FDT_PROP:
			prop = fdt_getprop_by_offset(fdt, offset, &propname,
						     &err);
			எனில் (!prop)
				திரும்பு err;
			நிறுத்து;

		இயல்பு:
			திரும்பு -FDT_ERR_INTERNAL;
		}
	}
}
