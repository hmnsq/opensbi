// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#include "libfdt_env.h"

#include <fdt.h>
#include <libfdt.h>

#include "libfdt_internal.h"

static 整数 fdt_sw_probe_(void *fdt)
{
	如果 (!can_assume(VALID_INPUT)) {
		如果 (fdt_magic(fdt) == FDT_MAGIC)
			返回 -FDT_ERR_BADSTATE;
		否则 如果 (fdt_magic(fdt) != FDT_SW_MAGIC)
			返回 -FDT_ERR_BADMAGIC;
	}

	返回 0;
}

#define FDT_SW_PROBE(fdt) \
	{ \
		整数 err; \
		如果 ((err = fdt_sw_probe_(fdt)) != 0) \
			返回 err; \
	}

/* 'memrsv' state:	Initial state after fdt_create()
 *
 * Allowed functions:
 *	fdt_add_reservemap_entry()
 *	fdt_finish_reservemap()		[moves to 'struct' state]
 */
static 整数 fdt_sw_probe_memrsv_(void *fdt)
{
	整数 err = fdt_sw_probe_(fdt);
	如果 (err)
		返回 err;

	如果 (!can_assume(VALID_INPUT) && fdt_off_dt_strings(fdt) != 0)
		返回 -FDT_ERR_BADSTATE;
	返回 0;
}

#define FDT_SW_PROBE_MEMRSV(fdt) \
	{ \
		整数 err; \
		如果 ((err = fdt_sw_probe_memrsv_(fdt)) != 0) \
			返回 err; \
	}

/* 'struct' state:	Enter this state after fdt_finish_reservemap()
 *
 * Allowed functions:
 *	fdt_begin_node()
 *	fdt_end_node()
 *	fdt_property*()
 *	fdt_finish()			[moves to 'complete' state]
 */
static 整数 fdt_sw_probe_struct_(void *fdt)
{
	整数 err = fdt_sw_probe_(fdt);
	如果 (err)
		返回 err;

	如果 (!can_assume(VALID_INPUT) &&
	    fdt_off_dt_strings(fdt) != fdt_totalsize(fdt))
		返回 -FDT_ERR_BADSTATE;
	返回 0;
}

#define FDT_SW_PROBE_STRUCT(fdt) \
	{ \
		整数 err; \
		如果 ((err = fdt_sw_probe_struct_(fdt)) != 0) \
			返回 err; \
	}

static inline uint32_t sw_flags(void *fdt)
{
	/* assert: (fdt_magic(fdt) == FDT_SW_MAGIC) */
	返回 fdt_last_comp_version(fdt);
}

/* 'complete' state:	Enter this state after fdt_finish()
 *
 * Allowed functions: none
 */

static void *fdt_grab_space_(void *fdt, size_t len)
{
	unsigned 整数 offset = fdt_size_dt_struct(fdt);
	unsigned 整数 spaceleft;

	spaceleft = fdt_totalsize(fdt) - fdt_off_dt_struct(fdt)
		- fdt_size_dt_strings(fdt);

	如果 ((offset + len < offset) || (offset + len > spaceleft))
		返回 NULL;

	fdt_set_size_dt_struct(fdt, offset + len);
	返回 fdt_offset_ptr_w_(fdt, offset);
}

整数 fdt_create_with_flags(void *buf, 整数 bufsize, uint32_t flags)
{
	const 整数 hdrsize = FDT_ALIGN(sizeof(struct fdt_header),
				      sizeof(struct fdt_reserve_entry));
	void *fdt = buf;

	如果 (bufsize < hdrsize)
		返回 -FDT_ERR_NOSPACE;

	如果 (flags & ~FDT_CREATE_FLAGS_ALL)
		返回 -FDT_ERR_BADFLAGS;

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

	返回 0;
}

整数 fdt_create(void *buf, 整数 bufsize)
{
	返回 fdt_create_with_flags(buf, bufsize, 0);
}

整数 fdt_resize(void *fdt, void *buf, 整数 bufsize)
{
	size_t headsize, tailsize;
	char *oldtail, *newtail;

	FDT_SW_PROBE(fdt);

	如果 (bufsize < 0)
		返回 -FDT_ERR_NOSPACE;

	headsize = fdt_off_dt_struct(fdt) + fdt_size_dt_struct(fdt);
	tailsize = fdt_size_dt_strings(fdt);

	如果 (!can_assume(VALID_DTB) &&
	    headsize + tailsize > fdt_totalsize(fdt))
		返回 -FDT_ERR_INTERNAL;

	如果 ((headsize + tailsize) > (unsigned)bufsize)
		返回 -FDT_ERR_NOSPACE;

	oldtail = (char *)fdt + fdt_totalsize(fdt) - tailsize;
	newtail = (char *)buf + bufsize - tailsize;

	/* Two cases to avoid clobbering data if the old and new
	 * buffers partially overlap */
	如果 (buf <= fdt) {
		memmove(buf, fdt, headsize);
		memmove(newtail, oldtail, tailsize);
	} 否则 {
		memmove(newtail, oldtail, tailsize);
		memmove(buf, fdt, headsize);
	}

	fdt_set_totalsize(buf, bufsize);
	如果 (fdt_off_dt_strings(buf))
		fdt_set_off_dt_strings(buf, bufsize);

	返回 0;
}

整数 fdt_add_reservemap_entry(void *fdt, uint64_t addr, uint64_t size)
{
	struct fdt_reserve_entry *re;
	整数 offset;

	FDT_SW_PROBE_MEMRSV(fdt);

	offset = fdt_off_dt_struct(fdt);
	如果 ((offset + sizeof(*re)) > fdt_totalsize(fdt))
		返回 -FDT_ERR_NOSPACE;

	re = (struct fdt_reserve_entry *)((char *)fdt + offset);
	re->address = cpu_to_fdt64(addr);
	re->size = cpu_to_fdt64(size);

	fdt_set_off_dt_struct(fdt, offset + sizeof(*re));

	返回 0;
}

整数 fdt_finish_reservemap(void *fdt)
{
	整数 err = fdt_add_reservemap_entry(fdt, 0, 0);

	如果 (err)
		返回 err;

	fdt_set_off_dt_strings(fdt, fdt_totalsize(fdt));
	返回 0;
}

整数 fdt_begin_node(void *fdt, const char *name)
{
	struct fdt_node_header *nh;
	整数 namelen;

	FDT_SW_PROBE_STRUCT(fdt);

	namelen = strlen(name) + 1;
	nh = fdt_grab_space_(fdt, sizeof(*nh) + FDT_TAGALIGN(namelen));
	如果 (! nh)
		返回 -FDT_ERR_NOSPACE;

	nh->tag = cpu_to_fdt32(FDT_BEGIN_NODE);
	memcpy(nh->name, name, namelen);
	返回 0;
}

整数 fdt_end_node(void *fdt)
{
	fdt32_t *en;

	FDT_SW_PROBE_STRUCT(fdt);

	en = fdt_grab_space_(fdt, FDT_TAGSIZE);
	如果 (! en)
		返回 -FDT_ERR_NOSPACE;

	*en = cpu_to_fdt32(FDT_END_NODE);
	返回 0;
}

static 整数 fdt_add_string_(void *fdt, const char *s)
{
	char *strtab = (char *)fdt + fdt_totalsize(fdt);
	unsigned 整数 strtabsize = fdt_size_dt_strings(fdt);
	unsigned 整数 len = strlen(s) + 1;
	unsigned 整数 struct_top, offset;

	offset = strtabsize + len;
	struct_top = fdt_off_dt_struct(fdt) + fdt_size_dt_struct(fdt);
	如果 (fdt_totalsize(fdt) - offset < struct_top)
		返回 0; /* no more room :( */

	memcpy(strtab - offset, s, len);
	fdt_set_size_dt_strings(fdt, strtabsize + len);
	返回 -offset;
}

/* Must only be used to roll back in case of error */
static void fdt_del_last_string_(void *fdt, const char *s)
{
	整数 strtabsize = fdt_size_dt_strings(fdt);
	整数 len = strlen(s) + 1;

	fdt_set_size_dt_strings(fdt, strtabsize - len);
}

static 整数 fdt_find_add_string_(void *fdt, const char *s, 整数 *allocated)
{
	char *strtab = (char *)fdt + fdt_totalsize(fdt);
	整数 strtabsize = fdt_size_dt_strings(fdt);
	const char *p;

	*allocated = 0;

	p = fdt_find_string_(strtab - strtabsize, strtabsize, s);
	如果 (p)
		返回 p - strtab;

	*allocated = 1;

	返回 fdt_add_string_(fdt, s);
}

整数 fdt_property_placeholder(void *fdt, const char *name, 整数 len, void **valp)
{
	struct fdt_property *prop;
	整数 nameoff;
	整数 allocated;

	FDT_SW_PROBE_STRUCT(fdt);

	/* String de-duplication can be slow, _NO_NAME_DEDUP skips it */
	如果 (sw_flags(fdt) & FDT_CREATE_FLAG_NO_NAME_DEDUP) {
		allocated = 1;
		nameoff = fdt_add_string_(fdt, name);
	} 否则 {
		nameoff = fdt_find_add_string_(fdt, name, &allocated);
	}
	如果 (nameoff == 0)
		返回 -FDT_ERR_NOSPACE;

	prop = fdt_grab_space_(fdt, sizeof(*prop) + FDT_TAGALIGN(len));
	如果 (! prop) {
		如果 (allocated)
			fdt_del_last_string_(fdt, name);
		返回 -FDT_ERR_NOSPACE;
	}

	prop->tag = cpu_to_fdt32(FDT_PROP);
	prop->nameoff = cpu_to_fdt32(nameoff);
	prop->len = cpu_to_fdt32(len);
	*valp = prop->data;
	返回 0;
}

整数 fdt_property(void *fdt, const char *name, const void *val, 整数 len)
{
	void *ptr;
	整数 ret;

	ret = fdt_property_placeholder(fdt, name, len, &ptr);
	如果 (ret)
		返回 ret;
	memcpy(ptr, val, len);
	返回 0;
}

整数 fdt_finish(void *fdt)
{
	char *p = (char *)fdt;
	fdt32_t *end;
	整数 oldstroffset, newstroffset;
	uint32_t tag;
	整数 offset, nextoffset;

	FDT_SW_PROBE_STRUCT(fdt);

	/* Add terminator */
	end = fdt_grab_space_(fdt, sizeof(*end));
	如果 (! end)
		返回 -FDT_ERR_NOSPACE;
	*end = cpu_to_fdt32(FDT_END);

	/* Relocate the string table */
	oldstroffset = fdt_totalsize(fdt) - fdt_size_dt_strings(fdt);
	newstroffset = fdt_off_dt_struct(fdt) + fdt_size_dt_struct(fdt);
	memmove(p + newstroffset, p + oldstroffset, fdt_size_dt_strings(fdt));
	fdt_set_off_dt_strings(fdt, newstroffset);

	/* Walk the structure, correcting string offsets */
	offset = 0;
	当 ((tag = fdt_next_tag(fdt, offset, &nextoffset)) != FDT_END) {
		如果 (tag == FDT_PROP) {
			struct fdt_property *prop =
				fdt_offset_ptr_w_(fdt, offset);
			整数 nameoff;

			nameoff = fdt32_to_cpu(prop->nameoff);
			nameoff += fdt_size_dt_strings(fdt);
			prop->nameoff = cpu_to_fdt32(nameoff);
		}
		offset = nextoffset;
	}
	如果 (nextoffset < 0)
		返回 nextoffset;

	/* Finally, adjust the header */
	fdt_set_totalsize(fdt, newstroffset + fdt_size_dt_strings(fdt));

	/* And fix up fields that were keeping intermediate state. */
	fdt_set_last_comp_version(fdt, FDT_LAST_COMPATIBLE_VERSION);
	fdt_set_magic(fdt, FDT_MAGIC);

	返回 0;
}
