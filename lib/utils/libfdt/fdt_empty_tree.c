// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2012 David Gibson, IBM Corporation.
 */
#அந்தர்பாவ "libfdt_env.h"

#அந்தர்பாவ <fdt.h>
#அந்தர்பாவ <libfdt.h>

#அந்தர்பாவ "libfdt_internal.h"

முழு fdt_create_empty_மரம்(வெறுமை *buf, முழு bufsize)
{
	முழு err;

	err = fdt_create(buf, bufsize);
	எனில் (err)
		திரும்பு err;

	err = fdt_finish_reservemap(buf);
	எனில் (err)
		திரும்பு err;

	err = fdt_begin_முனை(buf, "");
	எனில் (err)
		திரும்பு err;

	err =  fdt_end_முனை(buf);
	எனில் (err)
		திரும்பு err;

	err = fdt_finish(buf);
	எனில் (err)
		திரும்பு err;

	திரும்பு fdt_open_into(buf, buf, bufsize);
}
