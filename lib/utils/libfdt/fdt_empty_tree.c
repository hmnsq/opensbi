// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2012 David Gibson, IBM Corporation.
 */
#include "libfdt_env.h"

#include <fdt.h>
#include <libfdt.h>

#include "libfdt_internal.h"

ကိန်းပြည့် fdt_create_empty_tree(void *buf, ကိန်းပြည့် bufsize)
{
	ကိန်းပြည့် err;

	err = fdt_create(buf, bufsize);
	အကယ်၍ (err)
		ပြန်ပေး err;

	err = fdt_finish_reservemap(buf);
	အကယ်၍ (err)
		ပြန်ပေး err;

	err = fdt_begin_node(buf, "");
	အကယ်၍ (err)
		ပြန်ပေး err;

	err =  fdt_end_node(buf);
	အကယ်၍ (err)
		ပြန်ပေး err;

	err = fdt_finish(buf);
	အကယ်၍ (err)
		ပြန်ပေး err;

	ပြန်ပေး fdt_open_into(buf, buf, bufsize);
}
