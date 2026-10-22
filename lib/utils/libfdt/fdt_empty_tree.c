// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2012 David Gibson, IBM Corporation.
 */
#ଅନ୍ତର୍ଭାଵ "libfdt_env.h"

#ଅନ୍ତର୍ଭାଵ <fdt.h>
#ଅନ୍ତର୍ଭାଵ <libfdt.h>

#ଅନ୍ତର୍ଭାଵ "libfdt_internal.h"

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_create_empty_ବୃକ୍ଷ(ଶୂନ୍ୟ *buf, ପୂର୍ଣ୍ଣସଂଖ୍ୟା bufsize)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା err;

	err = fdt_create(buf, bufsize);
	ଯଦି (err)
		ଫେରନ୍ତୁ err;

	err = fdt_finish_reservemap(buf);
	ଯଦି (err)
		ଫେରନ୍ତୁ err;

	err = fdt_begin_ଗଣ୍ଠି(buf, "");
	ଯଦି (err)
		ଫେରନ୍ତୁ err;

	err =  fdt_end_ଗଣ୍ଠି(buf);
	ଯଦି (err)
		ଫେରନ୍ତୁ err;

	err = fdt_finish(buf);
	ଯଦି (err)
		ଫେରନ୍ତୁ err;

	ଫେରନ୍ତୁ fdt_open_into(buf, buf, bufsize);
}
