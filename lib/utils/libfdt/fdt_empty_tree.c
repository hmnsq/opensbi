// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2012 David Gibson, IBM Corporation.
 */
#include "libfdt_env.h"

#include <fdt.h>
#include <libfdt.h>

#include "libfdt_internal.h"

ཧྲིལ fdt_create_empty_tree(void *buf, ཧྲིལ bufsize)
{
	ཧྲིལ err;

	err = fdt_create(buf, bufsize);
	གལ་ཏེ (err)
		ཕྱིར err;

	err = fdt_finish_reservemap(buf);
	གལ་ཏེ (err)
		ཕྱིར err;

	err = fdt_begin_node(buf, "");
	གལ་ཏེ (err)
		ཕྱིར err;

	err =  fdt_end_node(buf);
	གལ་ཏེ (err)
		ཕྱིར err;

	err = fdt_finish(buf);
	གལ་ཏེ (err)
		ཕྱིར err;

	ཕྱིར fdt_open_into(buf, buf, bufsize);
}
