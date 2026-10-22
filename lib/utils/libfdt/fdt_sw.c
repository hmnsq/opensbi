// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#include "libfdt_env.h"

#include <fdt.h>
#include <libfdt.h>

#include "libfdt_internal.h"

static số_nguyên fdt_sw_probe_(void *fdt)
{
	nếu (!can_assume(VALID_INPUT)) {
		nếu (fdt_magic(fdt) == FDT_MAGIC)
			trả_về -FDT_ERR_BADSTATE;
		không_thì nếu (fdt_magic(fdt) != FDT_SW_MAGIC)
			trả_về -FDT_ERR_BADMAGIC;
	}

	trả_về 0;
}

#define FDT_SW_PROBE(fdt) \
	{ \
		số_nguyên err; \
		nếu ((err = fdt_sw_probe_(fdt)) != 0) \
			trả_về err; \
	}

/* 'memrsv' state:	Initial state after fdt_create()
 *
 * Allowed functions:
 *	fdt_add_reservemap_entry()
 *	fdt_finish_reservemap()		[moves to 'struct' state]
 */
static số_nguyên fdt_sw_probe_memrsv_(void *fdt)
{
	số_nguyên err = fdt_sw_probe_(fdt);
	nếu (err)
		trả_về err;

	nếu (!can_assume(VALID_INPUT) && fdt_off_dt_strings(fdt) != 0)
		trả_về -FDT_ERR_BADSTATE;
	trả_về 0;
}

#define FDT_SW_PROBE_MEMRSV(fdt) \
	{ \
		số_nguyên err; \
		nếu ((err = fdt_sw_probe_memrsv_(fdt)) != 0) \
			trả_về err; \
	}

/* 'struct' state:	Enter this state after fdt_finish_reservemap()
 *
 * Allowed functions:
 *	fdt_begin_node()
 *	fdt_end_node()
 *	fdt_property*()
 *	fdt_finish()			[moves to 'complete' state]
 */
static số_nguyên fdt_sw_probe_struct_(void *fdt)
{
	số_nguyên err = fdt_sw_probe_(fdt);
	nếu (err)
		trả_về err;

	nếu (!can_assume(VALID_INPUT) &&
	    fdt_off_dt_strings(fdt) != fdt_totalsize(fdt))
		trả_về -FDT_ERR_BADSTATE;
	trả_về 0;
}

#define FDT_SW_PROBE_STRUCT(fdt) \
	{ \
		số_nguyên err; \
		nếu ((err = fdt_sw_probe_struct_(fdt)) != 0) \
			trả_về err; \
	}

static inline uint32_t sw_flags(void *fdt)
{
	/* assert: (fdt_magic(fdt) == FDT_SW_MAGIC) */
	trả_về fdt_last_comp_version(fdt);
}

/* 'complete' state:	Enter this state after fdt_finish()
 *
 * Allowed functions: none
 */

static void *fdt_grab_space_(void *fdt, size_t len)
{
	unsigned số_nguyên offset = fdt_size_dt_struct(fdt);
	unsigned số_nguyên spaceleft;

	spaceleft = fdt_totalsize(fdt) - fdt_off_dt_struct(fdt)
		- fdt_size_dt_strings(fdt);

	nếu ((offset + len < offset) || (offset + len > spaceleft))
		trả_về NULL;

	fdt_set_size_dt_struct(fdt, offset + len);
	trả_về fdt_offset_ptr_w_(fdt, offset);
}

số_nguyên fdt_create_with_flags(void *buf, số_nguyên bufsize, uint32_t flags)
{
	const số_nguyên hdrsize = FDT_ALIGN(sizeof(struct fdt_header),
				      sizeof(struct fdt_reserve_entry));
	void *fdt = buf;

	nếu (bufsize < hdrsize)
		trả_về -FDT_ERR_NOSPACE;

	nếu (flags & ~FDT_CREATE_FLAGS_ALL)
		trả_về -FDT_ERR_BADFLAGS;

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

	trả_về 0;
}

số_nguyên fdt_create(void *buf, số_nguyên bufsize)
{
	trả_về fdt_create_with_flags(buf, bufsize, 0);
}

số_nguyên fdt_resize(void *fdt, void *buf, số_nguyên bufsize)
{
	size_t headsize, tailsize;
	char *oldtail, *newtail;

	FDT_SW_PROBE(fdt);

	nếu (bufsize < 0)
		trả_về -FDT_ERR_NOSPACE;

	headsize = fdt_off_dt_struct(fdt) + fdt_size_dt_struct(fdt);
	tailsize = fdt_size_dt_strings(fdt);

	nếu (!can_assume(VALID_DTB) &&
	    headsize + tailsize > fdt_totalsize(fdt))
		trả_về -FDT_ERR_INTERNAL;

	nếu ((headsize + tailsize) > (unsigned)bufsize)
		trả_về -FDT_ERR_NOSPACE;

	oldtail = (char *)fdt + fdt_totalsize(fdt) - tailsize;
	newtail = (char *)buf + bufsize - tailsize;

	/* Two cases to avoid clobbering data if the old and new
	 * buffers partially overlap */
	nếu (buf <= fdt) {
		memmove(buf, fdt, headsize);
		memmove(newtail, oldtail, tailsize);
	} không_thì {
		memmove(newtail, oldtail, tailsize);
		memmove(buf, fdt, headsize);
	}

	fdt_set_totalsize(buf, bufsize);
	nếu (fdt_off_dt_strings(buf))
		fdt_set_off_dt_strings(buf, bufsize);

	trả_về 0;
}

số_nguyên fdt_add_reservemap_entry(void *fdt, uint64_t addr, uint64_t size)
{
	struct fdt_reserve_entry *re;
	số_nguyên offset;

	FDT_SW_PROBE_MEMRSV(fdt);

	offset = fdt_off_dt_struct(fdt);
	nếu ((offset + sizeof(*re)) > fdt_totalsize(fdt))
		trả_về -FDT_ERR_NOSPACE;

	re = (struct fdt_reserve_entry *)((char *)fdt + offset);
	re->address = cpu_to_fdt64(addr);
	re->size = cpu_to_fdt64(size);

	fdt_set_off_dt_struct(fdt, offset + sizeof(*re));

	trả_về 0;
}

số_nguyên fdt_finish_reservemap(void *fdt)
{
	số_nguyên err = fdt_add_reservemap_entry(fdt, 0, 0);

	nếu (err)
		trả_về err;

	fdt_set_off_dt_strings(fdt, fdt_totalsize(fdt));
	trả_về 0;
}

số_nguyên fdt_begin_node(void *fdt, const char *name)
{
	struct fdt_node_header *nh;
	số_nguyên namelen;

	FDT_SW_PROBE_STRUCT(fdt);

	namelen = strlen(name) + 1;
	nh = fdt_grab_space_(fdt, sizeof(*nh) + FDT_TAGALIGN(namelen));
	nếu (! nh)
		trả_về -FDT_ERR_NOSPACE;

	nh->tag = cpu_to_fdt32(FDT_BEGIN_NODE);
	memcpy(nh->name, name, namelen);
	trả_về 0;
}

số_nguyên fdt_end_node(void *fdt)
{
	fdt32_t *en;

	FDT_SW_PROBE_STRUCT(fdt);

	en = fdt_grab_space_(fdt, FDT_TAGSIZE);
	nếu (! en)
		trả_về -FDT_ERR_NOSPACE;

	*en = cpu_to_fdt32(FDT_END_NODE);
	trả_về 0;
}

static số_nguyên fdt_add_string_(void *fdt, const char *s)
{
	char *strtab = (char *)fdt + fdt_totalsize(fdt);
	unsigned số_nguyên strtabsize = fdt_size_dt_strings(fdt);
	unsigned số_nguyên len = strlen(s) + 1;
	unsigned số_nguyên struct_top, offset;

	offset = strtabsize + len;
	struct_top = fdt_off_dt_struct(fdt) + fdt_size_dt_struct(fdt);
	nếu (fdt_totalsize(fdt) - offset < struct_top)
		trả_về 0; /* no more room :( */

	memcpy(strtab - offset, s, len);
	fdt_set_size_dt_strings(fdt, strtabsize + len);
	trả_về -offset;
}

/* Must only be used to roll back in case of error */
static void fdt_del_last_string_(void *fdt, const char *s)
{
	số_nguyên strtabsize = fdt_size_dt_strings(fdt);
	số_nguyên len = strlen(s) + 1;

	fdt_set_size_dt_strings(fdt, strtabsize - len);
}

static số_nguyên fdt_find_add_string_(void *fdt, const char *s, số_nguyên *allocated)
{
	char *strtab = (char *)fdt + fdt_totalsize(fdt);
	số_nguyên strtabsize = fdt_size_dt_strings(fdt);
	const char *p;

	*allocated = 0;

	p = fdt_find_string_(strtab - strtabsize, strtabsize, s);
	nếu (p)
		trả_về p - strtab;

	*allocated = 1;

	trả_về fdt_add_string_(fdt, s);
}

số_nguyên fdt_property_placeholder(void *fdt, const char *name, số_nguyên len, void **valp)
{
	struct fdt_property *prop;
	số_nguyên nameoff;
	số_nguyên allocated;

	FDT_SW_PROBE_STRUCT(fdt);

	/* String de-duplication can be slow, _NO_NAME_DEDUP skips it */
	nếu (sw_flags(fdt) & FDT_CREATE_FLAG_NO_NAME_DEDUP) {
		allocated = 1;
		nameoff = fdt_add_string_(fdt, name);
	} không_thì {
		nameoff = fdt_find_add_string_(fdt, name, &allocated);
	}
	nếu (nameoff == 0)
		trả_về -FDT_ERR_NOSPACE;

	prop = fdt_grab_space_(fdt, sizeof(*prop) + FDT_TAGALIGN(len));
	nếu (! prop) {
		nếu (allocated)
			fdt_del_last_string_(fdt, name);
		trả_về -FDT_ERR_NOSPACE;
	}

	prop->tag = cpu_to_fdt32(FDT_PROP);
	prop->nameoff = cpu_to_fdt32(nameoff);
	prop->len = cpu_to_fdt32(len);
	*valp = prop->data;
	trả_về 0;
}

số_nguyên fdt_property(void *fdt, const char *name, const void *val, số_nguyên len)
{
	void *ptr;
	số_nguyên ret;

	ret = fdt_property_placeholder(fdt, name, len, &ptr);
	nếu (ret)
		trả_về ret;
	memcpy(ptr, val, len);
	trả_về 0;
}

số_nguyên fdt_finish(void *fdt)
{
	char *p = (char *)fdt;
	fdt32_t *end;
	số_nguyên oldstroffset, newstroffset;
	uint32_t tag;
	số_nguyên offset, nextoffset;

	FDT_SW_PROBE_STRUCT(fdt);

	/* Add terminator */
	end = fdt_grab_space_(fdt, sizeof(*end));
	nếu (! end)
		trả_về -FDT_ERR_NOSPACE;
	*end = cpu_to_fdt32(FDT_END);

	/* Relocate the string table */
	oldstroffset = fdt_totalsize(fdt) - fdt_size_dt_strings(fdt);
	newstroffset = fdt_off_dt_struct(fdt) + fdt_size_dt_struct(fdt);
	memmove(p + newstroffset, p + oldstroffset, fdt_size_dt_strings(fdt));
	fdt_set_off_dt_strings(fdt, newstroffset);

	/* Walk the structure, correcting string offsets */
	offset = 0;
	trong_khi ((tag = fdt_next_tag(fdt, offset, &nextoffset)) != FDT_END) {
		nếu (tag == FDT_PROP) {
			struct fdt_property *prop =
				fdt_offset_ptr_w_(fdt, offset);
			số_nguyên nameoff;

			nameoff = fdt32_to_cpu(prop->nameoff);
			nameoff += fdt_size_dt_strings(fdt);
			prop->nameoff = cpu_to_fdt32(nameoff);
		}
		offset = nextoffset;
	}
	nếu (nextoffset < 0)
		trả_về nextoffset;

	/* Finally, adjust the header */
	fdt_set_totalsize(fdt, newstroffset + fdt_size_dt_strings(fdt));

	/* And fix up fields that were keeping intermediate state. */
	fdt_set_last_comp_version(fdt, FDT_LAST_COMPATIBLE_VERSION);
	fdt_set_magic(fdt, FDT_MAGIC);

	trả_về 0;
}
