// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2012 David Gibson, IBM Corporation.
 */
#include "libfdt_env.h"

#include <fdt.h>
#include <libfdt.h>

#include "libfdt_internal.h"

จำนวนเต็ม fdt_create_empty_tree(void *buf, จำนวนเต็ม bufsize)
{
	จำนวนเต็ม err;

	err = fdt_create(buf, bufsize);
	ถ้า (err)
		คืน err;

	err = fdt_finish_reservemap(buf);
	ถ้า (err)
		คืน err;

	err = fdt_begin_node(buf, "");
	ถ้า (err)
		คืน err;

	err =  fdt_end_node(buf);
	ถ้า (err)
		คืน err;

	err = fdt_finish(buf);
	ถ้า (err)
		คืน err;

	คืน fdt_open_into(buf, buf, bufsize);
}
