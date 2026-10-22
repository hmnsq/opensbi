// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2012 David Gibson, IBM Corporation.
 */
#ضمن "libfdt_env.h"

#ضمن <fdt.h>
#ضمن <libfdt.h>

#ضمن "libfdt_internal.h"

صحيح fdt_create_خالي_جايون_وڻ(فراغ *buf, صحيح bufsize)
{
	صحيح err;

	err = fdt_create(buf, bufsize);
	إذا (err)
		أرجع err;

	err = fdt_پڄاڻي_reservemap(buf);
	إذا (err)
		أرجع err;

	err = fdt_begin_ڳنڍ(buf, "");
	إذا (err)
		أرجع err;

	err =  fdt_پڇاڙي_ڳنڍ(buf);
	إذا (err)
		أرجع err;

	err = fdt_پڄاڻي(buf);
	إذا (err)
		أرجع err;

	أرجع fdt_open_into(buf, buf, bufsize);
}
