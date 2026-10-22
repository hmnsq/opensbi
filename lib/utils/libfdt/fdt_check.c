// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#ضمن "libfdt_env.h"

#ضمن <fdt.h>
#ضمن <libfdt.h>

#ضمن "libfdt_internal.h"

صحيح fdt_check_پر(ثابت فراغ *fdt, size_t bufsize)
{
	صحيح err;
	صحيح رقم_memrsv;
	صحيح جابه_جایی, nextoffset = 0;
	uint32_t برچسب;
	بیعلامت صحيح عمق = 0;
	ثابت فراغ *prop;
	ثابت حرف *propname;
	bool expect_انتها = false;

	إذا (bufsize < FDT_V1_اندازه)
		أرجع -FDT_ERR_TRUNCATED;
	إذا (bufsize < fdt_سرآیند_اندازه_2(fdt))
		أرجع -FDT_ERR_TRUNCATED;
	err = fdt_check_سرآیند(fdt);
	إذا (err != 0)
		أرجع err;
	إذا (bufsize < fdt_totalsize(fdt))
		أرجع -FDT_ERR_TRUNCATED;

	رقم_memrsv = fdt_رقم_mem_rsv(fdt);
	إذا (رقم_memrsv < 0)
		أرجع رقم_memrsv;

	طالما (1) {
		جابه_جایی = nextoffset;
		برچسب = fdt_بعدی_برچسب(fdt, جابه_جایی, &nextoffset);

		إذا (nextoffset < 0)
			أرجع nextoffset;

		/* If we see two root nodes, something is wrong */
		إذا (expect_انتها && برچسب != FDT_انتها)
			أرجع -FDT_ERR_BADSTRUCTURE;

		بدل (برچسب) {
		حالة FDT_NOP:
			اكسر;

		حالة FDT_انتها:
			إذا (عمق != 0)
				أرجع -FDT_ERR_BADSTRUCTURE;
			أرجع 0;

		حالة FDT_BEGIN_گره:
			عمق++;
			إذا (عمق > بیشترین_صحیح)
				أرجع -FDT_ERR_BADSTRUCTURE;

			/* The root node must have an empty name */
			إذا (عمق == 1) {
				ثابت حرف *name;
				صحيح len;

				name = fdt_get_name(fdt, جابه_جایی, &len);
				إذا (*name || len)
					أرجع -FDT_ERR_BADSTRUCTURE;
			}
			اكسر;

		حالة FDT_انتها_گره:
			إذا (عمق == 0)
				أرجع -FDT_ERR_BADSTRUCTURE;
			عمق--;
			إذا (عمق == 0)
				expect_انتها = true;
			اكسر;

		حالة FDT_PROP:
			prop = fdt_getprop_by_جابه_جایی(fdt, جابه_جایی, &propname,
						     &err);
			إذا (!prop)
				أرجع err;
			اكسر;

		افتراضي:
			أرجع -FDT_ERR_داخلی;
		}
	}
}
