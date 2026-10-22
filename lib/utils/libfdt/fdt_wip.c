// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#ಅನ್ತರ್ಭಾವ "libfdt_env.h"

#ಅನ್ತರ್ಭಾವ <fdt.h>
#ಅನ್ತರ್ಭಾವ <libfdt.h>

#ಅನ್ತರ್ಭಾವ "libfdt_internal.h"

ಪೂರ್ಣಾಂಕ fdt_setprop_inplace_namelen_partial(ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ nodeoffset,
					ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, ಪೂರ್ಣಾಂಕ namelen,
					uint32_t idx, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *val,
					ಪೂರ್ಣಾಂಕ len)
{
	ಶೂನ್ಯ *propval;
	ಪೂರ್ಣಾಂಕ proplen;

	propval = fdt_getprop_namelen_w(fdt, nodeoffset, name, namelen,
					&proplen);
	ಆದರೆ (!propval)
		ಹಿಂತಿರುಗಿ proplen;

	ಆದರೆ ((ಚಿಹ್ನೆರಹಿತ)proplen < (len + idx))
		ಹಿಂತಿರುಗಿ -FDT_ERR_NOSPACE;

	ಸ್ಮರಣೆ_ನಕಲು((ಅಕ್ಷರ *)propval + idx, val, len);
	ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ fdt_setprop_inplace(ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ nodeoffset, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name,
			ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *val, ಪೂರ್ಣಾಂಕ len)
{
	ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *propval;
	ಪೂರ್ಣಾಂಕ proplen;

	propval = fdt_getprop(fdt, nodeoffset, name, &proplen);
	ಆದರೆ (!propval)
		ಹಿಂತಿರುಗಿ proplen;

	ಆದರೆ (proplen != len)
		ಹಿಂತಿರುಗಿ -FDT_ERR_NOSPACE;

	ಹಿಂತಿರುಗಿ fdt_setprop_inplace_namelen_partial(fdt, nodeoffset, name,
						   ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(name), 0,
						   val, len);
}

ಸ್ಥಿರ ಶೂನ್ಯ fdt_nop_region_(ಶೂನ್ಯ *ಆರಂಭ, ಪೂರ್ಣಾಂಕ len)
{
	fdt32_t *p;

	ಗಾಗಿ (p = ಆರಂಭ; (ಅಕ್ಷರ *)p < ((ಅಕ್ಷರ *)ಆರಂಭ + len); p++)
		*p = cpu_ಗಮ್ಯ_fdt32(FDT_NOP);
}

ಪೂರ್ಣಾಂಕ fdt_nop_property(ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ nodeoffset, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name)
{
	ರಚನೆ fdt_property *prop;
	ಪೂರ್ಣಾಂಕ len;

	prop = fdt_get_property_w(fdt, nodeoffset, name, &len);
	ಆದರೆ (!prop)
		ಹಿಂತಿರುಗಿ len;

	fdt_nop_region_(prop, len + ಗಾತ್ರ(*prop));

	ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ fdt_ಗಂಟು_ಕೊನೆ_ಸ್ಥಾನಾಂತರ(ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ ಸ್ಥಾನಾಂತರ)
{
	ಪೂರ್ಣಾಂಕ ಆಳ = 0;

	ಆಗ ((ಸ್ಥಾನಾಂತರ >= 0) && (ಆಳ >= 0))
		ಸ್ಥಾನಾಂತರ = fdt_ಮುಂದಿನ_ಗಂಟು(fdt, ಸ್ಥಾನಾಂತರ, &ಆಳ);

	ಹಿಂತಿರುಗಿ ಸ್ಥಾನಾಂತರ;
}

ಪೂರ್ಣಾಂಕ fdt_nop_ಗಂಟು(ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ nodeoffset)
{
	ಪೂರ್ಣಾಂಕ endoffset;

	endoffset = fdt_ಗಂಟು_ಕೊನೆ_ಸ್ಥಾನಾಂತರ(fdt, nodeoffset);
	ಆದರೆ (endoffset < 0)
		ಹಿಂತಿರುಗಿ endoffset;

	fdt_nop_region_(fdt_ಸ್ಥಾನಾಂತರ_ptr_w_2(fdt, nodeoffset, 0),
			endoffset - nodeoffset);
	ಹಿಂತಿರುಗಿ 0;
}
