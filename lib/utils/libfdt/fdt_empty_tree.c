// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2012 David Gibson, IBM Corporation.
 */
#ಅನ್ತರ್ಭಾವ "libfdt_env.h"

#ಅನ್ತರ್ಭಾವ <fdt.h>
#ಅನ್ತರ್ಭಾವ <libfdt.h>

#ಅನ್ತರ್ಭಾವ "libfdt_internal.h"

ಪೂರ್ಣಾಂಕ fdt_create_ಖಾಲಿಗಳು_ವೃಕ್ಷ(ಶೂನ್ಯ *buf, ಪೂರ್ಣಾಂಕ bufsize)
{
	ಪೂರ್ಣಾಂಕ err;

	err = fdt_create(buf, bufsize);
	ಆದರೆ (err)
		ಹಿಂತಿರುಗಿ err;

	err = fdt_ಮುಕ್ತಾಯ_reservemap(buf);
	ಆದರೆ (err)
		ಹಿಂತಿರುಗಿ err;

	err = fdt_begin_ಗಂಟು(buf, "");
	ಆದರೆ (err)
		ಹಿಂತಿರುಗಿ err;

	err =  fdt_ಕೊನೆ_ಗಂಟು(buf);
	ಆದರೆ (err)
		ಹಿಂತಿರುಗಿ err;

	err = fdt_ಮುಕ್ತಾಯ(buf);
	ಆದರೆ (err)
		ಹಿಂತಿರುಗಿ err;

	ಹಿಂತಿರುಗಿ fdt_open_into(buf, buf, bufsize);
}
