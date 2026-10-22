// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#അന്തര്ഭാവ "libfdt_env.h"

#അന്തര്ഭാവ <fdt.h>
#അന്തര്ഭാവ <libfdt.h>

#അന്തര്ഭാവ "libfdt_internal.h"

പൂർണ്ണസംഖ്യ fdt_setprop_inplace_namelen_partial(ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ nodeoffset,
					സ്ഥിരം അക്ഷരം *name, പൂർണ്ണസംഖ്യ namelen,
					uint32_t idx, സ്ഥിരം ശൂന്യം *val,
					പൂർണ്ണസംഖ്യ len)
{
	ശൂന്യം *propval;
	പൂർണ്ണസംഖ്യ proplen;

	propval = fdt_getprop_namelen_w(fdt, nodeoffset, name, namelen,
					&proplen);
	എങ്കിൽ (!propval)
		തിരികെ proplen;

	എങ്കിൽ ((unsigned)proplen < (len + idx))
		തിരികെ -FDT_ERR_NOSPACE;

	memcpy((അക്ഷരം *)propval + idx, val, len);
	തിരികെ 0;
}

പൂർണ്ണസംഖ്യ fdt_setprop_inplace(ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ nodeoffset, സ്ഥിരം അക്ഷരം *name,
			സ്ഥിരം ശൂന്യം *val, പൂർണ്ണസംഖ്യ len)
{
	സ്ഥിരം ശൂന്യം *propval;
	പൂർണ്ണസംഖ്യ proplen;

	propval = fdt_getprop(fdt, nodeoffset, name, &proplen);
	എങ്കിൽ (!propval)
		തിരികെ proplen;

	എങ്കിൽ (proplen != len)
		തിരികെ -FDT_ERR_NOSPACE;

	തിരികെ fdt_setprop_inplace_namelen_partial(fdt, nodeoffset, name,
						   strlen(name), 0,
						   val, len);
}

സ്ഥിതം ശൂന്യം fdt_nop_region_(ശൂന്യം *start, പൂർണ്ണസംഖ്യ len)
{
	fdt32_t *p;

	വേണ്ടി (p = start; (അക്ഷരം *)p < ((അക്ഷരം *)start + len); p++)
		*p = cpu_to_fdt32(FDT_NOP);
}

പൂർണ്ണസംഖ്യ fdt_nop_property(ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ nodeoffset, സ്ഥിരം അക്ഷരം *name)
{
	ഘടന fdt_property *prop;
	പൂർണ്ണസംഖ്യ len;

	prop = fdt_get_property_w(fdt, nodeoffset, name, &len);
	എങ്കിൽ (!prop)
		തിരികെ len;

	fdt_nop_region_(prop, len + വലുപ്പം(*prop));

	തിരികെ 0;
}

പൂർണ്ണസംഖ്യ fdt_കണ്ണി_end_offset(ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ offset)
{
	പൂർണ്ണസംഖ്യ depth = 0;

	ആകുമ്പോൾ ((offset >= 0) && (depth >= 0))
		offset = fdt_next_കണ്ണി(fdt, offset, &depth);

	തിരികെ offset;
}

പൂർണ്ണസംഖ്യ fdt_nop_കണ്ണി(ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ nodeoffset)
{
	പൂർണ്ണസംഖ്യ endoffset;

	endoffset = fdt_കണ്ണി_end_offset(fdt, nodeoffset);
	എങ്കിൽ (endoffset < 0)
		തിരികെ endoffset;

	fdt_nop_region_(fdt_offset_ptr_w(fdt, nodeoffset, 0),
			endoffset - nodeoffset);
	തിരികെ 0;
}
