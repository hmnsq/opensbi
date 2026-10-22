// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2012 David Gibson, IBM Corporation.
 */
#समावेश "libfdt_env.h"

#समावेश <fdt.h>
#समावेश <libfdt.h>

#समावेश "libfdt_internal.h"

पूर्णांक fdt_create_empty_वृक्ष(व्योम *buf, पूर्णांक bufsize)
{
	पूर्णांक err;

	err = fdt_create(buf, bufsize);
	अगर (err)
		वापस err;

	err = fdt_finish_reservemap(buf);
	अगर (err)
		वापस err;

	err = fdt_begin_गाँठ(buf, "");
	अगर (err)
		वापस err;

	err =  fdt_end_गाँठ(buf);
	अगर (err)
		वापस err;

	err = fdt_finish(buf);
	अगर (err)
		वापस err;

	वापस fdt_open_into(buf, buf, bufsize);
}
