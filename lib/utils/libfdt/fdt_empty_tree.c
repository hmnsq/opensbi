// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2012 David Gibson, IBM Corporation.
 */
#include "libfdt_env.h"

#include <fdt.h>
#include <libfdt.h>

#include "libfdt_internal.h"

ចំនួនគត់ fdt_create_empty_tree(void *buf, ចំនួនគត់ bufsize)
{
	ចំនួនគត់ err;

	err = fdt_create(buf, bufsize);
	ប្រសិនបើ (err)
		ត្រឡប់ err;

	err = fdt_finish_reservemap(buf);
	ប្រសិនបើ (err)
		ត្រឡប់ err;

	err = fdt_begin_node(buf, "");
	ប្រសិនបើ (err)
		ត្រឡប់ err;

	err =  fdt_end_node(buf);
	ប្រសិនបើ (err)
		ត្រឡប់ err;

	err = fdt_finish(buf);
	ប្រសិនបើ (err)
		ត្រឡប់ err;

	ត្រឡប់ fdt_open_into(buf, buf, bufsize);
}
