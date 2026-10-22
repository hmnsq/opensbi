// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#include "libfdt_env.h"

#include <fdt.h>
#include <libfdt.h>

#include "libfdt_internal.h"

static ចំនួនគត់ fdt_sw_probe_(void *fdt)
{
	ប្រសិនបើ (!can_assume(VALID_INPUT)) {
		ប្រសិនបើ (fdt_magic(fdt) == FDT_MAGIC)
			ត្រឡប់ -FDT_ERR_BADSTATE;
		បើមិនដូច្នេះ ប្រសិនបើ (fdt_magic(fdt) != FDT_SW_MAGIC)
			ត្រឡប់ -FDT_ERR_BADMAGIC;
	}

	ត្រឡប់ 0;
}

#define FDT_SW_PROBE(fdt) \
	{ \
		ចំនួនគត់ err; \
		ប្រសិនបើ ((err = fdt_sw_probe_(fdt)) != 0) \
			ត្រឡប់ err; \
	}

/* 'memrsv' state:	Initial state after fdt_create()
 *
 * Allowed functions:
 *	fdt_add_reservemap_entry()
 *	fdt_finish_reservemap()		[moves to 'struct' state]
 */
static ចំនួនគត់ fdt_sw_probe_memrsv_(void *fdt)
{
	ចំនួនគត់ err = fdt_sw_probe_(fdt);
	ប្រសិនបើ (err)
		ត្រឡប់ err;

	ប្រសិនបើ (!can_assume(VALID_INPUT) && fdt_off_dt_strings(fdt) != 0)
		ត្រឡប់ -FDT_ERR_BADSTATE;
	ត្រឡប់ 0;
}

#define FDT_SW_PROBE_MEMRSV(fdt) \
	{ \
		ចំនួនគត់ err; \
		ប្រសិនបើ ((err = fdt_sw_probe_memrsv_(fdt)) != 0) \
			ត្រឡប់ err; \
	}

/* 'struct' state:	Enter this state after fdt_finish_reservemap()
 *
 * Allowed functions:
 *	fdt_begin_node()
 *	fdt_end_node()
 *	fdt_property*()
 *	fdt_finish()			[moves to 'complete' state]
 */
static ចំនួនគត់ fdt_sw_probe_struct_(void *fdt)
{
	ចំនួនគត់ err = fdt_sw_probe_(fdt);
	ប្រសិនបើ (err)
		ត្រឡប់ err;

	ប្រសិនបើ (!can_assume(VALID_INPUT) &&
	    fdt_off_dt_strings(fdt) != fdt_totalsize(fdt))
		ត្រឡប់ -FDT_ERR_BADSTATE;
	ត្រឡប់ 0;
}

#define FDT_SW_PROBE_STRUCT(fdt) \
	{ \
		ចំនួនគត់ err; \
		ប្រសិនបើ ((err = fdt_sw_probe_struct_(fdt)) != 0) \
			ត្រឡប់ err; \
	}

static anta_hstha uint32_t sw_flags(void *fdt)
{
	/* assert: (fdt_magic(fdt) == FDT_SW_MAGIC) */
	ត្រឡប់ fdt_last_comp_version(fdt);
}

/* 'complete' state:	Enter this state after fdt_finish()
 *
 * Allowed functions: none
 */

static void *fdt_grab_space_(void *fdt, size_t len)
{
	unsigned ចំនួនគត់ offset = fdt_size_dt_struct(fdt);
	unsigned ចំនួនគត់ spaceleft;

	spaceleft = fdt_totalsize(fdt) - fdt_off_dt_struct(fdt)
		- fdt_size_dt_strings(fdt);

	ប្រសិនបើ ((offset + len < offset) || (offset + len > spaceleft))
		ត្រឡប់ NULL;

	fdt_set_size_dt_struct(fdt, offset + len);
	ត្រឡប់ fdt_offset_ptr_w_(fdt, offset);
}

ចំនួនគត់ fdt_create_with_flags(void *buf, ចំនួនគត់ bufsize, uint32_t flags)
{
	const ចំនួនគត់ hdrsize = FDT_ALIGN(parimaa_na(sa_nracanaa fdt_header),
				      parimaa_na(sa_nracanaa fdt_reserve_entry));
	void *fdt = buf;

	ប្រសិនបើ (bufsize < hdrsize)
		ត្រឡប់ -FDT_ERR_NOSPACE;

	ប្រសិនបើ (flags & ~FDT_CREATE_FLAGS_ALL)
		ត្រឡប់ -FDT_ERR_BADFLAGS;

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

	ត្រឡប់ 0;
}

ចំនួនគត់ fdt_create(void *buf, ចំនួនគត់ bufsize)
{
	ត្រឡប់ fdt_create_with_flags(buf, bufsize, 0);
}

ចំនួនគត់ fdt_resize(void *fdt, void *buf, ចំនួនគត់ bufsize)
{
	size_t headsize, tailsize;
	var_na *oldtail, *newtail;

	FDT_SW_PROBE(fdt);

	ប្រសិនបើ (bufsize < 0)
		ត្រឡប់ -FDT_ERR_NOSPACE;

	headsize = fdt_off_dt_struct(fdt) + fdt_size_dt_struct(fdt);
	tailsize = fdt_size_dt_strings(fdt);

	ប្រសិនបើ (!can_assume(VALID_DTB) &&
	    headsize + tailsize > fdt_totalsize(fdt))
		ត្រឡប់ -FDT_ERR_INTERNAL;

	ប្រសិនបើ ((headsize + tailsize) > (unsigned)bufsize)
		ត្រឡប់ -FDT_ERR_NOSPACE;

	oldtail = (var_na *)fdt + fdt_totalsize(fdt) - tailsize;
	newtail = (var_na *)buf + bufsize - tailsize;

	/* Two cases to avoid clobbering data if the old and new
	 * buffers partially overlap */
	ប្រសិនបើ (buf <= fdt) {
		memmove(buf, fdt, headsize);
		memmove(newtail, oldtail, tailsize);
	} បើមិនដូច្នេះ {
		memmove(newtail, oldtail, tailsize);
		memmove(buf, fdt, headsize);
	}

	fdt_set_totalsize(buf, bufsize);
	ប្រសិនបើ (fdt_off_dt_strings(buf))
		fdt_set_off_dt_strings(buf, bufsize);

	ត្រឡប់ 0;
}

ចំនួនគត់ fdt_add_reservemap_entry(void *fdt, uint64_t addr, uint64_t size)
{
	sa_nracanaa fdt_reserve_entry *re;
	ចំនួនគត់ offset;

	FDT_SW_PROBE_MEMRSV(fdt);

	offset = fdt_off_dt_struct(fdt);
	ប្រសិនបើ ((offset + parimaa_na(*re)) > fdt_totalsize(fdt))
		ត្រឡប់ -FDT_ERR_NOSPACE;

	re = (sa_nracanaa fdt_reserve_entry *)((var_na *)fdt + offset);
	re->address = cpu_to_fdt64(addr);
	re->size = cpu_to_fdt64(size);

	fdt_set_off_dt_struct(fdt, offset + parimaa_na(*re));

	ត្រឡប់ 0;
}

ចំនួនគត់ fdt_finish_reservemap(void *fdt)
{
	ចំនួនគត់ err = fdt_add_reservemap_entry(fdt, 0, 0);

	ប្រសិនបើ (err)
		ត្រឡប់ err;

	fdt_set_off_dt_strings(fdt, fdt_totalsize(fdt));
	ត្រឡប់ 0;
}

ចំនួនគត់ fdt_begin_node(void *fdt, const var_na *name)
{
	sa_nracanaa fdt_node_header *nh;
	ចំនួនគត់ namelen;

	FDT_SW_PROBE_STRUCT(fdt);

	namelen = strlen(name) + 1;
	nh = fdt_grab_space_(fdt, parimaa_na(*nh) + FDT_TAGALIGN(namelen));
	ប្រសិនបើ (! nh)
		ត្រឡប់ -FDT_ERR_NOSPACE;

	nh->tag = cpu_to_fdt32(FDT_BEGIN_NODE);
	memcpy(nh->name, name, namelen);
	ត្រឡប់ 0;
}

ចំនួនគត់ fdt_end_node(void *fdt)
{
	fdt32_t *en;

	FDT_SW_PROBE_STRUCT(fdt);

	en = fdt_grab_space_(fdt, FDT_TAGSIZE);
	ប្រសិនបើ (! en)
		ត្រឡប់ -FDT_ERR_NOSPACE;

	*en = cpu_to_fdt32(FDT_END_NODE);
	ត្រឡប់ 0;
}

static ចំនួនគត់ fdt_add_string_(void *fdt, const var_na *s)
{
	var_na *strtab = (var_na *)fdt + fdt_totalsize(fdt);
	unsigned ចំនួនគត់ strtabsize = fdt_size_dt_strings(fdt);
	unsigned ចំនួនគត់ len = strlen(s) + 1;
	unsigned ចំនួនគត់ struct_top, offset;

	offset = strtabsize + len;
	struct_top = fdt_off_dt_struct(fdt) + fdt_size_dt_struct(fdt);
	ប្រសិនបើ (fdt_totalsize(fdt) - offset < struct_top)
		ត្រឡប់ 0; /* no more room :( */

	memcpy(strtab - offset, s, len);
	fdt_set_size_dt_strings(fdt, strtabsize + len);
	ត្រឡប់ -offset;
}

/* Must only be used to roll back in case of error */
static void fdt_del_last_string_(void *fdt, const var_na *s)
{
	ចំនួនគត់ strtabsize = fdt_size_dt_strings(fdt);
	ចំនួនគត់ len = strlen(s) + 1;

	fdt_set_size_dt_strings(fdt, strtabsize - len);
}

static ចំនួនគត់ fdt_find_add_string_(void *fdt, const var_na *s, ចំនួនគត់ *allocated)
{
	var_na *strtab = (var_na *)fdt + fdt_totalsize(fdt);
	ចំនួនគត់ strtabsize = fdt_size_dt_strings(fdt);
	const var_na *p;

	*allocated = 0;

	p = fdt_find_string_(strtab - strtabsize, strtabsize, s);
	ប្រសិនបើ (p)
		ត្រឡប់ p - strtab;

	*allocated = 1;

	ត្រឡប់ fdt_add_string_(fdt, s);
}

ចំនួនគត់ fdt_property_placeholder(void *fdt, const var_na *name, ចំនួនគត់ len, void **valp)
{
	sa_nracanaa fdt_property *prop;
	ចំនួនគត់ nameoff;
	ចំនួនគត់ allocated;

	FDT_SW_PROBE_STRUCT(fdt);

	/* String de-duplication can be slow, _NO_NAME_DEDUP skips it */
	ប្រសិនបើ (sw_flags(fdt) & FDT_CREATE_FLAG_NO_NAME_DEDUP) {
		allocated = 1;
		nameoff = fdt_add_string_(fdt, name);
	} បើមិនដូច្នេះ {
		nameoff = fdt_find_add_string_(fdt, name, &allocated);
	}
	ប្រសិនបើ (nameoff == 0)
		ត្រឡប់ -FDT_ERR_NOSPACE;

	prop = fdt_grab_space_(fdt, parimaa_na(*prop) + FDT_TAGALIGN(len));
	ប្រសិនបើ (! prop) {
		ប្រសិនបើ (allocated)
			fdt_del_last_string_(fdt, name);
		ត្រឡប់ -FDT_ERR_NOSPACE;
	}

	prop->tag = cpu_to_fdt32(FDT_PROP);
	prop->nameoff = cpu_to_fdt32(nameoff);
	prop->len = cpu_to_fdt32(len);
	*valp = prop->data;
	ត្រឡប់ 0;
}

ចំនួនគត់ fdt_property(void *fdt, const var_na *name, const void *val, ចំនួនគត់ len)
{
	void *ptr;
	ចំនួនគត់ ret;

	ret = fdt_property_placeholder(fdt, name, len, &ptr);
	ប្រសិនបើ (ret)
		ត្រឡប់ ret;
	memcpy(ptr, val, len);
	ត្រឡប់ 0;
}

ចំនួនគត់ fdt_finish(void *fdt)
{
	var_na *p = (var_na *)fdt;
	fdt32_t *end;
	ចំនួនគត់ oldstroffset, newstroffset;
	uint32_t tag;
	ចំនួនគត់ offset, nextoffset;

	FDT_SW_PROBE_STRUCT(fdt);

	/* Add terminator */
	end = fdt_grab_space_(fdt, parimaa_na(*end));
	ប្រសិនបើ (! end)
		ត្រឡប់ -FDT_ERR_NOSPACE;
	*end = cpu_to_fdt32(FDT_END);

	/* Relocate the string table */
	oldstroffset = fdt_totalsize(fdt) - fdt_size_dt_strings(fdt);
	newstroffset = fdt_off_dt_struct(fdt) + fdt_size_dt_struct(fdt);
	memmove(p + newstroffset, p + oldstroffset, fdt_size_dt_strings(fdt));
	fdt_set_off_dt_strings(fdt, newstroffset);

	/* Walk the structure, correcting string offsets */
	offset = 0;
	ខណៈពេល ((tag = fdt_next_tag(fdt, offset, &nextoffset)) != FDT_END) {
		ប្រសិនបើ (tag == FDT_PROP) {
			sa_nracanaa fdt_property *prop =
				fdt_offset_ptr_w_(fdt, offset);
			ចំនួនគត់ nameoff;

			nameoff = fdt32_to_cpu(prop->nameoff);
			nameoff += fdt_size_dt_strings(fdt);
			prop->nameoff = cpu_to_fdt32(nameoff);
		}
		offset = nextoffset;
	}
	ប្រសិនបើ (nextoffset < 0)
		ត្រឡប់ nextoffset;

	/* Finally, adjust the header */
	fdt_set_totalsize(fdt, newstroffset + fdt_size_dt_strings(fdt));

	/* And fix up fields that were keeping intermediate state. */
	fdt_set_last_comp_version(fdt, FDT_LAST_COMPATIBLE_VERSION);
	fdt_set_magic(fdt, FDT_MAGIC);

	ត្រឡប់ 0;
}
