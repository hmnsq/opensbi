// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2012 David Gibson, IBM Corporation.
 */
#include "libfdt_env.h"

#include <fdt.h>
#include <libfdt.h>

#include "libfdt_internal.h"

ціле fdt_create_empty_tree(void *buf, ціле bufsize)
{
	ціле err;

	err = fdt_create(buf, bufsize);
	якщо (err)
		повернути err;

	err = fdt_finish_reservemap(buf);
	якщо (err)
		повернути err;

	err = fdt_begin_node(buf, "");
	якщо (err)
		повернути err;

	err =  fdt_end_node(buf);
	якщо (err)
		повернути err;

	err = fdt_finish(buf);
	якщо (err)
		повернути err;

	повернути fdt_open_into(buf, buf, bufsize);
}
