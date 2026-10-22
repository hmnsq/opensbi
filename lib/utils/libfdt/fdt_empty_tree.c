// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2012 David Gibson, IBM Corporation.
 */
#include "libfdt_env.h"

#include <fdt.h>
#include <libfdt.h>

#include "libfdt_internal.h"

puur_naa_nka fdt_create_empty_tree(void *buf, puur_naa_nka bufsize)
{
	puur_naa_nka err;

	err = fdt_create(buf, bufsize);
	ꦪꦼꦤ꧀ (err)
		ꦮꦭꦶ err;

	err = fdt_finish_reservemap(buf);
	ꦪꦼꦤ꧀ (err)
		ꦮꦭꦶ err;

	err = fdt_begin_node(buf, "");
	ꦪꦼꦤ꧀ (err)
		ꦮꦭꦶ err;

	err =  fdt_end_node(buf);
	ꦪꦼꦤ꧀ (err)
		ꦮꦭꦶ err;

	err = fdt_finish(buf);
	ꦪꦼꦤ꧀ (err)
		ꦮꦭꦶ err;

	ꦮꦭꦶ fdt_open_into(buf, buf, bufsize);
}
