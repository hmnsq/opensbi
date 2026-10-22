// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#అన్తర్భావ "libfdt_env.h"

#అన్తర్భావ <fdt.h>
#అన్తర్భావ <libfdt.h>

#అన్తర్భావ "libfdt_internal.h"

పూర్ణాంకం fdt_setprop_inplace_namelen_partial(శూన్య *fdt, పూర్ణాంకం nodeoffset,
					స్థిరరాశి అక్షరం *name, పూర్ణాంకం namelen,
					uint32_t idx, స్థిరరాశి శూన్య *val,
					పూర్ణాంకం len)
{
	శూన్య *propval;
	పూర్ణాంకం proplen;

	propval = fdt_getprop_namelen_w(fdt, nodeoffset, name, namelen,
					&proplen);
	ఒకవేళ (!propval)
		తిరిగి proplen;

	ఒకవేళ ((గుర్తులేని)proplen < (len + idx))
		తిరిగి -FDT_ERR_NOSPACE;

	జ్ఞాపకం_నకలు((అక్షరం *)propval + idx, val, len);
	తిరిగి 0;
}

పూర్ణాంకం fdt_setprop_inplace(శూన్య *fdt, పూర్ణాంకం nodeoffset, స్థిరరాశి అక్షరం *name,
			స్థిరరాశి శూన్య *val, పూర్ణాంకం len)
{
	స్థిరరాశి శూన్య *propval;
	పూర్ణాంకం proplen;

	propval = fdt_getprop(fdt, nodeoffset, name, &proplen);
	ఒకవేళ (!propval)
		తిరిగి proplen;

	ఒకవేళ (proplen != len)
		తిరిగి -FDT_ERR_NOSPACE;

	తిరిగి fdt_setprop_inplace_namelen_partial(fdt, nodeoffset, name,
						   అక్షరశ్రేణి_పొడవు(name), 0,
						   val, len);
}

స్థిర శూన్య fdt_nop_region_(శూన్య *ప్రారంభం, పూర్ణాంకం len)
{
	fdt32_t *p;

	కొరకు (p = ప్రారంభం; (అక్షరం *)p < ((అక్షరం *)ప్రారంభం + len); p++)
		*p = cpu_గమ్యం_fdt32(FDT_NOP);
}

పూర్ణాంకం fdt_nop_property(శూన్య *fdt, పూర్ణాంకం nodeoffset, స్థిరరాశి అక్షరం *name)
{
	నిర్మాణం fdt_property *prop;
	పూర్ణాంకం len;

	prop = fdt_get_property_w(fdt, nodeoffset, name, &len);
	ఒకవేళ (!prop)
		తిరిగి len;

	fdt_nop_region_(prop, len + పరిమాణం(*prop));

	తిరిగి 0;
}

పూర్ణాంకం fdt_కణుపు_చివర_స్థానభ్రంశం(శూన్య *fdt, పూర్ణాంకం స్థానభ్రంశం)
{
	పూర్ణాంకం లోతు = 0;

	అయితే ((స్థానభ్రంశం >= 0) && (లోతు >= 0))
		స్థానభ్రంశం = fdt_తదుపరి_కణుపు(fdt, స్థానభ్రంశం, &లోతు);

	తిరిగి స్థానభ్రంశం;
}

పూర్ణాంకం fdt_nop_కణుపు(శూన్య *fdt, పూర్ణాంకం nodeoffset)
{
	పూర్ణాంకం endoffset;

	endoffset = fdt_కణుపు_చివర_స్థానభ్రంశం(fdt, nodeoffset);
	ఒకవేళ (endoffset < 0)
		తిరిగి endoffset;

	fdt_nop_region_(fdt_స్థానభ్రంశం_ptr_w_2(fdt, nodeoffset, 0),
			endoffset - nodeoffset);
	తిరిగి 0;
}
