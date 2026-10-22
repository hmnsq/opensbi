// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2012 David Gibson, IBM Corporation.
 */
#అన్తర్భావ "libfdt_env.h"

#అన్తర్భావ <fdt.h>
#అన్తర్భావ <libfdt.h>

#అన్తర్భావ "libfdt_internal.h"

పూర్ణాంకం fdt_create_ఖాళీలు_వృక్షం(శూన్య *buf, పూర్ణాంకం bufsize)
{
	పూర్ణాంకం err;

	err = fdt_create(buf, bufsize);
	ఒకవేళ (err)
		తిరిగి err;

	err = fdt_ముగింపు_reservemap(buf);
	ఒకవేళ (err)
		తిరిగి err;

	err = fdt_begin_కణుపు(buf, "");
	ఒకవేళ (err)
		తిరిగి err;

	err =  fdt_చివర_కణుపు(buf);
	ఒకవేళ (err)
		తిరిగి err;

	err = fdt_ముగింపు(buf);
	ఒకవేళ (err)
		తిరిగి err;

	తిరిగి fdt_open_into(buf, buf, bufsize);
}
