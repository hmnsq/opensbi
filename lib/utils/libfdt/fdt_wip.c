// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#ಅನ್ತರ್ಭಾವ "libfdt_env.h"

#ಅನ್ತರ್ಭಾವ <fdt.h>
#ಅನ್ತರ್ಭಾವ <libfdt.h>

#ಅನ್ತರ್ಭಾವ "libfdt_internal.h"

ಪೂರ್ಣಾಙ್ಕ fdt_setprop_inplace_namelen_partial(ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ nodeoffset,
					ಧ್ರುವ ವರ್ಣ *name, ಪೂರ್ಣಾಙ್ಕ namelen,
					uint32_t idx, ಧ್ರುವ ಶೂನ್ಯ *val,
					ಪೂರ್ಣಾಙ್ಕ len)
{
	ಶೂನ್ಯ *propval;
	ಪೂರ್ಣಾಙ್ಕ proplen;

	propval = fdt_getprop_namelen_w(fdt, nodeoffset, name, namelen,
					&proplen);
	ಯದಿ (!propval)
		ಪ್ರತ್ಯಾಗಮ proplen;

	ಯದಿ ((unsigned)proplen < (len + idx))
		ಪ್ರತ್ಯಾಗಮ -FDT_ERR_NOSPACE;

	memcpy((ವರ್ಣ *)propval + idx, val, len);
	ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ fdt_setprop_inplace(ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ nodeoffset, ಧ್ರುವ ವರ್ಣ *name,
			ಧ್ರುವ ಶೂನ್ಯ *val, ಪೂರ್ಣಾಙ್ಕ len)
{
	ಧ್ರುವ ಶೂನ್ಯ *propval;
	ಪೂರ್ಣಾಙ್ಕ proplen;

	propval = fdt_getprop(fdt, nodeoffset, name, &proplen);
	ಯದಿ (!propval)
		ಪ್ರತ್ಯಾಗಮ proplen;

	ಯದಿ (proplen != len)
		ಪ್ರತ್ಯಾಗಮ -FDT_ERR_NOSPACE;

	ಪ್ರತ್ಯಾಗಮ fdt_setprop_inplace_namelen_partial(fdt, nodeoffset, name,
						   strlen(name), 0,
						   val, len);
}

ಸ್ಥಿರ ಶೂನ್ಯ fdt_nop_region_(ಶೂನ್ಯ *start, ಪೂರ್ಣಾಙ್ಕ len)
{
	fdt32_t *p;

	ಪ್ರತಿ (p = start; (ವರ್ಣ *)p < ((ವರ್ಣ *)start + len); p++)
		*p = cpu_to_fdt32(FDT_NOP);
}

ಪೂರ್ಣಾಙ್ಕ fdt_nop_property(ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ nodeoffset, ಧ್ರುವ ವರ್ಣ *name)
{
	ಸಂರಚನಾ fdt_property *prop;
	ಪೂರ್ಣಾಙ್ಕ len;

	prop = fdt_get_property_w(fdt, nodeoffset, name, &len);
	ಯದಿ (!prop)
		ಪ್ರತ್ಯಾಗಮ len;

	fdt_nop_region_(prop, len + ಪರಿಮಾಣ(*prop));

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ fdt_ಗಂಟು_end_offset(ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ offset)
{
	ಪೂರ್ಣಾಙ್ಕ depth = 0;

	ಯಾವತ್ ((offset >= 0) && (depth >= 0))
		offset = fdt_next_ಗಂಟು(fdt, offset, &depth);

	ಪ್ರತ್ಯಾಗಮ offset;
}

ಪೂರ್ಣಾಙ್ಕ fdt_nop_ಗಂಟು(ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ nodeoffset)
{
	ಪೂರ್ಣಾಙ್ಕ endoffset;

	endoffset = fdt_ಗಂಟು_end_offset(fdt, nodeoffset);
	ಯದಿ (endoffset < 0)
		ಪ್ರತ್ಯಾಗಮ endoffset;

	fdt_nop_region_(fdt_offset_ptr_w(fdt, nodeoffset, 0),
			endoffset - nodeoffset);
	ಪ್ರತ್ಯಾಗಮ 0;
}
