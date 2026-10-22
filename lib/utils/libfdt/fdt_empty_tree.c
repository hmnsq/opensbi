// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2012 David Gibson, IBM Corporation.
 */
#ಅನ್ತರ್ಭಾವ "libfdt_env.h"

#ಅನ್ತರ್ಭಾವ <fdt.h>
#ಅನ್ತರ್ಭಾವ <libfdt.h>

#ಅನ್ತರ್ಭಾವ "libfdt_internal.h"

ಪೂರ್ಣಾಙ್ಕ fdt_create_empty_ವೃಕ್ಷ(ಶೂನ್ಯ *buf, ಪೂರ್ಣಾಙ್ಕ bufsize)
{
	ಪೂರ್ಣಾಙ್ಕ err;

	err = fdt_create(buf, bufsize);
	ಯದಿ (err)
		ಪ್ರತ್ಯಾಗಮ err;

	err = fdt_finish_reservemap(buf);
	ಯದಿ (err)
		ಪ್ರತ್ಯಾಗಮ err;

	err = fdt_begin_ಗಂಟು(buf, "");
	ಯದಿ (err)
		ಪ್ರತ್ಯಾಗಮ err;

	err =  fdt_end_ಗಂಟು(buf);
	ಯದಿ (err)
		ಪ್ರತ್ಯಾಗಮ err;

	err = fdt_finish(buf);
	ಯದಿ (err)
		ಪ್ರತ್ಯಾಗಮ err;

	ಪ್ರತ್ಯಾಗಮ fdt_open_into(buf, buf, bufsize);
}
