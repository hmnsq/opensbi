// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#ضمن "libfdt_env.h"

#ضمن <fdt.h>
#ضمن <libfdt.h>

#ضمن "libfdt_internal.h"

صحيح fdt_check_ڀريل(ثابت فراغ *fdt, size_t bufsize)
{
	صحيح err;
	صحيح انگ_memrsv;
	صحيح وٿي, nextoffset = 0;
	uint32_t ٽيگ;
	بنانشان صحيح اونهائي = 0;
	ثابت فراغ *prop;
	ثابت حرف *propname;
	bool expect_پڇاڙي = false;

	إذا (bufsize < FDT_V1_سائيز)
		أرجع -FDT_ERR_TRUNCATED;
	إذا (bufsize < fdt_سرو_سائيز_2(fdt))
		أرجع -FDT_ERR_TRUNCATED;
	err = fdt_check_سرو(fdt);
	إذا (err != 0)
		أرجع err;
	إذا (bufsize < fdt_totalsize(fdt))
		أرجع -FDT_ERR_TRUNCATED;

	انگ_memrsv = fdt_انگ_mem_rsv(fdt);
	إذا (انگ_memrsv < 0)
		أرجع انگ_memrsv;

	طالما (1) {
		وٿي = nextoffset;
		ٽيگ = fdt_ايندڙ_ٽيگ(fdt, وٿي, &nextoffset);

		إذا (nextoffset < 0)
			أرجع nextoffset;

		/* If we see two root nodes, something is wrong */
		إذا (expect_پڇاڙي && ٽيگ != FDT_پڇاڙي)
			أرجع -FDT_ERR_BADSTRUCTURE;

		بدل (ٽيگ) {
		حالة FDT_NOP:
			اكسر;

		حالة FDT_پڇاڙي:
			إذا (اونهائي != 0)
				أرجع -FDT_ERR_BADSTRUCTURE;
			أرجع 0;

		حالة FDT_BEGIN_ڳنڍ:
			اونهائي++;
			إذا (اونهائي > وڌ_کان_وڌ_عدد)
				أرجع -FDT_ERR_BADSTRUCTURE;

			/* The root node must have an empty name */
			إذا (اونهائي == 1) {
				ثابت حرف *name;
				صحيح len;

				name = fdt_get_name(fdt, وٿي, &len);
				إذا (*name || len)
					أرجع -FDT_ERR_BADSTRUCTURE;
			}
			اكسر;

		حالة FDT_پڇاڙي_ڳنڍ:
			إذا (اونهائي == 0)
				أرجع -FDT_ERR_BADSTRUCTURE;
			اونهائي--;
			إذا (اونهائي == 0)
				expect_پڇاڙي = true;
			اكسر;

		حالة FDT_PROP:
			prop = fdt_getprop_by_وٿي(fdt, وٿي, &propname,
						     &err);
			إذا (!prop)
				أرجع err;
			اكسر;

		افتراضي:
			أرجع -FDT_ERR_اندروني;
		}
	}
}
