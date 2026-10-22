// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2012 David Gibson, IBM Corporation.
 */
#অন্তর্ভাব "libfdt_env.h"

#অন্তর্ভাব <fdt.h>
#অন্তর্ভাব <libfdt.h>

#অন্তর্ভাব "libfdt_internal.h"

পূর্ণসংখ্যা fdt_create_empty_বৃক্ষ(শূন্য *buf, পূর্ণসংখ্যা bufsize)
{
	পূর্ণসংখ্যা err;

	err = fdt_create(buf, bufsize);
	যদি (err)
		ফেরত err;

	err = fdt_finish_reservemap(buf);
	যদি (err)
		ফেরত err;

	err = fdt_begin_গ্রন্থি(buf, "");
	যদি (err)
		ফেরত err;

	err =  fdt_end_গ্রন্থি(buf);
	যদি (err)
		ফেরত err;

	err = fdt_finish(buf);
	যদি (err)
		ফেরত err;

	ফেরত fdt_open_into(buf, buf, bufsize);
}
