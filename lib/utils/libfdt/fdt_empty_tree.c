// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2012 David Gibson, IBM Corporation.
 */
#include "libfdt_env.h"

#include <fdt.h>
#include <libfdt.h>

#include "libfdt_internal.h"

ຈຳນວນເຕັມ fdt_create_empty_tree(void *buf, ຈຳນວນເຕັມ bufsize)
{
	ຈຳນວນເຕັມ err;

	err = fdt_create(buf, bufsize);
	ຖ້າ (err)
		ສົ່ງຄືນ err;

	err = fdt_finish_reservemap(buf);
	ຖ້າ (err)
		ສົ່ງຄືນ err;

	err = fdt_begin_node(buf, "");
	ຖ້າ (err)
		ສົ່ງຄືນ err;

	err =  fdt_end_node(buf);
	ຖ້າ (err)
		ສົ່ງຄືນ err;

	err = fdt_finish(buf);
	ຖ້າ (err)
		ສົ່ງຄືນ err;

	ສົ່ງຄືນ fdt_open_into(buf, buf, bufsize);
}
