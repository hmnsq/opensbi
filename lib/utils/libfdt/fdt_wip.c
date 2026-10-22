// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#অন্তর্ভাব "libfdt_env.h"

#অন্তর্ভাব <fdt.h>
#অন্তর্ভাব <libfdt.h>

#অন্তর্ভাব "libfdt_internal.h"

পূর্ণসংখ্যা fdt_setprop_inplace_namelen_partial(শূন্য *fdt, পূর্ণসংখ্যা nodeoffset,
					ধ্রুবক অক্ষর *name, পূর্ণসংখ্যা namelen,
					uint32_t idx, ধ্রুবক শূন্য *val,
					পূর্ণসংখ্যা len)
{
	শূন্য *propval;
	পূর্ণসংখ্যা proplen;

	propval = fdt_getprop_namelen_w(fdt, nodeoffset, name, namelen,
					&proplen);
	যদি (!propval)
		ফেরত proplen;

	যদি ((unsigned)proplen < (len + idx))
		ফেরত -FDT_ERR_NOSPACE;

	memcpy((অক্ষর *)propval + idx, val, len);
	ফেরত 0;
}

পূর্ণসংখ্যা fdt_setprop_inplace(শূন্য *fdt, পূর্ণসংখ্যা nodeoffset, ধ্রুবক অক্ষর *name,
			ধ্রুবক শূন্য *val, পূর্ণসংখ্যা len)
{
	ধ্রুবক শূন্য *propval;
	পূর্ণসংখ্যা proplen;

	propval = fdt_getprop(fdt, nodeoffset, name, &proplen);
	যদি (!propval)
		ফেরত proplen;

	যদি (proplen != len)
		ফেরত -FDT_ERR_NOSPACE;

	ফেরত fdt_setprop_inplace_namelen_partial(fdt, nodeoffset, name,
						   strlen(name), 0,
						   val, len);
}

স্থির শূন্য fdt_nop_region_(শূন্য *start, পূর্ণসংখ্যা len)
{
	fdt32_t *p;

	জন্য (p = start; (অক্ষর *)p < ((অক্ষর *)start + len); p++)
		*p = cpu_to_fdt32(FDT_NOP);
}

পূর্ণসংখ্যা fdt_nop_property(শূন্য *fdt, পূর্ণসংখ্যা nodeoffset, ধ্রুবক অক্ষর *name)
{
	কাঠামো fdt_property *prop;
	পূর্ণসংখ্যা len;

	prop = fdt_get_property_w(fdt, nodeoffset, name, &len);
	যদি (!prop)
		ফেরত len;

	fdt_nop_region_(prop, len + আকার(*prop));

	ফেরত 0;
}

পূর্ণসংখ্যা fdt_গ্রন্থি_end_offset(শূন্য *fdt, পূর্ণসংখ্যা offset)
{
	পূর্ণসংখ্যা depth = 0;

	যখন ((offset >= 0) && (depth >= 0))
		offset = fdt_পরবর্তী_গ্রন্থি(fdt, offset, &depth);

	ফেরত offset;
}

পূর্ণসংখ্যা fdt_nop_গ্রন্থি(শূন্য *fdt, পূর্ণসংখ্যা nodeoffset)
{
	পূর্ণসংখ্যা endoffset;

	endoffset = fdt_গ্রন্থি_end_offset(fdt, nodeoffset);
	যদি (endoffset < 0)
		ফেরত endoffset;

	fdt_nop_region_(fdt_offset_ptr_w(fdt, nodeoffset, 0),
			endoffset - nodeoffset);
	ফেরত 0;
}
