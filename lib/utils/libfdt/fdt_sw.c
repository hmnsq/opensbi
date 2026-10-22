// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#include "libfdt_env.h"

#include <fdt.h>
#include <libfdt.h>

#include "libfdt_internal.h"

static ཧྲིལ fdt_sw_probe_(void *fdt)
{
	གལ་ཏེ (!can_assume(VALID_INPUT)) {
		གལ་ཏེ (fdt_magic(fdt) == FDT_MAGIC)
			ཕྱིར -FDT_ERR_BADSTATE;
		དེ་མིན གལ་ཏེ (fdt_magic(fdt) != FDT_SW_MAGIC)
			ཕྱིར -FDT_ERR_BADMAGIC;
	}

	ཕྱིར 0;
}

#define FDT_SW_PROBE(fdt) \
	{ \
		ཧྲིལ err; \
		གལ་ཏེ ((err = fdt_sw_probe_(fdt)) != 0) \
			ཕྱིར err; \
	}

/* 'memrsv' state:	Initial state after fdt_create()
 *
 * Allowed functions:
 *	fdt_add_reservemap_entry()
 *	fdt_finish_reservemap()		[moves to 'struct' state]
 */
static ཧྲིལ fdt_sw_probe_memrsv_(void *fdt)
{
	ཧྲིལ err = fdt_sw_probe_(fdt);
	གལ་ཏེ (err)
		ཕྱིར err;

	གལ་ཏེ (!can_assume(VALID_INPUT) && fdt_off_dt_strings(fdt) != 0)
		ཕྱིར -FDT_ERR_BADSTATE;
	ཕྱིར 0;
}

#define FDT_SW_PROBE_MEMRSV(fdt) \
	{ \
		ཧྲིལ err; \
		གལ་ཏེ ((err = fdt_sw_probe_memrsv_(fdt)) != 0) \
			ཕྱིར err; \
	}

/* 'struct' state:	Enter this state after fdt_finish_reservemap()
 *
 * Allowed functions:
 *	fdt_begin_node()
 *	fdt_end_node()
 *	fdt_property*()
 *	fdt_finish()			[moves to 'complete' state]
 */
static ཧྲིལ fdt_sw_probe_struct_(void *fdt)
{
	ཧྲིལ err = fdt_sw_probe_(fdt);
	གལ་ཏེ (err)
		ཕྱིར err;

	གལ་ཏེ (!can_assume(VALID_INPUT) &&
	    fdt_off_dt_strings(fdt) != fdt_totalsize(fdt))
		ཕྱིར -FDT_ERR_BADSTATE;
	ཕྱིར 0;
}

#define FDT_SW_PROBE_STRUCT(fdt) \
	{ \
		ཧྲིལ err; \
		གལ་ཏེ ((err = fdt_sw_probe_struct_(fdt)) != 0) \
			ཕྱིར err; \
	}

static anta_hstha uint32_t sw_flags(void *fdt)
{
	/* assert: (fdt_magic(fdt) == FDT_SW_MAGIC) */
	ཕྱིར fdt_last_comp_version(fdt);
}

/* 'complete' state:	Enter this state after fdt_finish()
 *
 * Allowed functions: none
 */

static void *fdt_grab_space_(void *fdt, size_t len)
{
	unsigned ཧྲིལ offset = fdt_size_dt_struct(fdt);
	unsigned ཧྲིལ spaceleft;

	spaceleft = fdt_totalsize(fdt) - fdt_off_dt_struct(fdt)
		- fdt_size_dt_strings(fdt);

	གལ་ཏེ ((offset + len < offset) || (offset + len > spaceleft))
		ཕྱིར NULL;

	fdt_set_size_dt_struct(fdt, offset + len);
	ཕྱིར fdt_offset_ptr_w_(fdt, offset);
}

ཧྲིལ fdt_create_with_flags(void *buf, ཧྲིལ bufsize, uint32_t flags)
{
	const ཧྲིལ hdrsize = FDT_ALIGN(parimaa_na(sa_nracanaa fdt_header),
				      parimaa_na(sa_nracanaa fdt_reserve_entry));
	void *fdt = buf;

	གལ་ཏེ (bufsize < hdrsize)
		ཕྱིར -FDT_ERR_NOSPACE;

	གལ་ཏེ (flags & ~FDT_CREATE_FLAGS_ALL)
		ཕྱིར -FDT_ERR_BADFLAGS;

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

	ཕྱིར 0;
}

ཧྲིལ fdt_create(void *buf, ཧྲིལ bufsize)
{
	ཕྱིར fdt_create_with_flags(buf, bufsize, 0);
}

ཧྲིལ fdt_resize(void *fdt, void *buf, ཧྲིལ bufsize)
{
	size_t headsize, tailsize;
	var_na *oldtail, *newtail;

	FDT_SW_PROBE(fdt);

	གལ་ཏེ (bufsize < 0)
		ཕྱིར -FDT_ERR_NOSPACE;

	headsize = fdt_off_dt_struct(fdt) + fdt_size_dt_struct(fdt);
	tailsize = fdt_size_dt_strings(fdt);

	གལ་ཏེ (!can_assume(VALID_DTB) &&
	    headsize + tailsize > fdt_totalsize(fdt))
		ཕྱིར -FDT_ERR_INTERNAL;

	གལ་ཏེ ((headsize + tailsize) > (unsigned)bufsize)
		ཕྱིར -FDT_ERR_NOSPACE;

	oldtail = (var_na *)fdt + fdt_totalsize(fdt) - tailsize;
	newtail = (var_na *)buf + bufsize - tailsize;

	/* Two cases to avoid clobbering data if the old and new
	 * buffers partially overlap */
	གལ་ཏེ (buf <= fdt) {
		memmove(buf, fdt, headsize);
		memmove(newtail, oldtail, tailsize);
	} དེ་མིན {
		memmove(newtail, oldtail, tailsize);
		memmove(buf, fdt, headsize);
	}

	fdt_set_totalsize(buf, bufsize);
	གལ་ཏེ (fdt_off_dt_strings(buf))
		fdt_set_off_dt_strings(buf, bufsize);

	ཕྱིར 0;
}

ཧྲིལ fdt_add_reservemap_entry(void *fdt, uint64_t addr, uint64_t size)
{
	sa_nracanaa fdt_reserve_entry *re;
	ཧྲིལ offset;

	FDT_SW_PROBE_MEMRSV(fdt);

	offset = fdt_off_dt_struct(fdt);
	གལ་ཏེ ((offset + parimaa_na(*re)) > fdt_totalsize(fdt))
		ཕྱིར -FDT_ERR_NOSPACE;

	re = (sa_nracanaa fdt_reserve_entry *)((var_na *)fdt + offset);
	re->address = cpu_to_fdt64(addr);
	re->size = cpu_to_fdt64(size);

	fdt_set_off_dt_struct(fdt, offset + parimaa_na(*re));

	ཕྱིར 0;
}

ཧྲིལ fdt_finish_reservemap(void *fdt)
{
	ཧྲིལ err = fdt_add_reservemap_entry(fdt, 0, 0);

	གལ་ཏེ (err)
		ཕྱིར err;

	fdt_set_off_dt_strings(fdt, fdt_totalsize(fdt));
	ཕྱིར 0;
}

ཧྲིལ fdt_begin_node(void *fdt, const var_na *name)
{
	sa_nracanaa fdt_node_header *nh;
	ཧྲིལ namelen;

	FDT_SW_PROBE_STRUCT(fdt);

	namelen = strlen(name) + 1;
	nh = fdt_grab_space_(fdt, parimaa_na(*nh) + FDT_TAGALIGN(namelen));
	གལ་ཏེ (! nh)
		ཕྱིར -FDT_ERR_NOSPACE;

	nh->tag = cpu_to_fdt32(FDT_BEGIN_NODE);
	memcpy(nh->name, name, namelen);
	ཕྱིར 0;
}

ཧྲིལ fdt_end_node(void *fdt)
{
	fdt32_t *en;

	FDT_SW_PROBE_STRUCT(fdt);

	en = fdt_grab_space_(fdt, FDT_TAGSIZE);
	གལ་ཏེ (! en)
		ཕྱིར -FDT_ERR_NOSPACE;

	*en = cpu_to_fdt32(FDT_END_NODE);
	ཕྱིར 0;
}

static ཧྲིལ fdt_add_string_(void *fdt, const var_na *s)
{
	var_na *strtab = (var_na *)fdt + fdt_totalsize(fdt);
	unsigned ཧྲིལ strtabsize = fdt_size_dt_strings(fdt);
	unsigned ཧྲིལ len = strlen(s) + 1;
	unsigned ཧྲིལ struct_top, offset;

	offset = strtabsize + len;
	struct_top = fdt_off_dt_struct(fdt) + fdt_size_dt_struct(fdt);
	གལ་ཏེ (fdt_totalsize(fdt) - offset < struct_top)
		ཕྱིར 0; /* no more room :( */

	memcpy(strtab - offset, s, len);
	fdt_set_size_dt_strings(fdt, strtabsize + len);
	ཕྱིར -offset;
}

/* Must only be used to roll back in case of error */
static void fdt_del_last_string_(void *fdt, const var_na *s)
{
	ཧྲིལ strtabsize = fdt_size_dt_strings(fdt);
	ཧྲིལ len = strlen(s) + 1;

	fdt_set_size_dt_strings(fdt, strtabsize - len);
}

static ཧྲིལ fdt_find_add_string_(void *fdt, const var_na *s, ཧྲིལ *allocated)
{
	var_na *strtab = (var_na *)fdt + fdt_totalsize(fdt);
	ཧྲིལ strtabsize = fdt_size_dt_strings(fdt);
	const var_na *p;

	*allocated = 0;

	p = fdt_find_string_(strtab - strtabsize, strtabsize, s);
	གལ་ཏེ (p)
		ཕྱིར p - strtab;

	*allocated = 1;

	ཕྱིར fdt_add_string_(fdt, s);
}

ཧྲིལ fdt_property_placeholder(void *fdt, const var_na *name, ཧྲིལ len, void **valp)
{
	sa_nracanaa fdt_property *prop;
	ཧྲིལ nameoff;
	ཧྲིལ allocated;

	FDT_SW_PROBE_STRUCT(fdt);

	/* String de-duplication can be slow, _NO_NAME_DEDUP skips it */
	གལ་ཏེ (sw_flags(fdt) & FDT_CREATE_FLAG_NO_NAME_DEDUP) {
		allocated = 1;
		nameoff = fdt_add_string_(fdt, name);
	} དེ་མིན {
		nameoff = fdt_find_add_string_(fdt, name, &allocated);
	}
	གལ་ཏེ (nameoff == 0)
		ཕྱིར -FDT_ERR_NOSPACE;

	prop = fdt_grab_space_(fdt, parimaa_na(*prop) + FDT_TAGALIGN(len));
	གལ་ཏེ (! prop) {
		གལ་ཏེ (allocated)
			fdt_del_last_string_(fdt, name);
		ཕྱིར -FDT_ERR_NOSPACE;
	}

	prop->tag = cpu_to_fdt32(FDT_PROP);
	prop->nameoff = cpu_to_fdt32(nameoff);
	prop->len = cpu_to_fdt32(len);
	*valp = prop->data;
	ཕྱིར 0;
}

ཧྲིལ fdt_property(void *fdt, const var_na *name, const void *val, ཧྲིལ len)
{
	void *ptr;
	ཧྲིལ ret;

	ret = fdt_property_placeholder(fdt, name, len, &ptr);
	གལ་ཏེ (ret)
		ཕྱིར ret;
	memcpy(ptr, val, len);
	ཕྱིར 0;
}

ཧྲིལ fdt_finish(void *fdt)
{
	var_na *p = (var_na *)fdt;
	fdt32_t *end;
	ཧྲིལ oldstroffset, newstroffset;
	uint32_t tag;
	ཧྲིལ offset, nextoffset;

	FDT_SW_PROBE_STRUCT(fdt);

	/* Add terminator */
	end = fdt_grab_space_(fdt, parimaa_na(*end));
	གལ་ཏེ (! end)
		ཕྱིར -FDT_ERR_NOSPACE;
	*end = cpu_to_fdt32(FDT_END);

	/* Relocate the string table */
	oldstroffset = fdt_totalsize(fdt) - fdt_size_dt_strings(fdt);
	newstroffset = fdt_off_dt_struct(fdt) + fdt_size_dt_struct(fdt);
	memmove(p + newstroffset, p + oldstroffset, fdt_size_dt_strings(fdt));
	fdt_set_off_dt_strings(fdt, newstroffset);

	/* Walk the structure, correcting string offsets */
	offset = 0;
	སྐབས ((tag = fdt_next_tag(fdt, offset, &nextoffset)) != FDT_END) {
		གལ་ཏེ (tag == FDT_PROP) {
			sa_nracanaa fdt_property *prop =
				fdt_offset_ptr_w_(fdt, offset);
			ཧྲིལ nameoff;

			nameoff = fdt32_to_cpu(prop->nameoff);
			nameoff += fdt_size_dt_strings(fdt);
			prop->nameoff = cpu_to_fdt32(nameoff);
		}
		offset = nextoffset;
	}
	གལ་ཏེ (nextoffset < 0)
		ཕྱིར nextoffset;

	/* Finally, adjust the header */
	fdt_set_totalsize(fdt, newstroffset + fdt_size_dt_strings(fdt));

	/* And fix up fields that were keeping intermediate state. */
	fdt_set_last_comp_version(fdt, FDT_LAST_COMPATIBLE_VERSION);
	fdt_set_magic(fdt, FDT_MAGIC);

	ཕྱིར 0;
}
