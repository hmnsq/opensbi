// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2012 David Gibson, IBM Corporation.
 */
#شامل "libfdt_env.h"

#شامل <fdt.h>
#شامل <libfdt.h>

#شامل "libfdt_internal.h"

صحیح fdt_create_خالی_خانے_درخت(خالی *buf, صحیح bufsize)
{
	صحیح err;

	err = fdt_create(buf, bufsize);
	اگر (err)
		واپس err;

	err = fdt_اختتام_reservemap(buf);
	اگر (err)
		واپس err;

	err = fdt_begin_گرہ(buf, "");
	اگر (err)
		واپس err;

	err =  fdt_انتہا_گرہ(buf);
	اگر (err)
		واپس err;

	err = fdt_اختتام(buf);
	اگر (err)
		واپس err;

	واپس fdt_open_into(buf, buf, bufsize);
}
