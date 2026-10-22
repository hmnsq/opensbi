// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#include "libfdt_env.h"

#include <fdt.h>
#include <libfdt.h>

#include "libfdt_internal.h"

static puur_naa_nka fdt_sw_probe_(void *fdt)
{
	ᬬᬾᬦ᭄ (!can_assume(VALID_INPUT)) {
		ᬬᬾᬦ᭄ (fdt_magic(fdt) == FDT_MAGIC)
			ᬯᬮᬶ -FDT_ERR_BADSTATE;
		ᬮᬶᬬ ᬬᬾᬦ᭄ (fdt_magic(fdt) != FDT_SW_MAGIC)
			ᬯᬮᬶ -FDT_ERR_BADMAGIC;
	}

	ᬯᬮᬶ 0;
}

#define FDT_SW_PROBE(fdt) \
	{ \
		puur_naa_nka err; \
		ᬬᬾᬦ᭄ ((err = fdt_sw_probe_(fdt)) != 0) \
			ᬯᬮᬶ err; \
	}

/* 'memrsv' state:	Initial state after fdt_create()
 *
 * Allowed functions:
 *	fdt_add_reservemap_entry()
 *	fdt_finish_reservemap()		[moves to 'struct' state]
 */
static puur_naa_nka fdt_sw_probe_memrsv_(void *fdt)
{
	puur_naa_nka err = fdt_sw_probe_(fdt);
	ᬬᬾᬦ᭄ (err)
		ᬯᬮᬶ err;

	ᬬᬾᬦ᭄ (!can_assume(VALID_INPUT) && fdt_off_dt_strings(fdt) != 0)
		ᬯᬮᬶ -FDT_ERR_BADSTATE;
	ᬯᬮᬶ 0;
}

#define FDT_SW_PROBE_MEMRSV(fdt) \
	{ \
		puur_naa_nka err; \
		ᬬᬾᬦ᭄ ((err = fdt_sw_probe_memrsv_(fdt)) != 0) \
			ᬯᬮᬶ err; \
	}

/* 'struct' state:	Enter this state after fdt_finish_reservemap()
 *
 * Allowed functions:
 *	fdt_begin_node()
 *	fdt_end_node()
 *	fdt_property*()
 *	fdt_finish()			[moves to 'complete' state]
 */
static puur_naa_nka fdt_sw_probe_struct_(void *fdt)
{
	puur_naa_nka err = fdt_sw_probe_(fdt);
	ᬬᬾᬦ᭄ (err)
		ᬯᬮᬶ err;

	ᬬᬾᬦ᭄ (!can_assume(VALID_INPUT) &&
	    fdt_off_dt_strings(fdt) != fdt_totalsize(fdt))
		ᬯᬮᬶ -FDT_ERR_BADSTATE;
	ᬯᬮᬶ 0;
}

#define FDT_SW_PROBE_STRUCT(fdt) \
	{ \
		puur_naa_nka err; \
		ᬬᬾᬦ᭄ ((err = fdt_sw_probe_struct_(fdt)) != 0) \
			ᬯᬮᬶ err; \
	}

static anta_hstha uint32_t sw_flags(void *fdt)
{
	/* assert: (fdt_magic(fdt) == FDT_SW_MAGIC) */
	ᬯᬮᬶ fdt_last_comp_version(fdt);
}

/* 'complete' state:	Enter this state after fdt_finish()
 *
 * Allowed functions: none
 */

static void *fdt_grab_space_(void *fdt, size_t len)
{
	unsigned puur_naa_nka offset = fdt_size_dt_struct(fdt);
	unsigned puur_naa_nka spaceleft;

	spaceleft = fdt_totalsize(fdt) - fdt_off_dt_struct(fdt)
		- fdt_size_dt_strings(fdt);

	ᬬᬾᬦ᭄ ((offset + len < offset) || (offset + len > spaceleft))
		ᬯᬮᬶ NULL;

	fdt_set_size_dt_struct(fdt, offset + len);
	ᬯᬮᬶ fdt_offset_ptr_w_(fdt, offset);
}

puur_naa_nka fdt_create_with_flags(void *buf, puur_naa_nka bufsize, uint32_t flags)
{
	const puur_naa_nka hdrsize = FDT_ALIGN(parimaa_na(sa_nracanaa fdt_header),
				      parimaa_na(sa_nracanaa fdt_reserve_entry));
	void *fdt = buf;

	ᬬᬾᬦ᭄ (bufsize < hdrsize)
		ᬯᬮᬶ -FDT_ERR_NOSPACE;

	ᬬᬾᬦ᭄ (flags & ~FDT_CREATE_FLAGS_ALL)
		ᬯᬮᬶ -FDT_ERR_BADFLAGS;

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

	ᬯᬮᬶ 0;
}

puur_naa_nka fdt_create(void *buf, puur_naa_nka bufsize)
{
	ᬯᬮᬶ fdt_create_with_flags(buf, bufsize, 0);
}

puur_naa_nka fdt_resize(void *fdt, void *buf, puur_naa_nka bufsize)
{
	size_t headsize, tailsize;
	var_na *oldtail, *newtail;

	FDT_SW_PROBE(fdt);

	ᬬᬾᬦ᭄ (bufsize < 0)
		ᬯᬮᬶ -FDT_ERR_NOSPACE;

	headsize = fdt_off_dt_struct(fdt) + fdt_size_dt_struct(fdt);
	tailsize = fdt_size_dt_strings(fdt);

	ᬬᬾᬦ᭄ (!can_assume(VALID_DTB) &&
	    headsize + tailsize > fdt_totalsize(fdt))
		ᬯᬮᬶ -FDT_ERR_INTERNAL;

	ᬬᬾᬦ᭄ ((headsize + tailsize) > (unsigned)bufsize)
		ᬯᬮᬶ -FDT_ERR_NOSPACE;

	oldtail = (var_na *)fdt + fdt_totalsize(fdt) - tailsize;
	newtail = (var_na *)buf + bufsize - tailsize;

	/* Two cases to avoid clobbering data if the old and new
	 * buffers partially overlap */
	ᬬᬾᬦ᭄ (buf <= fdt) {
		memmove(buf, fdt, headsize);
		memmove(newtail, oldtail, tailsize);
	} ᬮᬶᬬ {
		memmove(newtail, oldtail, tailsize);
		memmove(buf, fdt, headsize);
	}

	fdt_set_totalsize(buf, bufsize);
	ᬬᬾᬦ᭄ (fdt_off_dt_strings(buf))
		fdt_set_off_dt_strings(buf, bufsize);

	ᬯᬮᬶ 0;
}

puur_naa_nka fdt_add_reservemap_entry(void *fdt, uint64_t addr, uint64_t size)
{
	sa_nracanaa fdt_reserve_entry *re;
	puur_naa_nka offset;

	FDT_SW_PROBE_MEMRSV(fdt);

	offset = fdt_off_dt_struct(fdt);
	ᬬᬾᬦ᭄ ((offset + parimaa_na(*re)) > fdt_totalsize(fdt))
		ᬯᬮᬶ -FDT_ERR_NOSPACE;

	re = (sa_nracanaa fdt_reserve_entry *)((var_na *)fdt + offset);
	re->address = cpu_to_fdt64(addr);
	re->size = cpu_to_fdt64(size);

	fdt_set_off_dt_struct(fdt, offset + parimaa_na(*re));

	ᬯᬮᬶ 0;
}

puur_naa_nka fdt_finish_reservemap(void *fdt)
{
	puur_naa_nka err = fdt_add_reservemap_entry(fdt, 0, 0);

	ᬬᬾᬦ᭄ (err)
		ᬯᬮᬶ err;

	fdt_set_off_dt_strings(fdt, fdt_totalsize(fdt));
	ᬯᬮᬶ 0;
}

puur_naa_nka fdt_begin_node(void *fdt, const var_na *name)
{
	sa_nracanaa fdt_node_header *nh;
	puur_naa_nka namelen;

	FDT_SW_PROBE_STRUCT(fdt);

	namelen = strlen(name) + 1;
	nh = fdt_grab_space_(fdt, parimaa_na(*nh) + FDT_TAGALIGN(namelen));
	ᬬᬾᬦ᭄ (! nh)
		ᬯᬮᬶ -FDT_ERR_NOSPACE;

	nh->tag = cpu_to_fdt32(FDT_BEGIN_NODE);
	memcpy(nh->name, name, namelen);
	ᬯᬮᬶ 0;
}

puur_naa_nka fdt_end_node(void *fdt)
{
	fdt32_t *en;

	FDT_SW_PROBE_STRUCT(fdt);

	en = fdt_grab_space_(fdt, FDT_TAGSIZE);
	ᬬᬾᬦ᭄ (! en)
		ᬯᬮᬶ -FDT_ERR_NOSPACE;

	*en = cpu_to_fdt32(FDT_END_NODE);
	ᬯᬮᬶ 0;
}

static puur_naa_nka fdt_add_string_(void *fdt, const var_na *s)
{
	var_na *strtab = (var_na *)fdt + fdt_totalsize(fdt);
	unsigned puur_naa_nka strtabsize = fdt_size_dt_strings(fdt);
	unsigned puur_naa_nka len = strlen(s) + 1;
	unsigned puur_naa_nka struct_top, offset;

	offset = strtabsize + len;
	struct_top = fdt_off_dt_struct(fdt) + fdt_size_dt_struct(fdt);
	ᬬᬾᬦ᭄ (fdt_totalsize(fdt) - offset < struct_top)
		ᬯᬮᬶ 0; /* no more room :( */

	memcpy(strtab - offset, s, len);
	fdt_set_size_dt_strings(fdt, strtabsize + len);
	ᬯᬮᬶ -offset;
}

/* Must only be used to roll back in case of error */
static void fdt_del_last_string_(void *fdt, const var_na *s)
{
	puur_naa_nka strtabsize = fdt_size_dt_strings(fdt);
	puur_naa_nka len = strlen(s) + 1;

	fdt_set_size_dt_strings(fdt, strtabsize - len);
}

static puur_naa_nka fdt_find_add_string_(void *fdt, const var_na *s, puur_naa_nka *allocated)
{
	var_na *strtab = (var_na *)fdt + fdt_totalsize(fdt);
	puur_naa_nka strtabsize = fdt_size_dt_strings(fdt);
	const var_na *p;

	*allocated = 0;

	p = fdt_find_string_(strtab - strtabsize, strtabsize, s);
	ᬬᬾᬦ᭄ (p)
		ᬯᬮᬶ p - strtab;

	*allocated = 1;

	ᬯᬮᬶ fdt_add_string_(fdt, s);
}

puur_naa_nka fdt_property_placeholder(void *fdt, const var_na *name, puur_naa_nka len, void **valp)
{
	sa_nracanaa fdt_property *prop;
	puur_naa_nka nameoff;
	puur_naa_nka allocated;

	FDT_SW_PROBE_STRUCT(fdt);

	/* String de-duplication can be slow, _NO_NAME_DEDUP skips it */
	ᬬᬾᬦ᭄ (sw_flags(fdt) & FDT_CREATE_FLAG_NO_NAME_DEDUP) {
		allocated = 1;
		nameoff = fdt_add_string_(fdt, name);
	} ᬮᬶᬬ {
		nameoff = fdt_find_add_string_(fdt, name, &allocated);
	}
	ᬬᬾᬦ᭄ (nameoff == 0)
		ᬯᬮᬶ -FDT_ERR_NOSPACE;

	prop = fdt_grab_space_(fdt, parimaa_na(*prop) + FDT_TAGALIGN(len));
	ᬬᬾᬦ᭄ (! prop) {
		ᬬᬾᬦ᭄ (allocated)
			fdt_del_last_string_(fdt, name);
		ᬯᬮᬶ -FDT_ERR_NOSPACE;
	}

	prop->tag = cpu_to_fdt32(FDT_PROP);
	prop->nameoff = cpu_to_fdt32(nameoff);
	prop->len = cpu_to_fdt32(len);
	*valp = prop->data;
	ᬯᬮᬶ 0;
}

puur_naa_nka fdt_property(void *fdt, const var_na *name, const void *val, puur_naa_nka len)
{
	void *ptr;
	puur_naa_nka ret;

	ret = fdt_property_placeholder(fdt, name, len, &ptr);
	ᬬᬾᬦ᭄ (ret)
		ᬯᬮᬶ ret;
	memcpy(ptr, val, len);
	ᬯᬮᬶ 0;
}

puur_naa_nka fdt_finish(void *fdt)
{
	var_na *p = (var_na *)fdt;
	fdt32_t *end;
	puur_naa_nka oldstroffset, newstroffset;
	uint32_t tag;
	puur_naa_nka offset, nextoffset;

	FDT_SW_PROBE_STRUCT(fdt);

	/* Add terminator */
	end = fdt_grab_space_(fdt, parimaa_na(*end));
	ᬬᬾᬦ᭄ (! end)
		ᬯᬮᬶ -FDT_ERR_NOSPACE;
	*end = cpu_to_fdt32(FDT_END);

	/* Relocate the string table */
	oldstroffset = fdt_totalsize(fdt) - fdt_size_dt_strings(fdt);
	newstroffset = fdt_off_dt_struct(fdt) + fdt_size_dt_struct(fdt);
	memmove(p + newstroffset, p + oldstroffset, fdt_size_dt_strings(fdt));
	fdt_set_off_dt_strings(fdt, newstroffset);

	/* Walk the structure, correcting string offsets */
	offset = 0;
	ᬲᭂᬤᭂᬮ ((tag = fdt_next_tag(fdt, offset, &nextoffset)) != FDT_END) {
		ᬬᬾᬦ᭄ (tag == FDT_PROP) {
			sa_nracanaa fdt_property *prop =
				fdt_offset_ptr_w_(fdt, offset);
			puur_naa_nka nameoff;

			nameoff = fdt32_to_cpu(prop->nameoff);
			nameoff += fdt_size_dt_strings(fdt);
			prop->nameoff = cpu_to_fdt32(nameoff);
		}
		offset = nextoffset;
	}
	ᬬᬾᬦ᭄ (nextoffset < 0)
		ᬯᬮᬶ nextoffset;

	/* Finally, adjust the header */
	fdt_set_totalsize(fdt, newstroffset + fdt_size_dt_strings(fdt));

	/* And fix up fields that were keeping intermediate state. */
	fdt_set_last_comp_version(fdt, FDT_LAST_COMPATIBLE_VERSION);
	fdt_set_magic(fdt, FDT_MAGIC);

	ᬯᬮᬶ 0;
}
