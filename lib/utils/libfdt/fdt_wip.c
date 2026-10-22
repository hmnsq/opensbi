// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#include "libfdt_env.h"

#include <fdt.h>
#include <libfdt.h>

#include "libfdt_internal.h"

පූර්ණ_සංඛ්‍යා fdt_setprop_inplace_namelen_partial(void *fdt, පූර්ණ_සංඛ්‍යා nodeoffset,
					const var_na *name, පූර්ණ_සංඛ්‍යා namelen,
					uint32_t idx, const void *val,
					පූර්ණ_සංඛ්‍යා len)
{
	void *propval;
	පූර්ණ_සංඛ්‍යා proplen;

	propval = fdt_getprop_namelen_w(fdt, nodeoffset, name, namelen,
					&proplen);
	නම් (!propval)
		ආපසු proplen;

	නම් ((unsigned)proplen < (len + idx))
		ආපසු -FDT_ERR_NOSPACE;

	memcpy((var_na *)propval + idx, val, len);
	ආපසු 0;
}

පූර්ණ_සංඛ්‍යා fdt_setprop_inplace(void *fdt, පූර්ණ_සංඛ්‍යා nodeoffset, const var_na *name,
			const void *val, පූර්ණ_සංඛ්‍යා len)
{
	const void *propval;
	පූර්ණ_සංඛ්‍යා proplen;

	propval = fdt_getprop(fdt, nodeoffset, name, &proplen);
	නම් (!propval)
		ආපසු proplen;

	නම් (proplen != len)
		ආපසු -FDT_ERR_NOSPACE;

	ආපසු fdt_setprop_inplace_namelen_partial(fdt, nodeoffset, name,
						   strlen(name), 0,
						   val, len);
}

static void fdt_nop_region_(void *start, පූර්ණ_සංඛ්‍යා len)
{
	fdt32_t *p;

	සඳහා (p = start; (var_na *)p < ((var_na *)start + len); p++)
		*p = cpu_to_fdt32(FDT_NOP);
}

පූර්ණ_සංඛ්‍යා fdt_nop_property(void *fdt, පූර්ණ_සංඛ්‍යා nodeoffset, const var_na *name)
{
	sa_nracanaa fdt_property *prop;
	පූර්ණ_සංඛ්‍යා len;

	prop = fdt_get_property_w(fdt, nodeoffset, name, &len);
	නම් (!prop)
		ආපසු len;

	fdt_nop_region_(prop, len + parimaa_na(*prop));

	ආපසු 0;
}

පූර්ණ_සංඛ්‍යා fdt_node_end_offset_(void *fdt, පූර්ණ_සංඛ්‍යා offset)
{
	පූර්ණ_සංඛ්‍යා depth = 0;

	අතරතුර ((offset >= 0) && (depth >= 0))
		offset = fdt_next_node(fdt, offset, &depth);

	ආපසු offset;
}

පූර්ණ_සංඛ්‍යා fdt_nop_node(void *fdt, පූර්ණ_සංඛ්‍යා nodeoffset)
{
	පූර්ණ_සංඛ්‍යා endoffset;

	endoffset = fdt_node_end_offset_(fdt, nodeoffset);
	නම් (endoffset < 0)
		ආපසු endoffset;

	fdt_nop_region_(fdt_offset_ptr_w(fdt, nodeoffset, 0),
			endoffset - nodeoffset);
	ආපසු 0;
}
