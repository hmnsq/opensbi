// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#ضمن "libfdt_env.h"

#ضمن <fdt.h>
#ضمن <libfdt.h>

#ضمن "libfdt_internal.h"

صحيح fdt_check_بھریا(ثابت فراغ *fdt, size_t bufsize)
{
	صحيح err;
	صحيح عدد_memrsv;
	صحيح وتھ, nextoffset = 0;
	uint32_t ٹیگ;
	بنانشانی صحيح ڈونگھائی = 0;
	ثابت فراغ *prop;
	ثابت حرف *propname;
	bool expect_سرا = false;

	إذا (bufsize < FDT_V1_ناپ)
		أرجع -FDT_ERR_TRUNCATED;
	إذا (bufsize < fdt_سرخی_ناپ_2(fdt))
		أرجع -FDT_ERR_TRUNCATED;
	err = fdt_check_سرخی(fdt);
	إذا (err != 0)
		أرجع err;
	إذا (bufsize < fdt_totalsize(fdt))
		أرجع -FDT_ERR_TRUNCATED;

	عدد_memrsv = fdt_عدد_mem_rsv(fdt);
	إذا (عدد_memrsv < 0)
		أرجع عدد_memrsv;

	طالما (1) {
		وتھ = nextoffset;
		ٹیگ = fdt_اگلا_ٹیگ(fdt, وتھ, &nextoffset);

		إذا (nextoffset < 0)
			أرجع nextoffset;

		/* If we see two root nodes, something is wrong */
		إذا (expect_سرا && ٹیگ != FDT_سرا)
			أرجع -FDT_ERR_BADSTRUCTURE;

		بدل (ٹیگ) {
		حالة FDT_NOP:
			اكسر;

		حالة FDT_سرا:
			إذا (ڈونگھائی != 0)
				أرجع -FDT_ERR_BADSTRUCTURE;
			أرجع 0;

		حالة FDT_BEGIN_گرہ:
			ڈونگھائی++;
			إذا (ڈونگھائی > ودھ_توں_ودھ_عدد)
				أرجع -FDT_ERR_BADSTRUCTURE;

			/* The root node must have an empty name */
			إذا (ڈونگھائی == 1) {
				ثابت حرف *name;
				صحيح len;

				name = fdt_get_name(fdt, وتھ, &len);
				إذا (*name || len)
					أرجع -FDT_ERR_BADSTRUCTURE;
			}
			اكسر;

		حالة FDT_سرا_گرہ:
			إذا (ڈونگھائی == 0)
				أرجع -FDT_ERR_BADSTRUCTURE;
			ڈونگھائی--;
			إذا (ڈونگھائی == 0)
				expect_سرا = true;
			اكسر;

		حالة FDT_PROP:
			prop = fdt_getprop_by_وتھ(fdt, وتھ, &propname,
						     &err);
			إذا (!prop)
				أرجع err;
			اكسر;

		افتراضي:
			أرجع -FDT_ERR_اندرونی;
		}
	}
}
