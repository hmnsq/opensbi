// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#ضمن "libfdt_env.h"

#ضمن <fdt.h>
#ضمن <libfdt.h>

#ضمن "libfdt_internal.h"

صحيح fdt_check_full(ثابت فراغ *fdt, size_t bufsize)
{
	صحيح err;
	صحيح رقم_memrsv;
	صحيح offset, nextoffset = 0;
	uint32_t tag;
	unsigned صحيح depth = 0;
	ثابت فراغ *prop;
	ثابت حرف *propname;
	bool expect_end = false;

	إذا (bufsize < FDT_V1_SIZE)
		إرجاع -FDT_ERR_TRUNCATED;
	إذا (bufsize < fdt_header_size(fdt))
		إرجاع -FDT_ERR_TRUNCATED;
	err = fdt_check_header(fdt);
	إذا (err != 0)
		إرجاع err;
	إذا (bufsize < fdt_totalsize(fdt))
		إرجاع -FDT_ERR_TRUNCATED;

	رقم_memrsv = fdt_رقم_mem_rsv(fdt);
	إذا (رقم_memrsv < 0)
		إرجاع رقم_memrsv;

	بينما (1) {
		offset = nextoffset;
		tag = fdt_next_tag(fdt, offset, &nextoffset);

		إذا (nextoffset < 0)
			إرجاع nextoffset;

		/* If we see two root nodes, something is wrong */
		إذا (expect_end && tag != FDT_END)
			إرجاع -FDT_ERR_BADSTRUCTURE;

		بدل (tag) {
		حالة FDT_NOP:
			اكسر;

		حالة FDT_END:
			إذا (depth != 0)
				إرجاع -FDT_ERR_BADSTRUCTURE;
			إرجاع 0;

		حالة FDT_BEGIN_عقدة:
			depth++;
			إذا (depth > INT_MAX)
				إرجاع -FDT_ERR_BADSTRUCTURE;

			/* The root node must have an empty name */
			إذا (depth == 1) {
				ثابت حرف *name;
				صحيح len;

				name = fdt_get_name(fdt, offset, &len);
				إذا (*name || len)
					إرجاع -FDT_ERR_BADSTRUCTURE;
			}
			اكسر;

		حالة FDT_END_عقدة:
			إذا (depth == 0)
				إرجاع -FDT_ERR_BADSTRUCTURE;
			depth--;
			إذا (depth == 0)
				expect_end = true;
			اكسر;

		حالة FDT_PROP:
			prop = fdt_getprop_by_offset(fdt, offset, &propname,
						     &err);
			إذا (!prop)
				إرجاع err;
			اكسر;

		افتراضي:
			إرجاع -FDT_ERR_INTERNAL;
		}
	}
}
