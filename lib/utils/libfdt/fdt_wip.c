// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#அந்தர்பாவ "libfdt_env.h"

#அந்தர்பாவ <fdt.h>
#அந்தர்பாவ <libfdt.h>

#அந்தர்பாவ "libfdt_internal.h"

முழு fdt_setprop_inplace_namelen_partial(வெறுமை *fdt, முழு nodeoffset,
					மாறிலி எழுத்து *name, முழு namelen,
					uint32_t idx, மாறிலி வெறுமை *val,
					முழு len)
{
	வெறுமை *propval;
	முழு proplen;

	propval = fdt_getprop_namelen_w(fdt, nodeoffset, name, namelen,
					&proplen);
	எனில் (!propval)
		திரும்பு proplen;

	எனில் ((unsigned)proplen < (len + idx))
		திரும்பு -FDT_ERR_NOSPACE;

	memcpy((எழுத்து *)propval + idx, val, len);
	திரும்பு 0;
}

முழு fdt_setprop_inplace(வெறுமை *fdt, முழு nodeoffset, மாறிலி எழுத்து *name,
			மாறிலி வெறுமை *val, முழு len)
{
	மாறிலி வெறுமை *propval;
	முழு proplen;

	propval = fdt_getprop(fdt, nodeoffset, name, &proplen);
	எனில் (!propval)
		திரும்பு proplen;

	எனில் (proplen != len)
		திரும்பு -FDT_ERR_NOSPACE;

	திரும்பு fdt_setprop_inplace_namelen_partial(fdt, nodeoffset, name,
						   strlen(name), 0,
						   val, len);
}

நிலையான வெறுமை fdt_nop_region_(வெறுமை *start, முழு len)
{
	fdt32_t *p;

	க்கு (p = start; (எழுத்து *)p < ((எழுத்து *)start + len); p++)
		*p = cpu_to_fdt32(FDT_NOP);
}

முழு fdt_nop_property(வெறுமை *fdt, முழு nodeoffset, மாறிலி எழுத்து *name)
{
	கட்டமைப்பு fdt_property *prop;
	முழு len;

	prop = fdt_get_property_w(fdt, nodeoffset, name, &len);
	எனில் (!prop)
		திரும்பு len;

	fdt_nop_region_(prop, len + அளவு(*prop));

	திரும்பு 0;
}

முழு fdt_முனை_end_offset(வெறுமை *fdt, முழு offset)
{
	முழு depth = 0;

	போது ((offset >= 0) && (depth >= 0))
		offset = fdt_next_முனை(fdt, offset, &depth);

	திரும்பு offset;
}

முழு fdt_nop_முனை(வெறுமை *fdt, முழு nodeoffset)
{
	முழு endoffset;

	endoffset = fdt_முனை_end_offset(fdt, nodeoffset);
	எனில் (endoffset < 0)
		திரும்பு endoffset;

	fdt_nop_region_(fdt_offset_ptr_w(fdt, nodeoffset, 0),
			endoffset - nodeoffset);
	திரும்பு 0;
}
