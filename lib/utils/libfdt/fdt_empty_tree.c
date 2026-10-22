// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2012 David Gibson, IBM Corporation.
 */
#अन्तर्भाव "libfdt_env.h"

#अन्तर्भाव <fdt.h>
#अन्तर्भाव <libfdt.h>

#अन्तर्भाव "libfdt_internal.h"

पूर्णाङ्क fdt_create_empty_वृक्षः(शून्य *buf, पूर्णाङ्क bufsize)
{
	पूर्णाङ्क err;

	err = fdt_create(buf, bufsize);
	सचे (err)
		पटिनिवत्त err;

	err = fdt_finish_reservemap(buf);
	सचे (err)
		पटिनिवत्त err;

	err = fdt_begin_ग्रन्थिः(buf, "");
	सचे (err)
		पटिनिवत्त err;

	err =  fdt_end_ग्रन्थिः(buf);
	सचे (err)
		पटिनिवत्त err;

	err = fdt_finish(buf);
	सचे (err)
		पटिनिवत्त err;

	पटिनिवत्त fdt_open_into(buf, buf, bufsize);
}
