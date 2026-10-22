// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2012 David Gibson, IBM Corporation.
 */
#include "libfdt_env.h"

#include <fdt.h>
#include <libfdt.h>

#include "libfdt_internal.h"

цео fdt_create_empty_tree(void *buf, цео bufsize)
{
	цео err;

	err = fdt_create(buf, bufsize);
	ако (err)
		врати err;

	err = fdt_finish_reservemap(buf);
	ако (err)
		врати err;

	err = fdt_begin_node(buf, "");
	ако (err)
		врати err;

	err =  fdt_end_node(buf);
	ако (err)
		врати err;

	err = fdt_finish(buf);
	ако (err)
		врати err;

	врати fdt_open_into(buf, buf, bufsize);
}
