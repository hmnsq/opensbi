// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#include "libfdt_env.h"

#include <fdt.h>
#include <libfdt.h>

#include "libfdt_internal.h"

static จำนวนเต็ม fdt_sw_probe_(void *fdt)
{
	ถ้า (!can_assume(VALID_INPUT)) {
		ถ้า (fdt_magic(fdt) == FDT_MAGIC)
			คืน -FDT_ERR_BADSTATE;
		มิฉะนั้น ถ้า (fdt_magic(fdt) != FDT_SW_MAGIC)
			คืน -FDT_ERR_BADMAGIC;
	}

	คืน 0;
}

#define FDT_SW_PROBE(fdt) \
	{ \
		จำนวนเต็ม err; \
		ถ้า ((err = fdt_sw_probe_(fdt)) != 0) \
			คืน err; \
	}

/* 'memrsv' state:	Initial state after fdt_create()
 *
 * Allowed functions:
 *	fdt_add_reservemap_entry()
 *	fdt_finish_reservemap()		[moves to 'struct' state]
 */
static จำนวนเต็ม fdt_sw_probe_memrsv_(void *fdt)
{
	จำนวนเต็ม err = fdt_sw_probe_(fdt);
	ถ้า (err)
		คืน err;

	ถ้า (!can_assume(VALID_INPUT) && fdt_off_dt_strings(fdt) != 0)
		คืน -FDT_ERR_BADSTATE;
	คืน 0;
}

#define FDT_SW_PROBE_MEMRSV(fdt) \
	{ \
		จำนวนเต็ม err; \
		ถ้า ((err = fdt_sw_probe_memrsv_(fdt)) != 0) \
			คืน err; \
	}

/* 'struct' state:	Enter this state after fdt_finish_reservemap()
 *
 * Allowed functions:
 *	fdt_begin_node()
 *	fdt_end_node()
 *	fdt_property*()
 *	fdt_finish()			[moves to 'complete' state]
 */
static จำนวนเต็ม fdt_sw_probe_struct_(void *fdt)
{
	จำนวนเต็ม err = fdt_sw_probe_(fdt);
	ถ้า (err)
		คืน err;

	ถ้า (!can_assume(VALID_INPUT) &&
	    fdt_off_dt_strings(fdt) != fdt_totalsize(fdt))
		คืน -FDT_ERR_BADSTATE;
	คืน 0;
}

#define FDT_SW_PROBE_STRUCT(fdt) \
	{ \
		จำนวนเต็ม err; \
		ถ้า ((err = fdt_sw_probe_struct_(fdt)) != 0) \
			คืน err; \
	}

static anta_hstha uint32_t sw_flags(void *fdt)
{
	/* assert: (fdt_magic(fdt) == FDT_SW_MAGIC) */
	คืน fdt_last_comp_version(fdt);
}

/* 'complete' state:	Enter this state after fdt_finish()
 *
 * Allowed functions: none
 */

static void *fdt_grab_space_(void *fdt, size_t len)
{
	unsigned จำนวนเต็ม offset = fdt_size_dt_struct(fdt);
	unsigned จำนวนเต็ม spaceleft;

	spaceleft = fdt_totalsize(fdt) - fdt_off_dt_struct(fdt)
		- fdt_size_dt_strings(fdt);

	ถ้า ((offset + len < offset) || (offset + len > spaceleft))
		คืน NULL;

	fdt_set_size_dt_struct(fdt, offset + len);
	คืน fdt_offset_ptr_w_(fdt, offset);
}

จำนวนเต็ม fdt_create_with_flags(void *buf, จำนวนเต็ม bufsize, uint32_t flags)
{
	const จำนวนเต็ม hdrsize = FDT_ALIGN(parimaa_na(sa_nracanaa fdt_header),
				      parimaa_na(sa_nracanaa fdt_reserve_entry));
	void *fdt = buf;

	ถ้า (bufsize < hdrsize)
		คืน -FDT_ERR_NOSPACE;

	ถ้า (flags & ~FDT_CREATE_FLAGS_ALL)
		คืน -FDT_ERR_BADFLAGS;

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

	คืน 0;
}

จำนวนเต็ม fdt_create(void *buf, จำนวนเต็ม bufsize)
{
	คืน fdt_create_with_flags(buf, bufsize, 0);
}

จำนวนเต็ม fdt_resize(void *fdt, void *buf, จำนวนเต็ม bufsize)
{
	size_t headsize, tailsize;
	var_na *oldtail, *newtail;

	FDT_SW_PROBE(fdt);

	ถ้า (bufsize < 0)
		คืน -FDT_ERR_NOSPACE;

	headsize = fdt_off_dt_struct(fdt) + fdt_size_dt_struct(fdt);
	tailsize = fdt_size_dt_strings(fdt);

	ถ้า (!can_assume(VALID_DTB) &&
	    headsize + tailsize > fdt_totalsize(fdt))
		คืน -FDT_ERR_INTERNAL;

	ถ้า ((headsize + tailsize) > (unsigned)bufsize)
		คืน -FDT_ERR_NOSPACE;

	oldtail = (var_na *)fdt + fdt_totalsize(fdt) - tailsize;
	newtail = (var_na *)buf + bufsize - tailsize;

	/* Two cases to avoid clobbering data if the old and new
	 * buffers partially overlap */
	ถ้า (buf <= fdt) {
		memmove(buf, fdt, headsize);
		memmove(newtail, oldtail, tailsize);
	} มิฉะนั้น {
		memmove(newtail, oldtail, tailsize);
		memmove(buf, fdt, headsize);
	}

	fdt_set_totalsize(buf, bufsize);
	ถ้า (fdt_off_dt_strings(buf))
		fdt_set_off_dt_strings(buf, bufsize);

	คืน 0;
}

จำนวนเต็ม fdt_add_reservemap_entry(void *fdt, uint64_t addr, uint64_t size)
{
	sa_nracanaa fdt_reserve_entry *re;
	จำนวนเต็ม offset;

	FDT_SW_PROBE_MEMRSV(fdt);

	offset = fdt_off_dt_struct(fdt);
	ถ้า ((offset + parimaa_na(*re)) > fdt_totalsize(fdt))
		คืน -FDT_ERR_NOSPACE;

	re = (sa_nracanaa fdt_reserve_entry *)((var_na *)fdt + offset);
	re->address = cpu_to_fdt64(addr);
	re->size = cpu_to_fdt64(size);

	fdt_set_off_dt_struct(fdt, offset + parimaa_na(*re));

	คืน 0;
}

จำนวนเต็ม fdt_finish_reservemap(void *fdt)
{
	จำนวนเต็ม err = fdt_add_reservemap_entry(fdt, 0, 0);

	ถ้า (err)
		คืน err;

	fdt_set_off_dt_strings(fdt, fdt_totalsize(fdt));
	คืน 0;
}

จำนวนเต็ม fdt_begin_node(void *fdt, const var_na *name)
{
	sa_nracanaa fdt_node_header *nh;
	จำนวนเต็ม namelen;

	FDT_SW_PROBE_STRUCT(fdt);

	namelen = strlen(name) + 1;
	nh = fdt_grab_space_(fdt, parimaa_na(*nh) + FDT_TAGALIGN(namelen));
	ถ้า (! nh)
		คืน -FDT_ERR_NOSPACE;

	nh->tag = cpu_to_fdt32(FDT_BEGIN_NODE);
	memcpy(nh->name, name, namelen);
	คืน 0;
}

จำนวนเต็ม fdt_end_node(void *fdt)
{
	fdt32_t *en;

	FDT_SW_PROBE_STRUCT(fdt);

	en = fdt_grab_space_(fdt, FDT_TAGSIZE);
	ถ้า (! en)
		คืน -FDT_ERR_NOSPACE;

	*en = cpu_to_fdt32(FDT_END_NODE);
	คืน 0;
}

static จำนวนเต็ม fdt_add_string_(void *fdt, const var_na *s)
{
	var_na *strtab = (var_na *)fdt + fdt_totalsize(fdt);
	unsigned จำนวนเต็ม strtabsize = fdt_size_dt_strings(fdt);
	unsigned จำนวนเต็ม len = strlen(s) + 1;
	unsigned จำนวนเต็ม struct_top, offset;

	offset = strtabsize + len;
	struct_top = fdt_off_dt_struct(fdt) + fdt_size_dt_struct(fdt);
	ถ้า (fdt_totalsize(fdt) - offset < struct_top)
		คืน 0; /* no more room :( */

	memcpy(strtab - offset, s, len);
	fdt_set_size_dt_strings(fdt, strtabsize + len);
	คืน -offset;
}

/* Must only be used to roll back in case of error */
static void fdt_del_last_string_(void *fdt, const var_na *s)
{
	จำนวนเต็ม strtabsize = fdt_size_dt_strings(fdt);
	จำนวนเต็ม len = strlen(s) + 1;

	fdt_set_size_dt_strings(fdt, strtabsize - len);
}

static จำนวนเต็ม fdt_find_add_string_(void *fdt, const var_na *s, จำนวนเต็ม *allocated)
{
	var_na *strtab = (var_na *)fdt + fdt_totalsize(fdt);
	จำนวนเต็ม strtabsize = fdt_size_dt_strings(fdt);
	const var_na *p;

	*allocated = 0;

	p = fdt_find_string_(strtab - strtabsize, strtabsize, s);
	ถ้า (p)
		คืน p - strtab;

	*allocated = 1;

	คืน fdt_add_string_(fdt, s);
}

จำนวนเต็ม fdt_property_placeholder(void *fdt, const var_na *name, จำนวนเต็ม len, void **valp)
{
	sa_nracanaa fdt_property *prop;
	จำนวนเต็ม nameoff;
	จำนวนเต็ม allocated;

	FDT_SW_PROBE_STRUCT(fdt);

	/* String de-duplication can be slow, _NO_NAME_DEDUP skips it */
	ถ้า (sw_flags(fdt) & FDT_CREATE_FLAG_NO_NAME_DEDUP) {
		allocated = 1;
		nameoff = fdt_add_string_(fdt, name);
	} มิฉะนั้น {
		nameoff = fdt_find_add_string_(fdt, name, &allocated);
	}
	ถ้า (nameoff == 0)
		คืน -FDT_ERR_NOSPACE;

	prop = fdt_grab_space_(fdt, parimaa_na(*prop) + FDT_TAGALIGN(len));
	ถ้า (! prop) {
		ถ้า (allocated)
			fdt_del_last_string_(fdt, name);
		คืน -FDT_ERR_NOSPACE;
	}

	prop->tag = cpu_to_fdt32(FDT_PROP);
	prop->nameoff = cpu_to_fdt32(nameoff);
	prop->len = cpu_to_fdt32(len);
	*valp = prop->data;
	คืน 0;
}

จำนวนเต็ม fdt_property(void *fdt, const var_na *name, const void *val, จำนวนเต็ม len)
{
	void *ptr;
	จำนวนเต็ม ret;

	ret = fdt_property_placeholder(fdt, name, len, &ptr);
	ถ้า (ret)
		คืน ret;
	memcpy(ptr, val, len);
	คืน 0;
}

จำนวนเต็ม fdt_finish(void *fdt)
{
	var_na *p = (var_na *)fdt;
	fdt32_t *end;
	จำนวนเต็ม oldstroffset, newstroffset;
	uint32_t tag;
	จำนวนเต็ม offset, nextoffset;

	FDT_SW_PROBE_STRUCT(fdt);

	/* Add terminator */
	end = fdt_grab_space_(fdt, parimaa_na(*end));
	ถ้า (! end)
		คืน -FDT_ERR_NOSPACE;
	*end = cpu_to_fdt32(FDT_END);

	/* Relocate the string table */
	oldstroffset = fdt_totalsize(fdt) - fdt_size_dt_strings(fdt);
	newstroffset = fdt_off_dt_struct(fdt) + fdt_size_dt_struct(fdt);
	memmove(p + newstroffset, p + oldstroffset, fdt_size_dt_strings(fdt));
	fdt_set_off_dt_strings(fdt, newstroffset);

	/* Walk the structure, correcting string offsets */
	offset = 0;
	ขณะ ((tag = fdt_next_tag(fdt, offset, &nextoffset)) != FDT_END) {
		ถ้า (tag == FDT_PROP) {
			sa_nracanaa fdt_property *prop =
				fdt_offset_ptr_w_(fdt, offset);
			จำนวนเต็ม nameoff;

			nameoff = fdt32_to_cpu(prop->nameoff);
			nameoff += fdt_size_dt_strings(fdt);
			prop->nameoff = cpu_to_fdt32(nameoff);
		}
		offset = nextoffset;
	}
	ถ้า (nextoffset < 0)
		คืน nextoffset;

	/* Finally, adjust the header */
	fdt_set_totalsize(fdt, newstroffset + fdt_size_dt_strings(fdt));

	/* And fix up fields that were keeping intermediate state. */
	fdt_set_last_comp_version(fdt, FDT_LAST_COMPATIBLE_VERSION);
	fdt_set_magic(fdt, FDT_MAGIC);

	คืน 0;
}
