// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#అన్తర్భావ "libfdt_env.h"

#అన్తర్భావ <fdt.h>
#అన్తర్భావ <libfdt.h>

#అన్తర్భావ "libfdt_internal.h"

పూర్ణాంకం fdt_check_నిండిన(స్థిరరాశి శూన్య *fdt, size_t bufsize)
{
	పూర్ణాంకం err;
	పూర్ణాంకం అంకె_memrsv;
	పూర్ణాంకం స్థానభ్రంశం, nextoffset = 0;
	uint32_t ట్యాగ్;
	గుర్తులేని పూర్ణాంకం లోతు = 0;
	స్థిరరాశి శూన్య *prop;
	స్థిరరాశి అక్షరం *propname;
	bool expect_చివర = false;

	ఒకవేళ (bufsize < FDT_V1_కొలత)
		తిరిగి -FDT_ERR_TRUNCATED;
	ఒకవేళ (bufsize < fdt_శీర్షిక_కొలత_2(fdt))
		తిరిగి -FDT_ERR_TRUNCATED;
	err = fdt_check_శీర్షిక(fdt);
	ఒకవేళ (err != 0)
		తిరిగి err;
	ఒకవేళ (bufsize < fdt_totalsize(fdt))
		తిరిగి -FDT_ERR_TRUNCATED;

	అంకె_memrsv = fdt_అంకె_mem_rsv(fdt);
	ఒకవేళ (అంకె_memrsv < 0)
		తిరిగి అంకె_memrsv;

	అయితే (1) {
		స్థానభ్రంశం = nextoffset;
		ట్యాగ్ = fdt_తదుపరి_ట్యాగ్(fdt, స్థానభ్రంశం, &nextoffset);

		ఒకవేళ (nextoffset < 0)
			తిరిగి nextoffset;

		/* If we see two root nodes, something is wrong */
		ఒకవేళ (expect_చివర && ట్యాగ్ != FDT_చివర)
			తిరిగి -FDT_ERR_BADSTRUCTURE;

		ఎంపిక (ట్యాగ్) {
		స్థితి FDT_NOP:
			ఆపు;

		స్థితి FDT_చివర:
			ఒకవేళ (లోతు != 0)
				తిరిగి -FDT_ERR_BADSTRUCTURE;
			తిరిగి 0;

		స్థితి FDT_BEGIN_కణుపు:
			లోతు++;
			ఒకవేళ (లోతు > గరిష్ఠ_పూర్ణాంకం)
				తిరిగి -FDT_ERR_BADSTRUCTURE;

			/* The root node must have an empty name */
			ఒకవేళ (లోతు == 1) {
				స్థిరరాశి అక్షరం *name;
				పూర్ణాంకం len;

				name = fdt_get_name(fdt, స్థానభ్రంశం, &len);
				ఒకవేళ (*name || len)
					తిరిగి -FDT_ERR_BADSTRUCTURE;
			}
			ఆపు;

		స్థితి FDT_చివర_కణుపు:
			ఒకవేళ (లోతు == 0)
				తిరిగి -FDT_ERR_BADSTRUCTURE;
			లోతు--;
			ఒకవేళ (లోతు == 0)
				expect_చివర = true;
			ఆపు;

		స్థితి FDT_PROP:
			prop = fdt_getprop_by_స్థానభ్రంశం(fdt, స్థానభ్రంశం, &propname,
						     &err);
			ఒకవేళ (!prop)
				తిరిగి err;
			ఆపు;

		అప్రమేయ:
			తిరిగి -FDT_ERR_అంతర్గత;
		}
	}
}
