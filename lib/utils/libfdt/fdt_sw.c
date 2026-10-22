// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#அந்தர்பாவ "libfdt_env.h"

#அந்தர்பாவ <fdt.h>
#அந்தர்பாவ <libfdt.h>

#அந்தர்பாவ "libfdt_internal.h"

நிலையான முழு fdt_sw_probe_(வெறுமை *fdt)
{
	எனில் (!can_assume(VALID_உள்ளீடு)) {
		எனில் (fdt_magic(fdt) == FDT_MAGIC)
			திரும்பு -FDT_ERR_BADSTATE;
		இல்லையெனில் எனில் (fdt_magic(fdt) != FDT_SW_MAGIC)
			திரும்பு -FDT_ERR_BADMAGIC;
	}

	திரும்பு 0;
}

#define FDT_SW_PROBE(fdt) \
	{ \
		முழு err; \
		எனில் ((err = fdt_sw_probe_(fdt)) != 0) \
			திரும்பு err; \
	}

/* 'memrsv' state:	Initial state after fdt_create()
 *
 * Allowed functions:
 *	fdt_add_reservemap_entry()
 *	fdt_finish_reservemap()		[moves to 'struct' state]
 */
நிலையான முழு fdt_sw_probe_memrsv_(வெறுமை *fdt)
{
	முழு err = fdt_sw_probe_(fdt);
	எனில் (err)
		திரும்பு err;

	எனில் (!can_assume(VALID_உள்ளீடு) && fdt_off_dt_strings(fdt) != 0)
		திரும்பு -FDT_ERR_BADSTATE;
	திரும்பு 0;
}

#define FDT_SW_PROBE_MEMRSV(fdt) \
	{ \
		முழு err; \
		எனில் ((err = fdt_sw_probe_memrsv_(fdt)) != 0) \
			திரும்பு err; \
	}

/* 'struct' state:	Enter this state after fdt_finish_reservemap()
 *
 * Allowed functions:
 *	fdt_begin_node()
 *	fdt_end_node()
 *	fdt_property*()
 *	fdt_finish()			[moves to 'complete' state]
 */
நிலையான முழு fdt_sw_probe_struct_(வெறுமை *fdt)
{
	முழு err = fdt_sw_probe_(fdt);
	எனில் (err)
		திரும்பு err;

	எனில் (!can_assume(VALID_உள்ளீடு) &&
	    fdt_off_dt_strings(fdt) != fdt_totalsize(fdt))
		திரும்பு -FDT_ERR_BADSTATE;
	திரும்பு 0;
}

#define FDT_SW_PROBE_STRUCT(fdt) \
	{ \
		முழு err; \
		எனில் ((err = fdt_sw_probe_struct_(fdt)) != 0) \
			திரும்பு err; \
	}

நிலையான உள்வரி uint32_t sw_flags(வெறுமை *fdt)
{
	/* assert: (fdt_magic(fdt) == FDT_SW_MAGIC) */
	திரும்பு fdt_last_comp_version(fdt);
}

/* 'complete' state:	Enter this state after fdt_finish()
 *
 * Allowed functions: none
 */

நிலையான வெறுமை *fdt_grab_space_(வெறுமை *fdt, size_t len)
{
	unsigned முழு offset = fdt_size_dt_struct(fdt);
	unsigned முழு spaceleft;

	spaceleft = fdt_totalsize(fdt) - fdt_off_dt_struct(fdt)
		- fdt_size_dt_strings(fdt);

	எனில் ((offset + len < offset) || (offset + len > spaceleft))
		திரும்பு வெற்று;

	fdt_set_size_dt_struct(fdt, offset + len);
	திரும்பு fdt_offset_ptr_w_(fdt, offset);
}

முழு fdt_create_with_flags(வெறுமை *buf, முழு bufsize, uint32_t flags)
{
	மாறிலி முழு hdrsize = FDT_ALIGN(அளவு(கட்டமைப்பு fdt_header),
				      அளவு(கட்டமைப்பு fdt_reserve_entry));
	வெறுமை *fdt = buf;

	எனில் (bufsize < hdrsize)
		திரும்பு -FDT_ERR_NOSPACE;

	எனில் (flags & ~FDT_CREATE_FLAGS_ALL)
		திரும்பு -FDT_ERR_BADFLAGS;

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

	திரும்பு 0;
}

முழு fdt_create(வெறுமை *buf, முழு bufsize)
{
	திரும்பு fdt_create_with_flags(buf, bufsize, 0);
}

முழு fdt_resize(வெறுமை *fdt, வெறுமை *buf, முழு bufsize)
{
	size_t headsize, tailsize;
	எழுத்து *oldtail, *newtail;

	FDT_SW_PROBE(fdt);

	எனில் (bufsize < 0)
		திரும்பு -FDT_ERR_NOSPACE;

	headsize = fdt_off_dt_struct(fdt) + fdt_size_dt_struct(fdt);
	tailsize = fdt_size_dt_strings(fdt);

	எனில் (!can_assume(VALID_DTB) &&
	    headsize + tailsize > fdt_totalsize(fdt))
		திரும்பு -FDT_ERR_INTERNAL;

	எனில் ((headsize + tailsize) > (unsigned)bufsize)
		திரும்பு -FDT_ERR_NOSPACE;

	oldtail = (எழுத்து *)fdt + fdt_totalsize(fdt) - tailsize;
	newtail = (எழுத்து *)buf + bufsize - tailsize;

	/* Two cases to avoid clobbering data if the old and new
	 * buffers partially overlap */
	எனில் (buf <= fdt) {
		memmove(buf, fdt, headsize);
		memmove(newtail, oldtail, tailsize);
	} இல்லையெனில் {
		memmove(newtail, oldtail, tailsize);
		memmove(buf, fdt, headsize);
	}

	fdt_set_totalsize(buf, bufsize);
	எனில் (fdt_off_dt_strings(buf))
		fdt_set_off_dt_strings(buf, bufsize);

	திரும்பு 0;
}

முழு fdt_add_reservemap_entry(வெறுமை *fdt, uint64_t addr, uint64_t size)
{
	கட்டமைப்பு fdt_reserve_entry *re;
	முழு offset;

	FDT_SW_PROBE_MEMRSV(fdt);

	offset = fdt_off_dt_struct(fdt);
	எனில் ((offset + அளவு(*re)) > fdt_totalsize(fdt))
		திரும்பு -FDT_ERR_NOSPACE;

	re = (கட்டமைப்பு fdt_reserve_entry *)((எழுத்து *)fdt + offset);
	re->address = cpu_to_fdt64(addr);
	re->size = cpu_to_fdt64(size);

	fdt_set_off_dt_struct(fdt, offset + அளவு(*re));

	திரும்பு 0;
}

முழு fdt_finish_reservemap(வெறுமை *fdt)
{
	முழு err = fdt_add_reservemap_entry(fdt, 0, 0);

	எனில் (err)
		திரும்பு err;

	fdt_set_off_dt_strings(fdt, fdt_totalsize(fdt));
	திரும்பு 0;
}

முழு fdt_begin_முனை(வெறுமை *fdt, மாறிலி எழுத்து *name)
{
	கட்டமைப்பு fdt_முனை_header *nh;
	முழு namelen;

	FDT_SW_PROBE_STRUCT(fdt);

	namelen = strlen(name) + 1;
	nh = fdt_grab_space_(fdt, அளவு(*nh) + FDT_TAGALIGN(namelen));
	எனில் (! nh)
		திரும்பு -FDT_ERR_NOSPACE;

	nh->tag = cpu_to_fdt32(FDT_BEGIN_முனை);
	memcpy(nh->name, name, namelen);
	திரும்பு 0;
}

முழு fdt_end_முனை(வெறுமை *fdt)
{
	fdt32_t *en;

	FDT_SW_PROBE_STRUCT(fdt);

	en = fdt_grab_space_(fdt, FDT_TAGSIZE);
	எனில் (! en)
		திரும்பு -FDT_ERR_NOSPACE;

	*en = cpu_to_fdt32(FDT_END_முனை);
	திரும்பு 0;
}

நிலையான முழு fdt_add_string_(வெறுமை *fdt, மாறிலி எழுத்து *s)
{
	எழுத்து *strtab = (எழுத்து *)fdt + fdt_totalsize(fdt);
	unsigned முழு strtabsize = fdt_size_dt_strings(fdt);
	unsigned முழு len = strlen(s) + 1;
	unsigned முழு struct_உச்சி, offset;

	offset = strtabsize + len;
	struct_உச்சி = fdt_off_dt_struct(fdt) + fdt_size_dt_struct(fdt);
	எனில் (fdt_totalsize(fdt) - offset < struct_உச்சி)
		திரும்பு 0; /* no more room :( */

	memcpy(strtab - offset, s, len);
	fdt_set_size_dt_strings(fdt, strtabsize + len);
	திரும்பு -offset;
}

/* Must only be used to roll back in case of error */
நிலையான வெறுமை fdt_del_last_string_(வெறுமை *fdt, மாறிலி எழுத்து *s)
{
	முழு strtabsize = fdt_size_dt_strings(fdt);
	முழு len = strlen(s) + 1;

	fdt_set_size_dt_strings(fdt, strtabsize - len);
}

நிலையான முழு fdt_find_add_string_(வெறுமை *fdt, மாறிலி எழுத்து *s, முழு *allocated)
{
	எழுத்து *strtab = (எழுத்து *)fdt + fdt_totalsize(fdt);
	முழு strtabsize = fdt_size_dt_strings(fdt);
	மாறிலி எழுத்து *p;

	*allocated = 0;

	p = fdt_find_string_(strtab - strtabsize, strtabsize, s);
	எனில் (p)
		திரும்பு p - strtab;

	*allocated = 1;

	திரும்பு fdt_add_string_(fdt, s);
}

முழு fdt_property_placeholder(வெறுமை *fdt, மாறிலி எழுத்து *name, முழு len, வெறுமை **valp)
{
	கட்டமைப்பு fdt_property *prop;
	முழு nameoff;
	முழு allocated;

	FDT_SW_PROBE_STRUCT(fdt);

	/* String de-duplication can be slow, _NO_NAME_DEDUP skips it */
	எனில் (sw_flags(fdt) & FDT_CREATE_FLAG_NO_NAME_DEDUP) {
		allocated = 1;
		nameoff = fdt_add_string_(fdt, name);
	} இல்லையெனில் {
		nameoff = fdt_find_add_string_(fdt, name, &allocated);
	}
	எனில் (nameoff == 0)
		திரும்பு -FDT_ERR_NOSPACE;

	prop = fdt_grab_space_(fdt, அளவு(*prop) + FDT_TAGALIGN(len));
	எனில் (! prop) {
		எனில் (allocated)
			fdt_del_last_string_(fdt, name);
		திரும்பு -FDT_ERR_NOSPACE;
	}

	prop->tag = cpu_to_fdt32(FDT_PROP);
	prop->nameoff = cpu_to_fdt32(nameoff);
	prop->len = cpu_to_fdt32(len);
	*valp = prop->data;
	திரும்பு 0;
}

முழு fdt_property(வெறுமை *fdt, மாறிலி எழுத்து *name, மாறிலி வெறுமை *val, முழு len)
{
	வெறுமை *ptr;
	முழு ret;

	ret = fdt_property_placeholder(fdt, name, len, &ptr);
	எனில் (ret)
		திரும்பு ret;
	memcpy(ptr, val, len);
	திரும்பு 0;
}

முழு fdt_finish(வெறுமை *fdt)
{
	எழுத்து *p = (எழுத்து *)fdt;
	fdt32_t *end;
	முழு oldstroffset, newstroffset;
	uint32_t tag;
	முழு offset, nextoffset;

	FDT_SW_PROBE_STRUCT(fdt);

	/* Add terminator */
	end = fdt_grab_space_(fdt, அளவு(*end));
	எனில் (! end)
		திரும்பு -FDT_ERR_NOSPACE;
	*end = cpu_to_fdt32(FDT_END);

	/* Relocate the string table */
	oldstroffset = fdt_totalsize(fdt) - fdt_size_dt_strings(fdt);
	newstroffset = fdt_off_dt_struct(fdt) + fdt_size_dt_struct(fdt);
	memmove(p + newstroffset, p + oldstroffset, fdt_size_dt_strings(fdt));
	fdt_set_off_dt_strings(fdt, newstroffset);

	/* Walk the structure, correcting string offsets */
	offset = 0;
	போது ((tag = fdt_next_tag(fdt, offset, &nextoffset)) != FDT_END) {
		எனில் (tag == FDT_PROP) {
			கட்டமைப்பு fdt_property *prop =
				fdt_offset_ptr_w_(fdt, offset);
			முழு nameoff;

			nameoff = fdt32_to_cpu(prop->nameoff);
			nameoff += fdt_size_dt_strings(fdt);
			prop->nameoff = cpu_to_fdt32(nameoff);
		}
		offset = nextoffset;
	}
	எனில் (nextoffset < 0)
		திரும்பு nextoffset;

	/* Finally, adjust the header */
	fdt_set_totalsize(fdt, newstroffset + fdt_size_dt_strings(fdt));

	/* And fix up fields that were keeping intermediate state. */
	fdt_set_last_comp_version(fdt, FDT_LAST_COMPATIBLE_VERSION);
	fdt_set_magic(fdt, FDT_MAGIC);

	திரும்பு 0;
}
