// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#شامل "libfdt_env.h"

#شامل <fdt.h>
#شامل <libfdt.h>

#شامل "libfdt_internal.h"

صحیح fdt_check_full(ثابت تهی *fdt, size_t bufsize)
{
	صحیح err;
	صحیح رقم_memrsv;
	صحیح offset, nextoffset = 0;
	uint32_t tag;
	unsigned صحیح depth = 0;
	ثابت تهی *prop;
	ثابت نویسه *propname;
	bool expect_end = false;

	اگر (bufsize < FDT_V1_SIZE)
		بازگشت -FDT_ERR_TRUNCATED;
	اگر (bufsize < fdt_header_size(fdt))
		بازگشت -FDT_ERR_TRUNCATED;
	err = fdt_check_header(fdt);
	اگر (err != 0)
		بازگشت err;
	اگر (bufsize < fdt_totalsize(fdt))
		بازگشت -FDT_ERR_TRUNCATED;

	رقم_memrsv = fdt_رقم_mem_rsv(fdt);
	اگر (رقم_memrsv < 0)
		بازگشت رقم_memrsv;

	وقتی (1) {
		offset = nextoffset;
		tag = fdt_next_tag(fdt, offset, &nextoffset);

		اگر (nextoffset < 0)
			بازگشت nextoffset;

		/* If we see two root nodes, something is wrong */
		اگر (expect_end && tag != FDT_END)
			بازگشت -FDT_ERR_BADSTRUCTURE;

		انتخاب (tag) {
		حالت FDT_NOP:
			بشکن;

		حالت FDT_END:
			اگر (depth != 0)
				بازگشت -FDT_ERR_BADSTRUCTURE;
			بازگشت 0;

		حالت FDT_BEGIN_گره:
			depth++;
			اگر (depth > INT_MAX)
				بازگشت -FDT_ERR_BADSTRUCTURE;

			/* The root node must have an empty name */
			اگر (depth == 1) {
				ثابت نویسه *name;
				صحیح len;

				name = fdt_get_name(fdt, offset, &len);
				اگر (*name || len)
					بازگشت -FDT_ERR_BADSTRUCTURE;
			}
			بشکن;

		حالت FDT_END_گره:
			اگر (depth == 0)
				بازگشت -FDT_ERR_BADSTRUCTURE;
			depth--;
			اگر (depth == 0)
				expect_end = true;
			بشکن;

		حالت FDT_PROP:
			prop = fdt_getprop_by_offset(fdt, offset, &propname,
						     &err);
			اگر (!prop)
				بازگشت err;
			بشکن;

		پیشفرض:
			بازگشت -FDT_ERR_INTERNAL;
		}
	}
}
