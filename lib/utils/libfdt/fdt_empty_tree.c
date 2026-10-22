// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2012 David Gibson, IBM Corporation.
 */
#ਅਨ੍ਤਰ੍ਭਾਵ "libfdt_env.h"

#ਅਨ੍ਤਰ੍ਭਾਵ <fdt.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <libfdt.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "libfdt_internal.h"

ਪੂਰਨ fdt_create_ਖਾਲੀ_ਥਾਵਾਂ_ਰੁੱਖ(ਖਾਲੀ *buf, ਪੂਰਨ bufsize)
{
	ਪੂਰਨ err;

	err = fdt_create(buf, bufsize);
	ਜੇ (err)
		ਵਾਪਸ err;

	err = fdt_ਅੰਤ_reservemap(buf);
	ਜੇ (err)
		ਵਾਪਸ err;

	err = fdt_begin_ਗੰਢ(buf, "");
	ਜੇ (err)
		ਵਾਪਸ err;

	err =  fdt_ਸਿਰਾ_ਗੰਢ(buf);
	ਜੇ (err)
		ਵਾਪਸ err;

	err = fdt_ਅੰਤ(buf);
	ਜੇ (err)
		ਵਾਪਸ err;

	ਵਾਪਸ fdt_open_into(buf, buf, bufsize);
}
