// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#include "libfdt_env.h"

#include <fdt.h>
#include <libfdt.h>

#include "libfdt_internal.h"

static 정수 fdt_sw_probe_(void *fdt)
{
	만약 (!can_assume(VALID_INPUT)) {
		만약 (fdt_magic(fdt) == FDT_MAGIC)
			반환 -FDT_ERR_BADSTATE;
		아니면 만약 (fdt_magic(fdt) != FDT_SW_MAGIC)
			반환 -FDT_ERR_BADMAGIC;
	}

	반환 0;
}

#define FDT_SW_PROBE(fdt) \
	{ \
		정수 err; \
		만약 ((err = fdt_sw_probe_(fdt)) != 0) \
			반환 err; \
	}

/* 'memrsv' state:	Initial state after fdt_create()
 *
 * Allowed functions:
 *	fdt_add_reservemap_entry()
 *	fdt_finish_reservemap()		[moves to 'struct' state]
 */
static 정수 fdt_sw_probe_memrsv_(void *fdt)
{
	정수 err = fdt_sw_probe_(fdt);
	만약 (err)
		반환 err;

	만약 (!can_assume(VALID_INPUT) && fdt_off_dt_strings(fdt) != 0)
		반환 -FDT_ERR_BADSTATE;
	반환 0;
}

#define FDT_SW_PROBE_MEMRSV(fdt) \
	{ \
		정수 err; \
		만약 ((err = fdt_sw_probe_memrsv_(fdt)) != 0) \
			반환 err; \
	}

/* 'struct' state:	Enter this state after fdt_finish_reservemap()
 *
 * Allowed functions:
 *	fdt_begin_node()
 *	fdt_end_node()
 *	fdt_property*()
 *	fdt_finish()			[moves to 'complete' state]
 */
static 정수 fdt_sw_probe_struct_(void *fdt)
{
	정수 err = fdt_sw_probe_(fdt);
	만약 (err)
		반환 err;

	만약 (!can_assume(VALID_INPUT) &&
	    fdt_off_dt_strings(fdt) != fdt_totalsize(fdt))
		반환 -FDT_ERR_BADSTATE;
	반환 0;
}

#define FDT_SW_PROBE_STRUCT(fdt) \
	{ \
		정수 err; \
		만약 ((err = fdt_sw_probe_struct_(fdt)) != 0) \
			반환 err; \
	}

static inline uint32_t sw_flags(void *fdt)
{
	/* assert: (fdt_magic(fdt) == FDT_SW_MAGIC) */
	반환 fdt_last_comp_version(fdt);
}

/* 'complete' state:	Enter this state after fdt_finish()
 *
 * Allowed functions: none
 */

static void *fdt_grab_space_(void *fdt, size_t len)
{
	unsigned 정수 offset = fdt_size_dt_struct(fdt);
	unsigned 정수 spaceleft;

	spaceleft = fdt_totalsize(fdt) - fdt_off_dt_struct(fdt)
		- fdt_size_dt_strings(fdt);

	만약 ((offset + len < offset) || (offset + len > spaceleft))
		반환 NULL;

	fdt_set_size_dt_struct(fdt, offset + len);
	반환 fdt_offset_ptr_w_(fdt, offset);
}

정수 fdt_create_with_flags(void *buf, 정수 bufsize, uint32_t flags)
{
	const 정수 hdrsize = FDT_ALIGN(sizeof(struct fdt_header),
				      sizeof(struct fdt_reserve_entry));
	void *fdt = buf;

	만약 (bufsize < hdrsize)
		반환 -FDT_ERR_NOSPACE;

	만약 (flags & ~FDT_CREATE_FLAGS_ALL)
		반환 -FDT_ERR_BADFLAGS;

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

	반환 0;
}

정수 fdt_create(void *buf, 정수 bufsize)
{
	반환 fdt_create_with_flags(buf, bufsize, 0);
}

정수 fdt_resize(void *fdt, void *buf, 정수 bufsize)
{
	size_t headsize, tailsize;
	char *oldtail, *newtail;

	FDT_SW_PROBE(fdt);

	만약 (bufsize < 0)
		반환 -FDT_ERR_NOSPACE;

	headsize = fdt_off_dt_struct(fdt) + fdt_size_dt_struct(fdt);
	tailsize = fdt_size_dt_strings(fdt);

	만약 (!can_assume(VALID_DTB) &&
	    headsize + tailsize > fdt_totalsize(fdt))
		반환 -FDT_ERR_INTERNAL;

	만약 ((headsize + tailsize) > (unsigned)bufsize)
		반환 -FDT_ERR_NOSPACE;

	oldtail = (char *)fdt + fdt_totalsize(fdt) - tailsize;
	newtail = (char *)buf + bufsize - tailsize;

	/* Two cases to avoid clobbering data if the old and new
	 * buffers partially overlap */
	만약 (buf <= fdt) {
		memmove(buf, fdt, headsize);
		memmove(newtail, oldtail, tailsize);
	} 아니면 {
		memmove(newtail, oldtail, tailsize);
		memmove(buf, fdt, headsize);
	}

	fdt_set_totalsize(buf, bufsize);
	만약 (fdt_off_dt_strings(buf))
		fdt_set_off_dt_strings(buf, bufsize);

	반환 0;
}

정수 fdt_add_reservemap_entry(void *fdt, uint64_t addr, uint64_t size)
{
	struct fdt_reserve_entry *re;
	정수 offset;

	FDT_SW_PROBE_MEMRSV(fdt);

	offset = fdt_off_dt_struct(fdt);
	만약 ((offset + sizeof(*re)) > fdt_totalsize(fdt))
		반환 -FDT_ERR_NOSPACE;

	re = (struct fdt_reserve_entry *)((char *)fdt + offset);
	re->address = cpu_to_fdt64(addr);
	re->size = cpu_to_fdt64(size);

	fdt_set_off_dt_struct(fdt, offset + sizeof(*re));

	반환 0;
}

정수 fdt_finish_reservemap(void *fdt)
{
	정수 err = fdt_add_reservemap_entry(fdt, 0, 0);

	만약 (err)
		반환 err;

	fdt_set_off_dt_strings(fdt, fdt_totalsize(fdt));
	반환 0;
}

정수 fdt_begin_node(void *fdt, const char *name)
{
	struct fdt_node_header *nh;
	정수 namelen;

	FDT_SW_PROBE_STRUCT(fdt);

	namelen = strlen(name) + 1;
	nh = fdt_grab_space_(fdt, sizeof(*nh) + FDT_TAGALIGN(namelen));
	만약 (! nh)
		반환 -FDT_ERR_NOSPACE;

	nh->tag = cpu_to_fdt32(FDT_BEGIN_NODE);
	memcpy(nh->name, name, namelen);
	반환 0;
}

정수 fdt_end_node(void *fdt)
{
	fdt32_t *en;

	FDT_SW_PROBE_STRUCT(fdt);

	en = fdt_grab_space_(fdt, FDT_TAGSIZE);
	만약 (! en)
		반환 -FDT_ERR_NOSPACE;

	*en = cpu_to_fdt32(FDT_END_NODE);
	반환 0;
}

static 정수 fdt_add_string_(void *fdt, const char *s)
{
	char *strtab = (char *)fdt + fdt_totalsize(fdt);
	unsigned 정수 strtabsize = fdt_size_dt_strings(fdt);
	unsigned 정수 len = strlen(s) + 1;
	unsigned 정수 struct_top, offset;

	offset = strtabsize + len;
	struct_top = fdt_off_dt_struct(fdt) + fdt_size_dt_struct(fdt);
	만약 (fdt_totalsize(fdt) - offset < struct_top)
		반환 0; /* no more room :( */

	memcpy(strtab - offset, s, len);
	fdt_set_size_dt_strings(fdt, strtabsize + len);
	반환 -offset;
}

/* Must only be used to roll back in case of error */
static void fdt_del_last_string_(void *fdt, const char *s)
{
	정수 strtabsize = fdt_size_dt_strings(fdt);
	정수 len = strlen(s) + 1;

	fdt_set_size_dt_strings(fdt, strtabsize - len);
}

static 정수 fdt_find_add_string_(void *fdt, const char *s, 정수 *allocated)
{
	char *strtab = (char *)fdt + fdt_totalsize(fdt);
	정수 strtabsize = fdt_size_dt_strings(fdt);
	const char *p;

	*allocated = 0;

	p = fdt_find_string_(strtab - strtabsize, strtabsize, s);
	만약 (p)
		반환 p - strtab;

	*allocated = 1;

	반환 fdt_add_string_(fdt, s);
}

정수 fdt_property_placeholder(void *fdt, const char *name, 정수 len, void **valp)
{
	struct fdt_property *prop;
	정수 nameoff;
	정수 allocated;

	FDT_SW_PROBE_STRUCT(fdt);

	/* String de-duplication can be slow, _NO_NAME_DEDUP skips it */
	만약 (sw_flags(fdt) & FDT_CREATE_FLAG_NO_NAME_DEDUP) {
		allocated = 1;
		nameoff = fdt_add_string_(fdt, name);
	} 아니면 {
		nameoff = fdt_find_add_string_(fdt, name, &allocated);
	}
	만약 (nameoff == 0)
		반환 -FDT_ERR_NOSPACE;

	prop = fdt_grab_space_(fdt, sizeof(*prop) + FDT_TAGALIGN(len));
	만약 (! prop) {
		만약 (allocated)
			fdt_del_last_string_(fdt, name);
		반환 -FDT_ERR_NOSPACE;
	}

	prop->tag = cpu_to_fdt32(FDT_PROP);
	prop->nameoff = cpu_to_fdt32(nameoff);
	prop->len = cpu_to_fdt32(len);
	*valp = prop->data;
	반환 0;
}

정수 fdt_property(void *fdt, const char *name, const void *val, 정수 len)
{
	void *ptr;
	정수 ret;

	ret = fdt_property_placeholder(fdt, name, len, &ptr);
	만약 (ret)
		반환 ret;
	memcpy(ptr, val, len);
	반환 0;
}

정수 fdt_finish(void *fdt)
{
	char *p = (char *)fdt;
	fdt32_t *end;
	정수 oldstroffset, newstroffset;
	uint32_t tag;
	정수 offset, nextoffset;

	FDT_SW_PROBE_STRUCT(fdt);

	/* Add terminator */
	end = fdt_grab_space_(fdt, sizeof(*end));
	만약 (! end)
		반환 -FDT_ERR_NOSPACE;
	*end = cpu_to_fdt32(FDT_END);

	/* Relocate the string table */
	oldstroffset = fdt_totalsize(fdt) - fdt_size_dt_strings(fdt);
	newstroffset = fdt_off_dt_struct(fdt) + fdt_size_dt_struct(fdt);
	memmove(p + newstroffset, p + oldstroffset, fdt_size_dt_strings(fdt));
	fdt_set_off_dt_strings(fdt, newstroffset);

	/* Walk the structure, correcting string offsets */
	offset = 0;
	동안 ((tag = fdt_next_tag(fdt, offset, &nextoffset)) != FDT_END) {
		만약 (tag == FDT_PROP) {
			struct fdt_property *prop =
				fdt_offset_ptr_w_(fdt, offset);
			정수 nameoff;

			nameoff = fdt32_to_cpu(prop->nameoff);
			nameoff += fdt_size_dt_strings(fdt);
			prop->nameoff = cpu_to_fdt32(nameoff);
		}
		offset = nextoffset;
	}
	만약 (nextoffset < 0)
		반환 nextoffset;

	/* Finally, adjust the header */
	fdt_set_totalsize(fdt, newstroffset + fdt_size_dt_strings(fdt));

	/* And fix up fields that were keeping intermediate state. */
	fdt_set_last_comp_version(fdt, FDT_LAST_COMPATIBLE_VERSION);
	fdt_set_magic(fdt, FDT_MAGIC);

	반환 0;
}
