// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2012 David Gibson, IBM Corporation.
 */
#അന്തര്ഭാവ "libfdt_env.h"

#അന്തര്ഭാവ <fdt.h>
#അന്തര്ഭാവ <libfdt.h>

#അന്തര്ഭാവ "libfdt_internal.h"

പൂർണ്ണസംഖ്യ fdt_create_empty_വൃക്ഷം(ശൂന്യം *buf, പൂർണ്ണസംഖ്യ bufsize)
{
	പൂർണ്ണസംഖ്യ err;

	err = fdt_create(buf, bufsize);
	എങ്കിൽ (err)
		തിരികെ err;

	err = fdt_finish_reservemap(buf);
	എങ്കിൽ (err)
		തിരികെ err;

	err = fdt_begin_കണ്ണി(buf, "");
	എങ്കിൽ (err)
		തിരികെ err;

	err =  fdt_end_കണ്ണി(buf);
	എങ്കിൽ (err)
		തിരികെ err;

	err = fdt_finish(buf);
	എങ്കിൽ (err)
		തിരികെ err;

	തിരികെ fdt_open_into(buf, buf, bufsize);
}
