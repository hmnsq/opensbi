// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#ଅନ୍ତର୍ଭାଵ "libfdt_env.h"

#ଅନ୍ତର୍ଭାଵ <fdt.h>
#ଅନ୍ତର୍ଭାଵ <libfdt.h>

#ଅନ୍ତର୍ଭାଵ "libfdt_internal.h"

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_setprop_inplace_namelen_partial(ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nodeoffset,
					ଅଚଳ ଅକ୍ଷର *name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା namelen,
					uint32_t idx, ଅଚଳ ଶୂନ୍ୟ *val,
					ପୂର୍ଣ୍ଣସଂଖ୍ୟା len)
{
	ଶୂନ୍ୟ *propval;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା proplen;

	propval = fdt_getprop_namelen_w(fdt, nodeoffset, name, namelen,
					&proplen);
	ଯଦି (!propval)
		ଫେରନ୍ତୁ proplen;

	ଯଦି ((unsigned)proplen < (len + idx))
		ଫେରନ୍ତୁ -FDT_ERR_NOSPACE;

	memcpy((ଅକ୍ଷର *)propval + idx, val, len);
	ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_setprop_inplace(ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nodeoffset, ଅଚଳ ଅକ୍ଷର *name,
			ଅଚଳ ଶୂନ୍ୟ *val, ପୂର୍ଣ୍ଣସଂଖ୍ୟା len)
{
	ଅଚଳ ଶୂନ୍ୟ *propval;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା proplen;

	propval = fdt_getprop(fdt, nodeoffset, name, &proplen);
	ଯଦି (!propval)
		ଫେରନ୍ତୁ proplen;

	ଯଦି (proplen != len)
		ଫେରନ୍ତୁ -FDT_ERR_NOSPACE;

	ଫେରନ୍ତୁ fdt_setprop_inplace_namelen_partial(fdt, nodeoffset, name,
						   strlen(name), 0,
						   val, len);
}

ସ୍ଥିର ଶୂନ୍ୟ fdt_nop_region_(ଶୂନ୍ୟ *start, ପୂର୍ଣ୍ଣସଂଖ୍ୟା len)
{
	fdt32_t *p;

	ପାଇଁ (p = start; (ଅକ୍ଷର *)p < ((ଅକ୍ଷର *)start + len); p++)
		*p = cpu_to_fdt32(FDT_NOP);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_nop_property(ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nodeoffset, ଅଚଳ ଅକ୍ଷର *name)
{
	ସଂରଚନା fdt_property *prop;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା len;

	prop = fdt_get_property_w(fdt, nodeoffset, name, &len);
	ଯଦି (!prop)
		ଫେରନ୍ତୁ len;

	fdt_nop_region_(prop, len + ଆକାର(*prop));

	ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_ଗଣ୍ଠି_end_offset(ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା offset)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା depth = 0;

	ଯେତେବେଳେ ((offset >= 0) && (depth >= 0))
		offset = fdt_next_ଗଣ୍ଠି(fdt, offset, &depth);

	ଫେରନ୍ତୁ offset;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_nop_ଗଣ୍ଠି(ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nodeoffset)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା endoffset;

	endoffset = fdt_ଗଣ୍ଠି_end_offset(fdt, nodeoffset);
	ଯଦି (endoffset < 0)
		ଫେରନ୍ତୁ endoffset;

	fdt_nop_region_(fdt_offset_ptr_w(fdt, nodeoffset, 0),
			endoffset - nodeoffset);
	ଫେରନ୍ତୁ 0;
}
