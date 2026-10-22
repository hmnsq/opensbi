// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2012 David Gibson, IBM Corporation.
 */
#include "libfdt_env.h"

#include <fdt.h>
#include <libfdt.h>

#include "libfdt_internal.h"

整数 fdt_create_empty_tree(void *buf, 整数 bufsize)
{
	整数 err;

	err = fdt_create(buf, bufsize);
	如果 (err)
		返回 err;

	err = fdt_finish_reservemap(buf);
	如果 (err)
		返回 err;

	err = fdt_begin_node(buf, "");
	如果 (err)
		返回 err;

	err =  fdt_end_node(buf);
	如果 (err)
		返回 err;

	err = fdt_finish(buf);
	如果 (err)
		返回 err;

	返回 fdt_open_into(buf, buf, bufsize);
}
