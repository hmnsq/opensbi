// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2012 David Gibson, IBM Corporation.
 */
#ضمن "libfdt_env.h"

#ضمن <fdt.h>
#ضمن <libfdt.h>

#ضمن "libfdt_internal.h"

صحيح fdt_create_empty_tree(فراغ *buf, صحيح bufsize)
{
	صحيح err;

	err = fdt_create(buf, bufsize);
	אם (err)
		החזר err;

	err = fdt_finish_reservemap(buf);
	אם (err)
		החזר err;

	err = fdt_begin_node(buf, "");
	אם (err)
		החזר err;

	err =  fdt_end_node(buf);
	אם (err)
		החזר err;

	err = fdt_finish(buf);
	אם (err)
		החזר err;

	החזר fdt_open_into(buf, buf, bufsize);
}
