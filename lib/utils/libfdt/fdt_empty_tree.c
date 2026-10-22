// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2012 David Gibson, IBM Corporation.
 */
#অন্তর্ভাৱ "libfdt_env.h"

#অন্তর্ভাৱ <fdt.h>
#অন্তর্ভাৱ <libfdt.h>

#অন্তর্ভাৱ "libfdt_internal.h"

পূর্ণাঙ্ক fdt_create_empty_বৃক্ষ(শূন্য *buf, পূর্ণাঙ্ক bufsize)
{
	পূর্ণাঙ্ক err;

	err = fdt_create(buf, bufsize);
	যদি (err)
		ঘূৰাই err;

	err = fdt_finish_reservemap(buf);
	যদি (err)
		ঘূৰাই err;

	err = fdt_begin_গ্ৰন্থি(buf, "");
	যদি (err)
		ঘূৰাই err;

	err =  fdt_end_গ্ৰন্থি(buf);
	যদি (err)
		ঘূৰাই err;

	err = fdt_finish(buf);
	যদি (err)
		ঘূৰাই err;

	ঘূৰাই fdt_open_into(buf, buf, bufsize);
}
