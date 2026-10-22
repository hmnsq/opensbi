// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#شامل "libfdt_env.h"

#شامل <fdt.h>
#شامل <libfdt.h>

#شامل "libfdt_internal.h"

صحیح fdt_check_بھرا(ثابت خالی *fdt, size_t bufsize)
{
	صحیح err;
	صحیح عدد_memrsv;
	صحیح انحراف, nextoffset = 0;
	uint32_t لیبل;
	غیرعلامتی صحیح گہرائی = 0;
	ثابت خالی *prop;
	ثابت حرف *propname;
	bool expect_انتہا = false;

	اگر (bufsize < FDT_V1_سائز)
		واپس -FDT_ERR_TRUNCATED;
	اگر (bufsize < fdt_سرخی_سائز_2(fdt))
		واپس -FDT_ERR_TRUNCATED;
	err = fdt_check_سرخی(fdt);
	اگر (err != 0)
		واپس err;
	اگر (bufsize < fdt_totalsize(fdt))
		واپس -FDT_ERR_TRUNCATED;

	عدد_memrsv = fdt_عدد_mem_rsv(fdt);
	اگر (عدد_memrsv < 0)
		واپس عدد_memrsv;

	جب (1) {
		انحراف = nextoffset;
		لیبل = fdt_اگلا_لیبل(fdt, انحراف, &nextoffset);

		اگر (nextoffset < 0)
			واپس nextoffset;

		/* If we see two root nodes, something is wrong */
		اگر (expect_انتہا && لیبل != FDT_انتہا)
			واپس -FDT_ERR_BADSTRUCTURE;

		انتخاب (لیبل) {
		حالت FDT_NOP:
			توڑو;

		حالت FDT_انتہا:
			اگر (گہرائی != 0)
				واپس -FDT_ERR_BADSTRUCTURE;
			واپس 0;

		حالت FDT_BEGIN_گرہ:
			گہرائی++;
			اگر (گہرائی > زیادہ_سے_زیادہ_عدد)
				واپس -FDT_ERR_BADSTRUCTURE;

			/* The root node must have an empty name */
			اگر (گہرائی == 1) {
				ثابت حرف *name;
				صحیح len;

				name = fdt_get_name(fdt, انحراف, &len);
				اگر (*name || len)
					واپس -FDT_ERR_BADSTRUCTURE;
			}
			توڑو;

		حالت FDT_انتہا_گرہ:
			اگر (گہرائی == 0)
				واپس -FDT_ERR_BADSTRUCTURE;
			گہرائی--;
			اگر (گہرائی == 0)
				expect_انتہا = true;
			توڑو;

		حالت FDT_PROP:
			prop = fdt_getprop_by_انحراف(fdt, انحراف, &propname,
						     &err);
			اگر (!prop)
				واپس err;
			توڑو;

		طےشدہ:
			واپس -FDT_ERR_اندرونی;
		}
	}
}
