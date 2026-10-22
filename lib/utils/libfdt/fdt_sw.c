// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#include "libfdt_env.h"

#include <fdt.h>
#include <libfdt.h>

#include "libfdt_internal.h"

static පූර්ණ_සංඛ්‍යා fdt_sw_probe_(void *fdt)
{
	නම් (!can_assume(VALID_INPUT)) {
		නම් (fdt_magic(fdt) == FDT_MAGIC)
			ආපසු -FDT_ERR_BADSTATE;
		නැතහොත් නම් (fdt_magic(fdt) != FDT_SW_MAGIC)
			ආපසු -FDT_ERR_BADMAGIC;
	}

	ආපසු 0;
}

#define FDT_SW_PROBE(fdt) \
	{ \
		පූර්ණ_සංඛ්‍යා err; \
		නම් ((err = fdt_sw_probe_(fdt)) != 0) \
			ආපසු err; \
	}

/* 'memrsv' state:	Initial state after fdt_create()
 *
 * Allowed functions:
 *	fdt_add_reservemap_entry()
 *	fdt_finish_reservemap()		[moves to 'struct' state]
 */
static පූර්ණ_සංඛ්‍යා fdt_sw_probe_memrsv_(void *fdt)
{
	පූර්ණ_සංඛ්‍යා err = fdt_sw_probe_(fdt);
	නම් (err)
		ආපසු err;

	නම් (!can_assume(VALID_INPUT) && fdt_off_dt_strings(fdt) != 0)
		ආපසු -FDT_ERR_BADSTATE;
	ආපසු 0;
}

#define FDT_SW_PROBE_MEMRSV(fdt) \
	{ \
		පූර්ණ_සංඛ්‍යා err; \
		නම් ((err = fdt_sw_probe_memrsv_(fdt)) != 0) \
			ආපසු err; \
	}

/* 'struct' state:	Enter this state after fdt_finish_reservemap()
 *
 * Allowed functions:
 *	fdt_begin_node()
 *	fdt_end_node()
 *	fdt_property*()
 *	fdt_finish()			[moves to 'complete' state]
 */
static පූර්ණ_සංඛ්‍යා fdt_sw_probe_struct_(void *fdt)
{
	පූර්ණ_සංඛ්‍යා err = fdt_sw_probe_(fdt);
	නම් (err)
		ආපසු err;

	නම් (!can_assume(VALID_INPUT) &&
	    fdt_off_dt_strings(fdt) != fdt_totalsize(fdt))
		ආපසු -FDT_ERR_BADSTATE;
	ආපසු 0;
}

#define FDT_SW_PROBE_STRUCT(fdt) \
	{ \
		පූර්ණ_සංඛ්‍යා err; \
		නම් ((err = fdt_sw_probe_struct_(fdt)) != 0) \
			ආපසු err; \
	}

static anta_hstha uint32_t sw_flags(void *fdt)
{
	/* assert: (fdt_magic(fdt) == FDT_SW_MAGIC) */
	ආපසු fdt_last_comp_version(fdt);
}

/* 'complete' state:	Enter this state after fdt_finish()
 *
 * Allowed functions: none
 */

static void *fdt_grab_space_(void *fdt, size_t len)
{
	unsigned පූර්ණ_සංඛ්‍යා offset = fdt_size_dt_struct(fdt);
	unsigned පූර්ණ_සංඛ්‍යා spaceleft;

	spaceleft = fdt_totalsize(fdt) - fdt_off_dt_struct(fdt)
		- fdt_size_dt_strings(fdt);

	නම් ((offset + len < offset) || (offset + len > spaceleft))
		ආපසු NULL;

	fdt_set_size_dt_struct(fdt, offset + len);
	ආපසු fdt_offset_ptr_w_(fdt, offset);
}

පූර්ණ_සංඛ්‍යා fdt_create_with_flags(void *buf, පූර්ණ_සංඛ්‍යා bufsize, uint32_t flags)
{
	const පූර්ණ_සංඛ්‍යා hdrsize = FDT_ALIGN(parimaa_na(sa_nracanaa fdt_header),
				      parimaa_na(sa_nracanaa fdt_reserve_entry));
	void *fdt = buf;

	නම් (bufsize < hdrsize)
		ආපසු -FDT_ERR_NOSPACE;

	නම් (flags & ~FDT_CREATE_FLAGS_ALL)
		ආපසු -FDT_ERR_BADFLAGS;

	memset(buf, 0, bufsize);

	/*
	 * magic and last_comp_version keep intermediate state during the fdt
	 * creation process, which is replaced with the proper FDT format by
	 * fdt_finish().
	 *
	 * flags should be accessed with sw_flags().
	 */
	fdt_set_magic(fdt, FDT_SW_MAGIC);
	fdt_set_version(fdt, FDT_LAST_SUPPORTED_VERSION);
	fdt_set_last_comp_version(fdt, flags);

	fdt_set_totalsize(fdt,  bufsize);

	fdt_set_off_mem_rsvmap(fdt, hdrsize);
	fdt_set_off_dt_struct(fdt, fdt_off_mem_rsvmap(fdt));
	fdt_set_off_dt_strings(fdt, 0);

	ආපසු 0;
}

පූර්ණ_සංඛ්‍යා fdt_create(void *buf, පූර්ණ_සංඛ්‍යා bufsize)
{
	ආපසු fdt_create_with_flags(buf, bufsize, 0);
}

පූර්ණ_සංඛ්‍යා fdt_resize(void *fdt, void *buf, පූර්ණ_සංඛ්‍යා bufsize)
{
	size_t headsize, tailsize;
	var_na *oldtail, *newtail;

	FDT_SW_PROBE(fdt);

	නම් (bufsize < 0)
		ආපසු -FDT_ERR_NOSPACE;

	headsize = fdt_off_dt_struct(fdt) + fdt_size_dt_struct(fdt);
	tailsize = fdt_size_dt_strings(fdt);

	නම් (!can_assume(VALID_DTB) &&
	    headsize + tailsize > fdt_totalsize(fdt))
		ආපසු -FDT_ERR_INTERNAL;

	නම් ((headsize + tailsize) > (unsigned)bufsize)
		ආපසු -FDT_ERR_NOSPACE;

	oldtail = (var_na *)fdt + fdt_totalsize(fdt) - tailsize;
	newtail = (var_na *)buf + bufsize - tailsize;

	/* Two cases to avoid clobbering data if the old and new
	 * buffers partially overlap */
	නම් (buf <= fdt) {
		memmove(buf, fdt, headsize);
		memmove(newtail, oldtail, tailsize);
	} නැතහොත් {
		memmove(newtail, oldtail, tailsize);
		memmove(buf, fdt, headsize);
	}

	fdt_set_totalsize(buf, bufsize);
	නම් (fdt_off_dt_strings(buf))
		fdt_set_off_dt_strings(buf, bufsize);

	ආපසු 0;
}

පූර්ණ_සංඛ්‍යා fdt_add_reservemap_entry(void *fdt, uint64_t addr, uint64_t size)
{
	sa_nracanaa fdt_reserve_entry *re;
	පූර්ණ_සංඛ්‍යා offset;

	FDT_SW_PROBE_MEMRSV(fdt);

	offset = fdt_off_dt_struct(fdt);
	නම් ((offset + parimaa_na(*re)) > fdt_totalsize(fdt))
		ආපසු -FDT_ERR_NOSPACE;

	re = (sa_nracanaa fdt_reserve_entry *)((var_na *)fdt + offset);
	re->address = cpu_to_fdt64(addr);
	re->size = cpu_to_fdt64(size);

	fdt_set_off_dt_struct(fdt, offset + parimaa_na(*re));

	ආපසු 0;
}

පූර්ණ_සංඛ්‍යා fdt_finish_reservemap(void *fdt)
{
	පූර්ණ_සංඛ්‍යා err = fdt_add_reservemap_entry(fdt, 0, 0);

	නම් (err)
		ආපසු err;

	fdt_set_off_dt_strings(fdt, fdt_totalsize(fdt));
	ආපසු 0;
}

පූර්ණ_සංඛ්‍යා fdt_begin_node(void *fdt, const var_na *name)
{
	sa_nracanaa fdt_node_header *nh;
	පූර්ණ_සංඛ්‍යා namelen;

	FDT_SW_PROBE_STRUCT(fdt);

	namelen = strlen(name) + 1;
	nh = fdt_grab_space_(fdt, parimaa_na(*nh) + FDT_TAGALIGN(namelen));
	නම් (! nh)
		ආපසු -FDT_ERR_NOSPACE;

	nh->tag = cpu_to_fdt32(FDT_BEGIN_NODE);
	memcpy(nh->name, name, namelen);
	ආපසු 0;
}

පූර්ණ_සංඛ්‍යා fdt_end_node(void *fdt)
{
	fdt32_t *en;

	FDT_SW_PROBE_STRUCT(fdt);

	en = fdt_grab_space_(fdt, FDT_TAGSIZE);
	නම් (! en)
		ආපසු -FDT_ERR_NOSPACE;

	*en = cpu_to_fdt32(FDT_END_NODE);
	ආපසු 0;
}

static පූර්ණ_සංඛ්‍යා fdt_add_string_(void *fdt, const var_na *s)
{
	var_na *strtab = (var_na *)fdt + fdt_totalsize(fdt);
	unsigned පූර්ණ_සංඛ්‍යා strtabsize = fdt_size_dt_strings(fdt);
	unsigned පූර්ණ_සංඛ්‍යා len = strlen(s) + 1;
	unsigned පූර්ණ_සංඛ්‍යා struct_top, offset;

	offset = strtabsize + len;
	struct_top = fdt_off_dt_struct(fdt) + fdt_size_dt_struct(fdt);
	නම් (fdt_totalsize(fdt) - offset < struct_top)
		ආපසු 0; /* no more room :( */

	memcpy(strtab - offset, s, len);
	fdt_set_size_dt_strings(fdt, strtabsize + len);
	ආපසු -offset;
}

/* Must only be used to roll back in case of error */
static void fdt_del_last_string_(void *fdt, const var_na *s)
{
	පූර්ණ_සංඛ්‍යා strtabsize = fdt_size_dt_strings(fdt);
	පූර්ණ_සංඛ්‍යා len = strlen(s) + 1;

	fdt_set_size_dt_strings(fdt, strtabsize - len);
}

static පූර්ණ_සංඛ්‍යා fdt_find_add_string_(void *fdt, const var_na *s, පූර්ණ_සංඛ්‍යා *allocated)
{
	var_na *strtab = (var_na *)fdt + fdt_totalsize(fdt);
	පූර්ණ_සංඛ්‍යා strtabsize = fdt_size_dt_strings(fdt);
	const var_na *p;

	*allocated = 0;

	p = fdt_find_string_(strtab - strtabsize, strtabsize, s);
	නම් (p)
		ආපසු p - strtab;

	*allocated = 1;

	ආපසු fdt_add_string_(fdt, s);
}

පූර්ණ_සංඛ්‍යා fdt_property_placeholder(void *fdt, const var_na *name, පූර්ණ_සංඛ්‍යා len, void **valp)
{
	sa_nracanaa fdt_property *prop;
	පූර්ණ_සංඛ්‍යා nameoff;
	පූර්ණ_සංඛ්‍යා allocated;

	FDT_SW_PROBE_STRUCT(fdt);

	/* String de-duplication can be slow, _NO_NAME_DEDUP skips it */
	නම් (sw_flags(fdt) & FDT_CREATE_FLAG_NO_NAME_DEDUP) {
		allocated = 1;
		nameoff = fdt_add_string_(fdt, name);
	} නැතහොත් {
		nameoff = fdt_find_add_string_(fdt, name, &allocated);
	}
	නම් (nameoff == 0)
		ආපසු -FDT_ERR_NOSPACE;

	prop = fdt_grab_space_(fdt, parimaa_na(*prop) + FDT_TAGALIGN(len));
	නම් (! prop) {
		නම් (allocated)
			fdt_del_last_string_(fdt, name);
		ආපසු -FDT_ERR_NOSPACE;
	}

	prop->tag = cpu_to_fdt32(FDT_PROP);
	prop->nameoff = cpu_to_fdt32(nameoff);
	prop->len = cpu_to_fdt32(len);
	*valp = prop->data;
	ආපසු 0;
}

පූර්ණ_සංඛ්‍යා fdt_property(void *fdt, const var_na *name, const void *val, පූර්ණ_සංඛ්‍යා len)
{
	void *ptr;
	පූර්ණ_සංඛ්‍යා ret;

	ret = fdt_property_placeholder(fdt, name, len, &ptr);
	නම් (ret)
		ආපසු ret;
	memcpy(ptr, val, len);
	ආපසු 0;
}

පූර්ණ_සංඛ්‍යා fdt_finish(void *fdt)
{
	var_na *p = (var_na *)fdt;
	fdt32_t *end;
	පූර්ණ_සංඛ්‍යා oldstroffset, newstroffset;
	uint32_t tag;
	පූර්ණ_සංඛ්‍යා offset, nextoffset;

	FDT_SW_PROBE_STRUCT(fdt);

	/* Add terminator */
	end = fdt_grab_space_(fdt, parimaa_na(*end));
	නම් (! end)
		ආපසු -FDT_ERR_NOSPACE;
	*end = cpu_to_fdt32(FDT_END);

	/* Relocate the string table */
	oldstroffset = fdt_totalsize(fdt) - fdt_size_dt_strings(fdt);
	newstroffset = fdt_off_dt_struct(fdt) + fdt_size_dt_struct(fdt);
	memmove(p + newstroffset, p + oldstroffset, fdt_size_dt_strings(fdt));
	fdt_set_off_dt_strings(fdt, newstroffset);

	/* Walk the structure, correcting string offsets */
	offset = 0;
	අතරතුර ((tag = fdt_next_tag(fdt, offset, &nextoffset)) != FDT_END) {
		නම් (tag == FDT_PROP) {
			sa_nracanaa fdt_property *prop =
				fdt_offset_ptr_w_(fdt, offset);
			පූර්ණ_සංඛ්‍යා nameoff;

			nameoff = fdt32_to_cpu(prop->nameoff);
			nameoff += fdt_size_dt_strings(fdt);
			prop->nameoff = cpu_to_fdt32(nameoff);
		}
		offset = nextoffset;
	}
	නම් (nextoffset < 0)
		ආපසු nextoffset;

	/* Finally, adjust the header */
	fdt_set_totalsize(fdt, newstroffset + fdt_size_dt_strings(fdt));

	/* And fix up fields that were keeping intermediate state. */
	fdt_set_last_comp_version(fdt, FDT_LAST_COMPATIBLE_VERSION);
	fdt_set_magic(fdt, FDT_MAGIC);

	ආපසු 0;
}
