// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#ضمن "libfdt_env.h"

#ضمن <fdt.h>
#ضمن <libfdt.h>

#ضمن "libfdt_internal.h"

صحيح fdt_check_ډک(ثابت فراغ *fdt, size_t bufsize)
{
	صحيح err;
	صحيح عدد_memrsv;
	صحيح دننه_واټن, nextoffset = 0;
	uint32_t نښان;
	بینښه صحيح ژوروالی = 0;
	ثابت فراغ *prop;
	ثابت حرف *propname;
	bool expect_پای_ټکی = false;

	إذا (bufsize < FDT_V1_اندازه)
		أرجع -FDT_ERR_TRUNCATED;
	إذا (bufsize < fdt_سرلیک_اندازه_2(fdt))
		أرجع -FDT_ERR_TRUNCATED;
	err = fdt_check_سرلیک(fdt);
	إذا (err != 0)
		أرجع err;
	إذا (bufsize < fdt_totalsize(fdt))
		أرجع -FDT_ERR_TRUNCATED;

	عدد_memrsv = fdt_عدد_mem_rsv(fdt);
	إذا (عدد_memrsv < 0)
		أرجع عدد_memrsv;

	طالما (1) {
		دننه_واټن = nextoffset;
		نښان = fdt_بل_نښان(fdt, دننه_واټن, &nextoffset);

		إذا (nextoffset < 0)
			أرجع nextoffset;

		/* If we see two root nodes, something is wrong */
		إذا (expect_پای_ټکی && نښان != FDT_پای_ټکی)
			أرجع -FDT_ERR_BADSTRUCTURE;

		بدل (نښان) {
		حالة FDT_NOP:
			اكسر;

		حالة FDT_پای_ټکی:
			إذا (ژوروالی != 0)
				أرجع -FDT_ERR_BADSTRUCTURE;
			أرجع 0;

		حالة FDT_BEGIN_غوټه:
			ژوروالی++;
			إذا (ژوروالی > لوی_عدد)
				أرجع -FDT_ERR_BADSTRUCTURE;

			/* The root node must have an empty name */
			إذا (ژوروالی == 1) {
				ثابت حرف *name;
				صحيح len;

				name = fdt_get_name(fdt, دننه_واټن, &len);
				إذا (*name || len)
					أرجع -FDT_ERR_BADSTRUCTURE;
			}
			اكسر;

		حالة FDT_پای_ټکی_غوټه:
			إذا (ژوروالی == 0)
				أرجع -FDT_ERR_BADSTRUCTURE;
			ژوروالی--;
			إذا (ژوروالی == 0)
				expect_پای_ټکی = true;
			اكسر;

		حالة FDT_PROP:
			prop = fdt_getprop_by_دننه_واټن(fdt, دننه_واټن, &propname,
						     &err);
			إذا (!prop)
				أرجع err;
			اكسر;

		افتراضي:
			أرجع -FDT_ERR_دننی;
		}
	}
}
