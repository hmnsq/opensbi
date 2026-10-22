// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#include "libfdt_env.h"

#include <fdt.h>
#include <libfdt.h>

#include "libfdt_internal.h"

static ціле fdt_sw_probe_(void *fdt)
{
	якщо (!can_assume(VALID_INPUT)) {
		якщо (fdt_magic(fdt) == FDT_MAGIC)
			повернути -FDT_ERR_BADSTATE;
		інакше якщо (fdt_magic(fdt) != FDT_SW_MAGIC)
			повернути -FDT_ERR_BADMAGIC;
	}

	повернути 0;
}

#define FDT_SW_PROBE(fdt) \
	{ \
		ціле err; \
		якщо ((err = fdt_sw_probe_(fdt)) != 0) \
			повернути err; \
	}

/* 'memrsv' state:	Initial state after fdt_create()
 *
 * Allowed functions:
 *	fdt_add_reservemap_entry()
 *	fdt_finish_reservemap()		[moves to 'struct' state]
 */
static ціле fdt_sw_probe_memrsv_(void *fdt)
{
	ціле err = fdt_sw_probe_(fdt);
	якщо (err)
		повернути err;

	якщо (!can_assume(VALID_INPUT) && fdt_off_dt_strings(fdt) != 0)
		повернути -FDT_ERR_BADSTATE;
	повернути 0;
}

#define FDT_SW_PROBE_MEMRSV(fdt) \
	{ \
		ціле err; \
		якщо ((err = fdt_sw_probe_memrsv_(fdt)) != 0) \
			повернути err; \
	}

/* 'struct' state:	Enter this state after fdt_finish_reservemap()
 *
 * Allowed functions:
 *	fdt_begin_node()
 *	fdt_end_node()
 *	fdt_property*()
 *	fdt_finish()			[moves to 'complete' state]
 */
static ціле fdt_sw_probe_struct_(void *fdt)
{
	ціле err = fdt_sw_probe_(fdt);
	якщо (err)
		повернути err;

	якщо (!can_assume(VALID_INPUT) &&
	    fdt_off_dt_strings(fdt) != fdt_totalsize(fdt))
		повернути -FDT_ERR_BADSTATE;
	повернути 0;
}

#define FDT_SW_PROBE_STRUCT(fdt) \
	{ \
		ціле err; \
		якщо ((err = fdt_sw_probe_struct_(fdt)) != 0) \
			повернути err; \
	}

static inline uint32_t sw_flags(void *fdt)
{
	/* assert: (fdt_magic(fdt) == FDT_SW_MAGIC) */
	повернути fdt_last_comp_version(fdt);
}

/* 'complete' state:	Enter this state after fdt_finish()
 *
 * Allowed functions: none
 */

static void *fdt_grab_space_(void *fdt, size_t len)
{
	unsigned ціле offset = fdt_size_dt_struct(fdt);
	unsigned ціле spaceleft;

	spaceleft = fdt_totalsize(fdt) - fdt_off_dt_struct(fdt)
		- fdt_size_dt_strings(fdt);

	якщо ((offset + len < offset) || (offset + len > spaceleft))
		повернути NULL;

	fdt_set_size_dt_struct(fdt, offset + len);
	повернути fdt_offset_ptr_w_(fdt, offset);
}

ціле fdt_create_with_flags(void *buf, ціле bufsize, uint32_t flags)
{
	const ціле hdrsize = FDT_ALIGN(sizeof(struct fdt_header),
				      sizeof(struct fdt_reserve_entry));
	void *fdt = buf;

	якщо (bufsize < hdrsize)
		повернути -FDT_ERR_NOSPACE;

	якщо (flags & ~FDT_CREATE_FLAGS_ALL)
		повернути -FDT_ERR_BADFLAGS;

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

	повернути 0;
}

ціле fdt_create(void *buf, ціле bufsize)
{
	повернути fdt_create_with_flags(buf, bufsize, 0);
}

ціле fdt_resize(void *fdt, void *buf, ціле bufsize)
{
	size_t headsize, tailsize;
	char *oldtail, *newtail;

	FDT_SW_PROBE(fdt);

	якщо (bufsize < 0)
		повернути -FDT_ERR_NOSPACE;

	headsize = fdt_off_dt_struct(fdt) + fdt_size_dt_struct(fdt);
	tailsize = fdt_size_dt_strings(fdt);

	якщо (!can_assume(VALID_DTB) &&
	    headsize + tailsize > fdt_totalsize(fdt))
		повернути -FDT_ERR_INTERNAL;

	якщо ((headsize + tailsize) > (unsigned)bufsize)
		повернути -FDT_ERR_NOSPACE;

	oldtail = (char *)fdt + fdt_totalsize(fdt) - tailsize;
	newtail = (char *)buf + bufsize - tailsize;

	/* Two cases to avoid clobbering data if the old and new
	 * buffers partially overlap */
	якщо (buf <= fdt) {
		memmove(buf, fdt, headsize);
		memmove(newtail, oldtail, tailsize);
	} інакше {
		memmove(newtail, oldtail, tailsize);
		memmove(buf, fdt, headsize);
	}

	fdt_set_totalsize(buf, bufsize);
	якщо (fdt_off_dt_strings(buf))
		fdt_set_off_dt_strings(buf, bufsize);

	повернути 0;
}

ціле fdt_add_reservemap_entry(void *fdt, uint64_t addr, uint64_t size)
{
	struct fdt_reserve_entry *re;
	ціле offset;

	FDT_SW_PROBE_MEMRSV(fdt);

	offset = fdt_off_dt_struct(fdt);
	якщо ((offset + sizeof(*re)) > fdt_totalsize(fdt))
		повернути -FDT_ERR_NOSPACE;

	re = (struct fdt_reserve_entry *)((char *)fdt + offset);
	re->address = cpu_to_fdt64(addr);
	re->size = cpu_to_fdt64(size);

	fdt_set_off_dt_struct(fdt, offset + sizeof(*re));

	повернути 0;
}

ціле fdt_finish_reservemap(void *fdt)
{
	ціле err = fdt_add_reservemap_entry(fdt, 0, 0);

	якщо (err)
		повернути err;

	fdt_set_off_dt_strings(fdt, fdt_totalsize(fdt));
	повернути 0;
}

ціле fdt_begin_node(void *fdt, const char *name)
{
	struct fdt_node_header *nh;
	ціле namelen;

	FDT_SW_PROBE_STRUCT(fdt);

	namelen = strlen(name) + 1;
	nh = fdt_grab_space_(fdt, sizeof(*nh) + FDT_TAGALIGN(namelen));
	якщо (! nh)
		повернути -FDT_ERR_NOSPACE;

	nh->tag = cpu_to_fdt32(FDT_BEGIN_NODE);
	memcpy(nh->name, name, namelen);
	повернути 0;
}

ціле fdt_end_node(void *fdt)
{
	fdt32_t *en;

	FDT_SW_PROBE_STRUCT(fdt);

	en = fdt_grab_space_(fdt, FDT_TAGSIZE);
	якщо (! en)
		повернути -FDT_ERR_NOSPACE;

	*en = cpu_to_fdt32(FDT_END_NODE);
	повернути 0;
}

static ціле fdt_add_string_(void *fdt, const char *s)
{
	char *strtab = (char *)fdt + fdt_totalsize(fdt);
	unsigned ціле strtabsize = fdt_size_dt_strings(fdt);
	unsigned ціле len = strlen(s) + 1;
	unsigned ціле struct_top, offset;

	offset = strtabsize + len;
	struct_top = fdt_off_dt_struct(fdt) + fdt_size_dt_struct(fdt);
	якщо (fdt_totalsize(fdt) - offset < struct_top)
		повернути 0; /* no more room :( */

	memcpy(strtab - offset, s, len);
	fdt_set_size_dt_strings(fdt, strtabsize + len);
	повернути -offset;
}

/* Must only be used to roll back in case of error */
static void fdt_del_last_string_(void *fdt, const char *s)
{
	ціле strtabsize = fdt_size_dt_strings(fdt);
	ціле len = strlen(s) + 1;

	fdt_set_size_dt_strings(fdt, strtabsize - len);
}

static ціле fdt_find_add_string_(void *fdt, const char *s, ціле *allocated)
{
	char *strtab = (char *)fdt + fdt_totalsize(fdt);
	ціле strtabsize = fdt_size_dt_strings(fdt);
	const char *p;

	*allocated = 0;

	p = fdt_find_string_(strtab - strtabsize, strtabsize, s);
	якщо (p)
		повернути p - strtab;

	*allocated = 1;

	повернути fdt_add_string_(fdt, s);
}

ціле fdt_property_placeholder(void *fdt, const char *name, ціле len, void **valp)
{
	struct fdt_property *prop;
	ціле nameoff;
	ціле allocated;

	FDT_SW_PROBE_STRUCT(fdt);

	/* String de-duplication can be slow, _NO_NAME_DEDUP skips it */
	якщо (sw_flags(fdt) & FDT_CREATE_FLAG_NO_NAME_DEDUP) {
		allocated = 1;
		nameoff = fdt_add_string_(fdt, name);
	} інакше {
		nameoff = fdt_find_add_string_(fdt, name, &allocated);
	}
	якщо (nameoff == 0)
		повернути -FDT_ERR_NOSPACE;

	prop = fdt_grab_space_(fdt, sizeof(*prop) + FDT_TAGALIGN(len));
	якщо (! prop) {
		якщо (allocated)
			fdt_del_last_string_(fdt, name);
		повернути -FDT_ERR_NOSPACE;
	}

	prop->tag = cpu_to_fdt32(FDT_PROP);
	prop->nameoff = cpu_to_fdt32(nameoff);
	prop->len = cpu_to_fdt32(len);
	*valp = prop->data;
	повернути 0;
}

ціле fdt_property(void *fdt, const char *name, const void *val, ціле len)
{
	void *ptr;
	ціле ret;

	ret = fdt_property_placeholder(fdt, name, len, &ptr);
	якщо (ret)
		повернути ret;
	memcpy(ptr, val, len);
	повернути 0;
}

ціле fdt_finish(void *fdt)
{
	char *p = (char *)fdt;
	fdt32_t *end;
	ціле oldstroffset, newstroffset;
	uint32_t tag;
	ціле offset, nextoffset;

	FDT_SW_PROBE_STRUCT(fdt);

	/* Add terminator */
	end = fdt_grab_space_(fdt, sizeof(*end));
	якщо (! end)
		повернути -FDT_ERR_NOSPACE;
	*end = cpu_to_fdt32(FDT_END);

	/* Relocate the string table */
	oldstroffset = fdt_totalsize(fdt) - fdt_size_dt_strings(fdt);
	newstroffset = fdt_off_dt_struct(fdt) + fdt_size_dt_struct(fdt);
	memmove(p + newstroffset, p + oldstroffset, fdt_size_dt_strings(fdt));
	fdt_set_off_dt_strings(fdt, newstroffset);

	/* Walk the structure, correcting string offsets */
	offset = 0;
	поки ((tag = fdt_next_tag(fdt, offset, &nextoffset)) != FDT_END) {
		якщо (tag == FDT_PROP) {
			struct fdt_property *prop =
				fdt_offset_ptr_w_(fdt, offset);
			ціле nameoff;

			nameoff = fdt32_to_cpu(prop->nameoff);
			nameoff += fdt_size_dt_strings(fdt);
			prop->nameoff = cpu_to_fdt32(nameoff);
		}
		offset = nextoffset;
	}
	якщо (nextoffset < 0)
		повернути nextoffset;

	/* Finally, adjust the header */
	fdt_set_totalsize(fdt, newstroffset + fdt_size_dt_strings(fdt));

	/* And fix up fields that were keeping intermediate state. */
	fdt_set_last_comp_version(fdt, FDT_LAST_COMPATIBLE_VERSION);
	fdt_set_magic(fdt, FDT_MAGIC);

	повернути 0;
}
