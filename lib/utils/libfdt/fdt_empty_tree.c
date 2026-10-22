// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2012 David Gibson, IBM Corporation.
 */
#شامل "libfdt_env.h"

#شامل <fdt.h>
#شامل <libfdt.h>

#شامل "libfdt_internal.h"

صحیح fdt_create_empty_درخت(تهی *buf, صحیح bufsize)
{
	صحیح err;

	err = fdt_create(buf, bufsize);
	اگر (err)
		بازگشت err;

	err = fdt_finish_reservemap(buf);
	اگر (err)
		بازگشت err;

	err = fdt_begin_گره(buf, "");
	اگر (err)
		بازگشت err;

	err =  fdt_end_گره(buf);
	اگر (err)
		بازگشت err;

	err = fdt_finish(buf);
	اگر (err)
		بازگشت err;

	بازگشت fdt_open_into(buf, buf, bufsize);
}
