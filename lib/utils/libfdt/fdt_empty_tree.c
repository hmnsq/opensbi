// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2012 David Gibson, IBM Corporation.
 */
#અન્તર્ભાવ "libfdt_env.h"

#અન્તર્ભાવ <fdt.h>
#અન્તર્ભાવ <libfdt.h>

#અન્તર્ભાવ "libfdt_internal.h"

પૂર્ણાંક fdt_create_empty_વૃક્ષ(શૂન્ય *buf, પૂર્ણાંક bufsize)
{
	પૂર્ણાંક err;

	err = fdt_create(buf, bufsize);
	જો (err)
		પાછા err;

	err = fdt_finish_reservemap(buf);
	જો (err)
		પાછા err;

	err = fdt_begin_ગાંઠ(buf, "");
	જો (err)
		પાછા err;

	err =  fdt_end_ગાંઠ(buf);
	જો (err)
		પાછા err;

	err = fdt_finish(buf);
	જો (err)
		પાછા err;

	પાછા fdt_open_into(buf, buf, bufsize);
}
