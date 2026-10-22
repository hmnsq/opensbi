// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2012 David Gibson, IBM Corporation.
 */
#include "libfdt_env.h"

#include <fdt.h>
#include <libfdt.h>

#include "libfdt_internal.h"

целое fdt_create_empty_tree(void *buf, целое bufsize)
{
	целое err;

	err = fdt_create(buf, bufsize);
	если (err)
		вернуть err;

	err = fdt_finish_reservemap(buf);
	если (err)
		вернуть err;

	err = fdt_begin_node(buf, "");
	если (err)
		вернуть err;

	err =  fdt_end_node(buf);
	если (err)
		вернуть err;

	err = fdt_finish(buf);
	если (err)
		вернуть err;

	вернуть fdt_open_into(buf, buf, bufsize);
}
