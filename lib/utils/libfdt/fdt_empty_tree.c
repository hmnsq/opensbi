// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2012 David Gibson, IBM Corporation.
 */
#include "libfdt_env.h"

#include <fdt.h>
#include <libfdt.h>

#include "libfdt_internal.h"

số_nguyên fdt_create_empty_tree(void *buf, số_nguyên bufsize)
{
	số_nguyên err;

	err = fdt_create(buf, bufsize);
	nếu (err)
		trả_về err;

	err = fdt_finish_reservemap(buf);
	nếu (err)
		trả_về err;

	err = fdt_begin_node(buf, "");
	nếu (err)
		trả_về err;

	err =  fdt_end_node(buf);
	nếu (err)
		trả_về err;

	err = fdt_finish(buf);
	nếu (err)
		trả_về err;

	trả_về fdt_open_into(buf, buf, bufsize);
}
