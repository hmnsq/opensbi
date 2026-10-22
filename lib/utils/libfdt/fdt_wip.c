// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#અન્તર્ભાવ "libfdt_env.h"

#અન્તર્ભાવ <fdt.h>
#અન્તર્ભાવ <libfdt.h>

#અન્તર્ભાવ "libfdt_internal.h"

પૂર્ણાંક fdt_setprop_inplace_namelen_partial(શૂન્ય *fdt, પૂર્ણાંક nodeoffset,
					અચલ અક્ષર *name, પૂર્ણાંક namelen,
					uint32_t idx, અચલ શૂન્ય *val,
					પૂર્ણાંક len)
{
	શૂન્ય *propval;
	પૂર્ણાંક proplen;

	propval = fdt_getprop_namelen_w(fdt, nodeoffset, name, namelen,
					&proplen);
	જો (!propval)
		પાછા proplen;

	જો ((unsigned)proplen < (len + idx))
		પાછા -FDT_ERR_NOSPACE;

	memcpy((અક્ષર *)propval + idx, val, len);
	પાછા 0;
}

પૂર્ણાંક fdt_setprop_inplace(શૂન્ય *fdt, પૂર્ણાંક nodeoffset, અચલ અક્ષર *name,
			અચલ શૂન્ય *val, પૂર્ણાંક len)
{
	અચલ શૂન્ય *propval;
	પૂર્ણાંક proplen;

	propval = fdt_getprop(fdt, nodeoffset, name, &proplen);
	જો (!propval)
		પાછા proplen;

	જો (proplen != len)
		પાછા -FDT_ERR_NOSPACE;

	પાછા fdt_setprop_inplace_namelen_partial(fdt, nodeoffset, name,
						   strlen(name), 0,
						   val, len);
}

સ્થિર શૂન્ય fdt_nop_region_(શૂન્ય *start, પૂર્ણાંક len)
{
	fdt32_t *p;

	માટે (p = start; (અક્ષર *)p < ((અક્ષર *)start + len); p++)
		*p = cpu_to_fdt32(FDT_NOP);
}

પૂર્ણાંક fdt_nop_property(શૂન્ય *fdt, પૂર્ણાંક nodeoffset, અચલ અક્ષર *name)
{
	સંરચના fdt_property *prop;
	પૂર્ણાંક len;

	prop = fdt_get_property_w(fdt, nodeoffset, name, &len);
	જો (!prop)
		પાછા len;

	fdt_nop_region_(prop, len + કદ(*prop));

	પાછા 0;
}

પૂર્ણાંક fdt_ગાંઠ_end_offset(શૂન્ય *fdt, પૂર્ણાંક offset)
{
	પૂર્ણાંક depth = 0;

	જ્યારે ((offset >= 0) && (depth >= 0))
		offset = fdt_next_ગાંઠ(fdt, offset, &depth);

	પાછા offset;
}

પૂર્ણાંક fdt_nop_ગાંઠ(શૂન્ય *fdt, પૂર્ણાંક nodeoffset)
{
	પૂર્ણાંક endoffset;

	endoffset = fdt_ગાંઠ_end_offset(fdt, nodeoffset);
	જો (endoffset < 0)
		પાછા endoffset;

	fdt_nop_region_(fdt_offset_ptr_w(fdt, nodeoffset, 0),
			endoffset - nodeoffset);
	પાછા 0;
}
