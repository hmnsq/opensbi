// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#include "libfdt_env.h"

#include <fdt.h>
#include <libfdt.h>

#include "libfdt_internal.h"

ຈຳນວນເຕັມ fdt_setprop_inplace_namelen_partial(void *fdt, ຈຳນວນເຕັມ nodeoffset,
					const var_na *name, ຈຳນວນເຕັມ namelen,
					uint32_t idx, const void *val,
					ຈຳນວນເຕັມ len)
{
	void *propval;
	ຈຳນວນເຕັມ proplen;

	propval = fdt_getprop_namelen_w(fdt, nodeoffset, name, namelen,
					&proplen);
	ຖ້າ (!propval)
		ສົ່ງຄືນ proplen;

	ຖ້າ ((unsigned)proplen < (len + idx))
		ສົ່ງຄືນ -FDT_ERR_NOSPACE;

	memcpy((var_na *)propval + idx, val, len);
	ສົ່ງຄືນ 0;
}

ຈຳນວນເຕັມ fdt_setprop_inplace(void *fdt, ຈຳນວນເຕັມ nodeoffset, const var_na *name,
			const void *val, ຈຳນວນເຕັມ len)
{
	const void *propval;
	ຈຳນວນເຕັມ proplen;

	propval = fdt_getprop(fdt, nodeoffset, name, &proplen);
	ຖ້າ (!propval)
		ສົ່ງຄືນ proplen;

	ຖ້າ (proplen != len)
		ສົ່ງຄືນ -FDT_ERR_NOSPACE;

	ສົ່ງຄືນ fdt_setprop_inplace_namelen_partial(fdt, nodeoffset, name,
						   strlen(name), 0,
						   val, len);
}

static void fdt_nop_region_(void *start, ຈຳນວນເຕັມ len)
{
	fdt32_t *p;

	ສຳລັບ (p = start; (var_na *)p < ((var_na *)start + len); p++)
		*p = cpu_to_fdt32(FDT_NOP);
}

ຈຳນວນເຕັມ fdt_nop_property(void *fdt, ຈຳນວນເຕັມ nodeoffset, const var_na *name)
{
	sa_nracanaa fdt_property *prop;
	ຈຳນວນເຕັມ len;

	prop = fdt_get_property_w(fdt, nodeoffset, name, &len);
	ຖ້າ (!prop)
		ສົ່ງຄືນ len;

	fdt_nop_region_(prop, len + parimaa_na(*prop));

	ສົ່ງຄືນ 0;
}

ຈຳນວນເຕັມ fdt_node_end_offset_(void *fdt, ຈຳນວນເຕັມ offset)
{
	ຈຳນວນເຕັມ depth = 0;

	ໃນຂະນະທີ່ ((offset >= 0) && (depth >= 0))
		offset = fdt_next_node(fdt, offset, &depth);

	ສົ່ງຄືນ offset;
}

ຈຳນວນເຕັມ fdt_nop_node(void *fdt, ຈຳນວນເຕັມ nodeoffset)
{
	ຈຳນວນເຕັມ endoffset;

	endoffset = fdt_node_end_offset_(fdt, nodeoffset);
	ຖ້າ (endoffset < 0)
		ສົ່ງຄືນ endoffset;

	fdt_nop_region_(fdt_offset_ptr_w(fdt, nodeoffset, 0),
			endoffset - nodeoffset);
	ສົ່ງຄືນ 0;
}
