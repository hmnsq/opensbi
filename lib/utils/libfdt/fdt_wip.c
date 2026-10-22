// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#ਅਨ੍ਤਰ੍ਭਾਵ "libfdt_env.h"

#ਅਨ੍ਤਰ੍ਭਾਵ <fdt.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <libfdt.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "libfdt_internal.h"

ਪੂਰਨ fdt_setprop_inplace_namelen_partial(ਖਾਲੀ *fdt, ਪੂਰਨ nodeoffset,
					ਅਚਲ ਅੱਖਰ *name, ਪੂਰਨ namelen,
					uint32_t idx, ਅਚਲ ਖਾਲੀ *val,
					ਪੂਰਨ len)
{
	ਖਾਲੀ *propval;
	ਪੂਰਨ proplen;

	propval = fdt_getprop_namelen_w(fdt, nodeoffset, name, namelen,
					&proplen);
	ਜੇ (!propval)
		ਵਾਪਸ proplen;

	ਜੇ ((ਬਿਨਾਨਿਸ਼ਾਨੀ)proplen < (len + idx))
		ਵਾਪਸ -FDT_ERR_NOSPACE;

	ਸਮ੍ਰਿਤੀ_ਨਕਲ((ਅੱਖਰ *)propval + idx, val, len);
	ਵਾਪਸ 0;
}

ਪੂਰਨ fdt_setprop_inplace(ਖਾਲੀ *fdt, ਪੂਰਨ nodeoffset, ਅਚਲ ਅੱਖਰ *name,
			ਅਚਲ ਖਾਲੀ *val, ਪੂਰਨ len)
{
	ਅਚਲ ਖਾਲੀ *propval;
	ਪੂਰਨ proplen;

	propval = fdt_getprop(fdt, nodeoffset, name, &proplen);
	ਜੇ (!propval)
		ਵਾਪਸ proplen;

	ਜੇ (proplen != len)
		ਵਾਪਸ -FDT_ERR_NOSPACE;

	ਵਾਪਸ fdt_setprop_inplace_namelen_partial(fdt, nodeoffset, name,
						   ਲੜੀ_ਲੰਬਾਈ(name), 0,
						   val, len);
}

ਸਥਿਰ ਖਾਲੀ fdt_nop_region_(ਖਾਲੀ *ਸ਼ੁਰੂ, ਪੂਰਨ len)
{
	fdt32_t *p;

	ਲਈ (p = ਸ਼ੁਰੂ; (ਅੱਖਰ *)p < ((ਅੱਖਰ *)ਸ਼ੁਰੂ + len); p++)
		*p = cpu_ਮੰਜ਼ਿਲ_fdt32(FDT_NOP);
}

ਪੂਰਨ fdt_nop_property(ਖਾਲੀ *fdt, ਪੂਰਨ nodeoffset, ਅਚਲ ਅੱਖਰ *name)
{
	ਬਣਤਰ fdt_property *prop;
	ਪੂਰਨ len;

	prop = fdt_get_property_w(fdt, nodeoffset, name, &len);
	ਜੇ (!prop)
		ਵਾਪਸ len;

	fdt_nop_region_(prop, len + ਆਕਾਰ(*prop));

	ਵਾਪਸ 0;
}

ਪੂਰਨ fdt_ਗੰਢ_ਸਿਰਾ_ਵਿੱਥ(ਖਾਲੀ *fdt, ਪੂਰਨ ਵਿੱਥ)
{
	ਪੂਰਨ ਡੂੰਘਾਈ = 0;

	ਜਦੋਂ ((ਵਿੱਥ >= 0) && (ਡੂੰਘਾਈ >= 0))
		ਵਿੱਥ = fdt_ਅਗਲਾ_ਗੰਢ(fdt, ਵਿੱਥ, &ਡੂੰਘਾਈ);

	ਵਾਪਸ ਵਿੱਥ;
}

ਪੂਰਨ fdt_nop_ਗੰਢ(ਖਾਲੀ *fdt, ਪੂਰਨ nodeoffset)
{
	ਪੂਰਨ endoffset;

	endoffset = fdt_ਗੰਢ_ਸਿਰਾ_ਵਿੱਥ(fdt, nodeoffset);
	ਜੇ (endoffset < 0)
		ਵਾਪਸ endoffset;

	fdt_nop_region_(fdt_ਵਿੱਥ_ptr_w_2(fdt, nodeoffset, 0),
			endoffset - nodeoffset);
	ਵਾਪਸ 0;
}
