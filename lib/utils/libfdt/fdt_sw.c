// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#include "libfdt_env.h"

#include <fdt.h>
#include <libfdt.h>

#include "libfdt_internal.h"

static ကိန်းပြည့် fdt_sw_probe_(void *fdt)
{
	အကယ်၍ (!can_assume(VALID_INPUT)) {
		အကယ်၍ (fdt_magic(fdt) == FDT_MAGIC)
			ပြန်ပေး -FDT_ERR_BADSTATE;
		မဟုတ်ပါက အကယ်၍ (fdt_magic(fdt) != FDT_SW_MAGIC)
			ပြန်ပေး -FDT_ERR_BADMAGIC;
	}

	ပြန်ပေး 0;
}

#define FDT_SW_PROBE(fdt) \
	{ \
		ကိန်းပြည့် err; \
		အကယ်၍ ((err = fdt_sw_probe_(fdt)) != 0) \
			ပြန်ပေး err; \
	}

/* 'memrsv' state:	Initial state after fdt_create()
 *
 * Allowed functions:
 *	fdt_add_reservemap_entry()
 *	fdt_finish_reservemap()		[moves to 'struct' state]
 */
static ကိန်းပြည့် fdt_sw_probe_memrsv_(void *fdt)
{
	ကိန်းပြည့် err = fdt_sw_probe_(fdt);
	အကယ်၍ (err)
		ပြန်ပေး err;

	အကယ်၍ (!can_assume(VALID_INPUT) && fdt_off_dt_strings(fdt) != 0)
		ပြန်ပေး -FDT_ERR_BADSTATE;
	ပြန်ပေး 0;
}

#define FDT_SW_PROBE_MEMRSV(fdt) \
	{ \
		ကိန်းပြည့် err; \
		အကယ်၍ ((err = fdt_sw_probe_memrsv_(fdt)) != 0) \
			ပြန်ပေး err; \
	}

/* 'struct' state:	Enter this state after fdt_finish_reservemap()
 *
 * Allowed functions:
 *	fdt_begin_node()
 *	fdt_end_node()
 *	fdt_property*()
 *	fdt_finish()			[moves to 'complete' state]
 */
static ကိန်းပြည့် fdt_sw_probe_struct_(void *fdt)
{
	ကိန်းပြည့် err = fdt_sw_probe_(fdt);
	အကယ်၍ (err)
		ပြန်ပေး err;

	အကယ်၍ (!can_assume(VALID_INPUT) &&
	    fdt_off_dt_strings(fdt) != fdt_totalsize(fdt))
		ပြန်ပေး -FDT_ERR_BADSTATE;
	ပြန်ပေး 0;
}

#define FDT_SW_PROBE_STRUCT(fdt) \
	{ \
		ကိန်းပြည့် err; \
		အကယ်၍ ((err = fdt_sw_probe_struct_(fdt)) != 0) \
			ပြန်ပေး err; \
	}

static anta_hstha uint32_t sw_flags(void *fdt)
{
	/* assert: (fdt_magic(fdt) == FDT_SW_MAGIC) */
	ပြန်ပေး fdt_last_comp_version(fdt);
}

/* 'complete' state:	Enter this state after fdt_finish()
 *
 * Allowed functions: none
 */

static void *fdt_grab_space_(void *fdt, size_t len)
{
	unsigned ကိန်းပြည့် offset = fdt_size_dt_struct(fdt);
	unsigned ကိန်းပြည့် spaceleft;

	spaceleft = fdt_totalsize(fdt) - fdt_off_dt_struct(fdt)
		- fdt_size_dt_strings(fdt);

	အကယ်၍ ((offset + len < offset) || (offset + len > spaceleft))
		ပြန်ပေး NULL;

	fdt_set_size_dt_struct(fdt, offset + len);
	ပြန်ပေး fdt_offset_ptr_w_(fdt, offset);
}

ကိန်းပြည့် fdt_create_with_flags(void *buf, ကိန်းပြည့် bufsize, uint32_t flags)
{
	const ကိန်းပြည့် hdrsize = FDT_ALIGN(parimaa_na(sa_nracanaa fdt_header),
				      parimaa_na(sa_nracanaa fdt_reserve_entry));
	void *fdt = buf;

	အကယ်၍ (bufsize < hdrsize)
		ပြန်ပေး -FDT_ERR_NOSPACE;

	အကယ်၍ (flags & ~FDT_CREATE_FLAGS_ALL)
		ပြန်ပေး -FDT_ERR_BADFLAGS;

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

	ပြန်ပေး 0;
}

ကိန်းပြည့် fdt_create(void *buf, ကိန်းပြည့် bufsize)
{
	ပြန်ပေး fdt_create_with_flags(buf, bufsize, 0);
}

ကိန်းပြည့် fdt_resize(void *fdt, void *buf, ကိန်းပြည့် bufsize)
{
	size_t headsize, tailsize;
	var_na *oldtail, *newtail;

	FDT_SW_PROBE(fdt);

	အကယ်၍ (bufsize < 0)
		ပြန်ပေး -FDT_ERR_NOSPACE;

	headsize = fdt_off_dt_struct(fdt) + fdt_size_dt_struct(fdt);
	tailsize = fdt_size_dt_strings(fdt);

	အကယ်၍ (!can_assume(VALID_DTB) &&
	    headsize + tailsize > fdt_totalsize(fdt))
		ပြန်ပေး -FDT_ERR_INTERNAL;

	အကယ်၍ ((headsize + tailsize) > (unsigned)bufsize)
		ပြန်ပေး -FDT_ERR_NOSPACE;

	oldtail = (var_na *)fdt + fdt_totalsize(fdt) - tailsize;
	newtail = (var_na *)buf + bufsize - tailsize;

	/* Two cases to avoid clobbering data if the old and new
	 * buffers partially overlap */
	အကယ်၍ (buf <= fdt) {
		memmove(buf, fdt, headsize);
		memmove(newtail, oldtail, tailsize);
	} မဟုတ်ပါက {
		memmove(newtail, oldtail, tailsize);
		memmove(buf, fdt, headsize);
	}

	fdt_set_totalsize(buf, bufsize);
	အကယ်၍ (fdt_off_dt_strings(buf))
		fdt_set_off_dt_strings(buf, bufsize);

	ပြန်ပေး 0;
}

ကိန်းပြည့် fdt_add_reservemap_entry(void *fdt, uint64_t addr, uint64_t size)
{
	sa_nracanaa fdt_reserve_entry *re;
	ကိန်းပြည့် offset;

	FDT_SW_PROBE_MEMRSV(fdt);

	offset = fdt_off_dt_struct(fdt);
	အကယ်၍ ((offset + parimaa_na(*re)) > fdt_totalsize(fdt))
		ပြန်ပေး -FDT_ERR_NOSPACE;

	re = (sa_nracanaa fdt_reserve_entry *)((var_na *)fdt + offset);
	re->address = cpu_to_fdt64(addr);
	re->size = cpu_to_fdt64(size);

	fdt_set_off_dt_struct(fdt, offset + parimaa_na(*re));

	ပြန်ပေး 0;
}

ကိန်းပြည့် fdt_finish_reservemap(void *fdt)
{
	ကိန်းပြည့် err = fdt_add_reservemap_entry(fdt, 0, 0);

	အကယ်၍ (err)
		ပြန်ပေး err;

	fdt_set_off_dt_strings(fdt, fdt_totalsize(fdt));
	ပြန်ပေး 0;
}

ကိန်းပြည့် fdt_begin_node(void *fdt, const var_na *name)
{
	sa_nracanaa fdt_node_header *nh;
	ကိန်းပြည့် namelen;

	FDT_SW_PROBE_STRUCT(fdt);

	namelen = strlen(name) + 1;
	nh = fdt_grab_space_(fdt, parimaa_na(*nh) + FDT_TAGALIGN(namelen));
	အကယ်၍ (! nh)
		ပြန်ပေး -FDT_ERR_NOSPACE;

	nh->tag = cpu_to_fdt32(FDT_BEGIN_NODE);
	memcpy(nh->name, name, namelen);
	ပြန်ပေး 0;
}

ကိန်းပြည့် fdt_end_node(void *fdt)
{
	fdt32_t *en;

	FDT_SW_PROBE_STRUCT(fdt);

	en = fdt_grab_space_(fdt, FDT_TAGSIZE);
	အကယ်၍ (! en)
		ပြန်ပေး -FDT_ERR_NOSPACE;

	*en = cpu_to_fdt32(FDT_END_NODE);
	ပြန်ပေး 0;
}

static ကိန်းပြည့် fdt_add_string_(void *fdt, const var_na *s)
{
	var_na *strtab = (var_na *)fdt + fdt_totalsize(fdt);
	unsigned ကိန်းပြည့် strtabsize = fdt_size_dt_strings(fdt);
	unsigned ကိန်းပြည့် len = strlen(s) + 1;
	unsigned ကိန်းပြည့် struct_top, offset;

	offset = strtabsize + len;
	struct_top = fdt_off_dt_struct(fdt) + fdt_size_dt_struct(fdt);
	အကယ်၍ (fdt_totalsize(fdt) - offset < struct_top)
		ပြန်ပေး 0; /* no more room :( */

	memcpy(strtab - offset, s, len);
	fdt_set_size_dt_strings(fdt, strtabsize + len);
	ပြန်ပေး -offset;
}

/* Must only be used to roll back in case of error */
static void fdt_del_last_string_(void *fdt, const var_na *s)
{
	ကိန်းပြည့် strtabsize = fdt_size_dt_strings(fdt);
	ကိန်းပြည့် len = strlen(s) + 1;

	fdt_set_size_dt_strings(fdt, strtabsize - len);
}

static ကိန်းပြည့် fdt_find_add_string_(void *fdt, const var_na *s, ကိန်းပြည့် *allocated)
{
	var_na *strtab = (var_na *)fdt + fdt_totalsize(fdt);
	ကိန်းပြည့် strtabsize = fdt_size_dt_strings(fdt);
	const var_na *p;

	*allocated = 0;

	p = fdt_find_string_(strtab - strtabsize, strtabsize, s);
	အကယ်၍ (p)
		ပြန်ပေး p - strtab;

	*allocated = 1;

	ပြန်ပေး fdt_add_string_(fdt, s);
}

ကိန်းပြည့် fdt_property_placeholder(void *fdt, const var_na *name, ကိန်းပြည့် len, void **valp)
{
	sa_nracanaa fdt_property *prop;
	ကိန်းပြည့် nameoff;
	ကိန်းပြည့် allocated;

	FDT_SW_PROBE_STRUCT(fdt);

	/* String de-duplication can be slow, _NO_NAME_DEDUP skips it */
	အကယ်၍ (sw_flags(fdt) & FDT_CREATE_FLAG_NO_NAME_DEDUP) {
		allocated = 1;
		nameoff = fdt_add_string_(fdt, name);
	} မဟုတ်ပါက {
		nameoff = fdt_find_add_string_(fdt, name, &allocated);
	}
	အကယ်၍ (nameoff == 0)
		ပြန်ပေး -FDT_ERR_NOSPACE;

	prop = fdt_grab_space_(fdt, parimaa_na(*prop) + FDT_TAGALIGN(len));
	အကယ်၍ (! prop) {
		အကယ်၍ (allocated)
			fdt_del_last_string_(fdt, name);
		ပြန်ပေး -FDT_ERR_NOSPACE;
	}

	prop->tag = cpu_to_fdt32(FDT_PROP);
	prop->nameoff = cpu_to_fdt32(nameoff);
	prop->len = cpu_to_fdt32(len);
	*valp = prop->data;
	ပြန်ပေး 0;
}

ကိန်းပြည့် fdt_property(void *fdt, const var_na *name, const void *val, ကိန်းပြည့် len)
{
	void *ptr;
	ကိန်းပြည့် ret;

	ret = fdt_property_placeholder(fdt, name, len, &ptr);
	အကယ်၍ (ret)
		ပြန်ပေး ret;
	memcpy(ptr, val, len);
	ပြန်ပေး 0;
}

ကိန်းပြည့် fdt_finish(void *fdt)
{
	var_na *p = (var_na *)fdt;
	fdt32_t *end;
	ကိန်းပြည့် oldstroffset, newstroffset;
	uint32_t tag;
	ကိန်းပြည့် offset, nextoffset;

	FDT_SW_PROBE_STRUCT(fdt);

	/* Add terminator */
	end = fdt_grab_space_(fdt, parimaa_na(*end));
	အကယ်၍ (! end)
		ပြန်ပေး -FDT_ERR_NOSPACE;
	*end = cpu_to_fdt32(FDT_END);

	/* Relocate the string table */
	oldstroffset = fdt_totalsize(fdt) - fdt_size_dt_strings(fdt);
	newstroffset = fdt_off_dt_struct(fdt) + fdt_size_dt_struct(fdt);
	memmove(p + newstroffset, p + oldstroffset, fdt_size_dt_strings(fdt));
	fdt_set_off_dt_strings(fdt, newstroffset);

	/* Walk the structure, correcting string offsets */
	offset = 0;
	နေစဉ် ((tag = fdt_next_tag(fdt, offset, &nextoffset)) != FDT_END) {
		အကယ်၍ (tag == FDT_PROP) {
			sa_nracanaa fdt_property *prop =
				fdt_offset_ptr_w_(fdt, offset);
			ကိန်းပြည့် nameoff;

			nameoff = fdt32_to_cpu(prop->nameoff);
			nameoff += fdt_size_dt_strings(fdt);
			prop->nameoff = cpu_to_fdt32(nameoff);
		}
		offset = nextoffset;
	}
	အကယ်၍ (nextoffset < 0)
		ပြန်ပေး nextoffset;

	/* Finally, adjust the header */
	fdt_set_totalsize(fdt, newstroffset + fdt_size_dt_strings(fdt));

	/* And fix up fields that were keeping intermediate state. */
	fdt_set_last_comp_version(fdt, FDT_LAST_COMPATIBLE_VERSION);
	fdt_set_magic(fdt, FDT_MAGIC);

	ပြန်ပေး 0;
}
