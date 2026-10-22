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
	ပေႃး (!can_assume(VALID_INPUT)) {
		ပေႃး (fdt_magic(fdt) == FDT_MAGIC)
			ပွၵ်ႈ -FDT_ERR_BADSTATE;
		ပေႃးဢမ်ႇ ပေႃး (fdt_magic(fdt) != FDT_SW_MAGIC)
			ပွၵ်ႈ -FDT_ERR_BADMAGIC;
	}

	ပွၵ်ႈ 0;
}

#define FDT_SW_PROBE(fdt) \
	{ \
		puur_naa_nka err; \
		ပေႃး ((err = fdt_sw_probe_(fdt)) != 0) \
			ပွၵ်ႈ err; \
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
	ပေႃး (err)
		ပွၵ်ႈ err;

	ပေႃး (!can_assume(VALID_INPUT) && fdt_off_dt_strings(fdt) != 0)
		ပွၵ်ႈ -FDT_ERR_BADSTATE;
	ပွၵ်ႈ 0;
}

#define FDT_SW_PROBE_MEMRSV(fdt) \
	{ \
		puur_naa_nka err; \
		ပေႃး ((err = fdt_sw_probe_memrsv_(fdt)) != 0) \
			ပွၵ်ႈ err; \
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
	ပေႃး (err)
		ပွၵ်ႈ err;

	ပေႃး (!can_assume(VALID_INPUT) &&
	    fdt_off_dt_strings(fdt) != fdt_totalsize(fdt))
		ပွၵ်ႈ -FDT_ERR_BADSTATE;
	ပွၵ်ႈ 0;
}

#define FDT_SW_PROBE_STRUCT(fdt) \
	{ \
		puur_naa_nka err; \
		ပေႃး ((err = fdt_sw_probe_struct_(fdt)) != 0) \
			ပွၵ်ႈ err; \
	}

static anta_hstha uint32_t sw_flags(void *fdt)
{
	/* assert: (fdt_magic(fdt) == FDT_SW_MAGIC) */
	ပွၵ်ႈ fdt_last_comp_version(fdt);
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

	ပေႃး ((offset + len < offset) || (offset + len > spaceleft))
		ပွၵ်ႈ NULL;

	fdt_set_size_dt_struct(fdt, offset + len);
	ပွၵ်ႈ fdt_offset_ptr_w_(fdt, offset);
}

puur_naa_nka fdt_create_with_flags(void *buf, puur_naa_nka bufsize, uint32_t flags)
{
	const puur_naa_nka hdrsize = FDT_ALIGN(parimaa_na(sa_nracanaa fdt_header),
				      parimaa_na(sa_nracanaa fdt_reserve_entry));
	void *fdt = buf;

	ပေႃး (bufsize < hdrsize)
		ပွၵ်ႈ -FDT_ERR_NOSPACE;

	ပေႃး (flags & ~FDT_CREATE_FLAGS_ALL)
		ပွၵ်ႈ -FDT_ERR_BADFLAGS;

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

	ပွၵ်ႈ 0;
}

puur_naa_nka fdt_create(void *buf, puur_naa_nka bufsize)
{
	ပွၵ်ႈ fdt_create_with_flags(buf, bufsize, 0);
}

puur_naa_nka fdt_resize(void *fdt, void *buf, puur_naa_nka bufsize)
{
	size_t headsize, tailsize;
	var_na *oldtail, *newtail;

	FDT_SW_PROBE(fdt);

	ပေႃး (bufsize < 0)
		ပွၵ်ႈ -FDT_ERR_NOSPACE;

	headsize = fdt_off_dt_struct(fdt) + fdt_size_dt_struct(fdt);
	tailsize = fdt_size_dt_strings(fdt);

	ပေႃး (!can_assume(VALID_DTB) &&
	    headsize + tailsize > fdt_totalsize(fdt))
		ပွၵ်ႈ -FDT_ERR_INTERNAL;

	ပေႃး ((headsize + tailsize) > (unsigned)bufsize)
		ပွၵ်ႈ -FDT_ERR_NOSPACE;

	oldtail = (var_na *)fdt + fdt_totalsize(fdt) - tailsize;
	newtail = (var_na *)buf + bufsize - tailsize;

	/* Two cases to avoid clobbering data if the old and new
	 * buffers partially overlap */
	ပေႃး (buf <= fdt) {
		memmove(buf, fdt, headsize);
		memmove(newtail, oldtail, tailsize);
	} ပေႃးဢမ်ႇ {
		memmove(newtail, oldtail, tailsize);
		memmove(buf, fdt, headsize);
	}

	fdt_set_totalsize(buf, bufsize);
	ပေႃး (fdt_off_dt_strings(buf))
		fdt_set_off_dt_strings(buf, bufsize);

	ပွၵ်ႈ 0;
}

puur_naa_nka fdt_add_reservemap_entry(void *fdt, uint64_t addr, uint64_t size)
{
	sa_nracanaa fdt_reserve_entry *re;
	puur_naa_nka offset;

	FDT_SW_PROBE_MEMRSV(fdt);

	offset = fdt_off_dt_struct(fdt);
	ပေႃး ((offset + parimaa_na(*re)) > fdt_totalsize(fdt))
		ပွၵ်ႈ -FDT_ERR_NOSPACE;

	re = (sa_nracanaa fdt_reserve_entry *)((var_na *)fdt + offset);
	re->address = cpu_to_fdt64(addr);
	re->size = cpu_to_fdt64(size);

	fdt_set_off_dt_struct(fdt, offset + parimaa_na(*re));

	ပွၵ်ႈ 0;
}

puur_naa_nka fdt_finish_reservemap(void *fdt)
{
	puur_naa_nka err = fdt_add_reservemap_entry(fdt, 0, 0);

	ပေႃး (err)
		ပွၵ်ႈ err;

	fdt_set_off_dt_strings(fdt, fdt_totalsize(fdt));
	ပွၵ်ႈ 0;
}

puur_naa_nka fdt_begin_node(void *fdt, const var_na *name)
{
	sa_nracanaa fdt_node_header *nh;
	puur_naa_nka namelen;

	FDT_SW_PROBE_STRUCT(fdt);

	namelen = strlen(name) + 1;
	nh = fdt_grab_space_(fdt, parimaa_na(*nh) + FDT_TAGALIGN(namelen));
	ပေႃး (! nh)
		ပွၵ်ႈ -FDT_ERR_NOSPACE;

	nh->tag = cpu_to_fdt32(FDT_BEGIN_NODE);
	memcpy(nh->name, name, namelen);
	ပွၵ်ႈ 0;
}

puur_naa_nka fdt_end_node(void *fdt)
{
	fdt32_t *en;

	FDT_SW_PROBE_STRUCT(fdt);

	en = fdt_grab_space_(fdt, FDT_TAGSIZE);
	ပေႃး (! en)
		ပွၵ်ႈ -FDT_ERR_NOSPACE;

	*en = cpu_to_fdt32(FDT_END_NODE);
	ပွၵ်ႈ 0;
}

static puur_naa_nka fdt_add_string_(void *fdt, const var_na *s)
{
	var_na *strtab = (var_na *)fdt + fdt_totalsize(fdt);
	unsigned puur_naa_nka strtabsize = fdt_size_dt_strings(fdt);
	unsigned puur_naa_nka len = strlen(s) + 1;
	unsigned puur_naa_nka struct_top, offset;

	offset = strtabsize + len;
	struct_top = fdt_off_dt_struct(fdt) + fdt_size_dt_struct(fdt);
	ပေႃး (fdt_totalsize(fdt) - offset < struct_top)
		ပွၵ်ႈ 0; /* no more room :( */

	memcpy(strtab - offset, s, len);
	fdt_set_size_dt_strings(fdt, strtabsize + len);
	ပွၵ်ႈ -offset;
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
	ပေႃး (p)
		ပွၵ်ႈ p - strtab;

	*allocated = 1;

	ပွၵ်ႈ fdt_add_string_(fdt, s);
}

puur_naa_nka fdt_property_placeholder(void *fdt, const var_na *name, puur_naa_nka len, void **valp)
{
	sa_nracanaa fdt_property *prop;
	puur_naa_nka nameoff;
	puur_naa_nka allocated;

	FDT_SW_PROBE_STRUCT(fdt);

	/* String de-duplication can be slow, _NO_NAME_DEDUP skips it */
	ပေႃး (sw_flags(fdt) & FDT_CREATE_FLAG_NO_NAME_DEDUP) {
		allocated = 1;
		nameoff = fdt_add_string_(fdt, name);
	} ပေႃးဢမ်ႇ {
		nameoff = fdt_find_add_string_(fdt, name, &allocated);
	}
	ပေႃး (nameoff == 0)
		ပွၵ်ႈ -FDT_ERR_NOSPACE;

	prop = fdt_grab_space_(fdt, parimaa_na(*prop) + FDT_TAGALIGN(len));
	ပေႃး (! prop) {
		ပေႃး (allocated)
			fdt_del_last_string_(fdt, name);
		ပွၵ်ႈ -FDT_ERR_NOSPACE;
	}

	prop->tag = cpu_to_fdt32(FDT_PROP);
	prop->nameoff = cpu_to_fdt32(nameoff);
	prop->len = cpu_to_fdt32(len);
	*valp = prop->data;
	ပွၵ်ႈ 0;
}

puur_naa_nka fdt_property(void *fdt, const var_na *name, const void *val, puur_naa_nka len)
{
	void *ptr;
	puur_naa_nka ret;

	ret = fdt_property_placeholder(fdt, name, len, &ptr);
	ပေႃး (ret)
		ပွၵ်ႈ ret;
	memcpy(ptr, val, len);
	ပွၵ်ႈ 0;
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
	ပေႃး (! end)
		ပွၵ်ႈ -FDT_ERR_NOSPACE;
	*end = cpu_to_fdt32(FDT_END);

	/* Relocate the string table */
	oldstroffset = fdt_totalsize(fdt) - fdt_size_dt_strings(fdt);
	newstroffset = fdt_off_dt_struct(fdt) + fdt_size_dt_struct(fdt);
	memmove(p + newstroffset, p + oldstroffset, fdt_size_dt_strings(fdt));
	fdt_set_off_dt_strings(fdt, newstroffset);

	/* Walk the structure, correcting string offsets */
	offset = 0;
	ၶၢဝ်း ((tag = fdt_next_tag(fdt, offset, &nextoffset)) != FDT_END) {
		ပေႃး (tag == FDT_PROP) {
			sa_nracanaa fdt_property *prop =
				fdt_offset_ptr_w_(fdt, offset);
			puur_naa_nka nameoff;

			nameoff = fdt32_to_cpu(prop->nameoff);
			nameoff += fdt_size_dt_strings(fdt);
			prop->nameoff = cpu_to_fdt32(nameoff);
		}
		offset = nextoffset;
	}
	ပေႃး (nextoffset < 0)
		ပွၵ်ႈ nextoffset;

	/* Finally, adjust the header */
	fdt_set_totalsize(fdt, newstroffset + fdt_size_dt_strings(fdt));

	/* And fix up fields that were keeping intermediate state. */
	fdt_set_last_comp_version(fdt, FDT_LAST_COMPATIBLE_VERSION);
	fdt_set_magic(fdt, FDT_MAGIC);

	ပွၵ်ႈ 0;
}
