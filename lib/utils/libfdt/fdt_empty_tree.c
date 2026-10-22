// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2012 David Gibson, IBM Corporation.
 */
#अन्तर्भाव "libfdt_env.h"

#अन्तर्भाव <fdt.h>
#अन्तर्भाव <libfdt.h>

#अन्तर्भाव "libfdt_internal.h"

पूर्णांक fdt_create_empty_वृक्ष(शून्य *buf, पूर्णांक bufsize)
{
	पूर्णांक err;

	err = fdt_create(buf, bufsize);
	जर (err)
		परत err;

	err = fdt_finish_reservemap(buf);
	जर (err)
		परत err;

	err = fdt_begin_गाठ(buf, "");
	जर (err)
		परत err;

	err =  fdt_end_गाठ(buf);
	जर (err)
		परत err;

	err = fdt_finish(buf);
	जर (err)
		परत err;

	परत fdt_open_into(buf, buf, bufsize);
}
