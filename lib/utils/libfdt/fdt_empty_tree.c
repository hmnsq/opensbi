// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2012 David Gibson, IBM Corporation.
 */
#include "libfdt_env.h"

#include <fdt.h>
#include <libfdt.h>

#include "libfdt_internal.h"

정수 fdt_create_empty_tree(void *buf, 정수 bufsize)
{
	정수 err;

	err = fdt_create(buf, bufsize);
	만약 (err)
		반환 err;

	err = fdt_finish_reservemap(buf);
	만약 (err)
		반환 err;

	err = fdt_begin_node(buf, "");
	만약 (err)
		반환 err;

	err =  fdt_end_node(buf);
	만약 (err)
		반환 err;

	err = fdt_finish(buf);
	만약 (err)
		반환 err;

	반환 fdt_open_into(buf, buf, bufsize);
}
