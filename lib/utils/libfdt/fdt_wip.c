// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#অন্তর্ভাব "libfdt_env.h"

#অন্তর্ভাব <fdt.h>
#অন্তর্ভাব <libfdt.h>

#অন্তর্ভাব "libfdt_internal.h"

পূর্ণাঙ্ক fdt_setprop_inplace_namelen_partial(শূন্য *fdt, পূর্ণাঙ্ক nodeoffset,
					ধ্রুব বর্ণ *name, পূর্ণাঙ্ক namelen,
					uint32_t idx, ধ্রুব শূন্য *val,
					পূর্ণাঙ্ক len)
{
	শূন্য *propval;
	পূর্ণাঙ্ক proplen;

	propval = fdt_getprop_namelen_w(fdt, nodeoffset, name, namelen,
					&proplen);
	যদি (!propval)
		প্রত্যাগম proplen;

	যদি ((unsigned)proplen < (len + idx))
		প্রত্যাগম -FDT_ERR_NOSPACE;

	memcpy((বর্ণ *)propval + idx, val, len);
	প্রত্যাগম 0;
}

পূর্ণাঙ্ক fdt_setprop_inplace(শূন্য *fdt, পূর্ণাঙ্ক nodeoffset, ধ্রুব বর্ণ *name,
			ধ্রুব শূন্য *val, পূর্ণাঙ্ক len)
{
	ধ্রুব শূন্য *propval;
	পূর্ণাঙ্ক proplen;

	propval = fdt_getprop(fdt, nodeoffset, name, &proplen);
	যদি (!propval)
		প্রত্যাগম proplen;

	যদি (proplen != len)
		প্রত্যাগম -FDT_ERR_NOSPACE;

	প্রত্যাগম fdt_setprop_inplace_namelen_partial(fdt, nodeoffset, name,
						   strlen(name), 0,
						   val, len);
}

স্থির শূন্য fdt_nop_region_(শূন্য *start, পূর্ণাঙ্ক len)
{
	fdt32_t *p;

	প্রতি (p = start; (বর্ণ *)p < ((বর্ণ *)start + len); p++)
		*p = cpu_to_fdt32(FDT_NOP);
}

পূর্ণাঙ্ক fdt_nop_property(শূন্য *fdt, পূর্ণাঙ্ক nodeoffset, ধ্রুব বর্ণ *name)
{
	সংরচনা fdt_property *prop;
	পূর্ণাঙ্ক len;

	prop = fdt_get_property_w(fdt, nodeoffset, name, &len);
	যদি (!prop)
		প্রত্যাগম len;

	fdt_nop_region_(prop, len + পরিমাণ(*prop));

	প্রত্যাগম 0;
}

পূর্ণাঙ্ক fdt_গ্রন্থি_end_offset(শূন্য *fdt, পূর্ণাঙ্ক offset)
{
	পূর্ণাঙ্ক depth = 0;

	যাবত্ ((offset >= 0) && (depth >= 0))
		offset = fdt_next_গ্রন্থি(fdt, offset, &depth);

	প্রত্যাগম offset;
}

পূর্ণাঙ্ক fdt_nop_গ্রন্থি(শূন্য *fdt, পূর্ণাঙ্ক nodeoffset)
{
	পূর্ণাঙ্ক endoffset;

	endoffset = fdt_গ্রন্থি_end_offset(fdt, nodeoffset);
	যদি (endoffset < 0)
		প্রত্যাগম endoffset;

	fdt_nop_region_(fdt_offset_ptr_w(fdt, nodeoffset, 0),
			endoffset - nodeoffset);
	প্রত্যাগম 0;
}
