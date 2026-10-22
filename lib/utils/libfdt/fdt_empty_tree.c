// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2012 David Gibson, IBM Corporation.
 */
#include "libfdt_env.h"

#include <fdt.h>
#include <libfdt.h>

#include "libfdt_internal.h"

පූර්ණ_සංඛ්‍යා fdt_create_empty_tree(void *buf, පූර්ණ_සංඛ්‍යා bufsize)
{
	පූර්ණ_සංඛ්‍යා err;

	err = fdt_create(buf, bufsize);
	නම් (err)
		ආපසු err;

	err = fdt_finish_reservemap(buf);
	නම් (err)
		ආපසු err;

	err = fdt_begin_node(buf, "");
	නම් (err)
		ආපසු err;

	err =  fdt_end_node(buf);
	නම් (err)
		ආපසු err;

	err = fdt_finish(buf);
	නම් (err)
		ආපසු err;

	ආපසු fdt_open_into(buf, buf, bufsize);
}
