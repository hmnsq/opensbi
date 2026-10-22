// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#അന്തര്ഭാവ "libfdt_env.h"

#അന്തര്ഭാവ <fdt.h>
#അന്തര്ഭാവ <libfdt.h>

#അന്തര്ഭാവ "libfdt_internal.h"

സ്ഥിതം പൂർണ്ണസംഖ്യ fdt_sw_probe_(ശൂന്യം *fdt)
{
	എങ്കിൽ (!can_assume(VALID_നിവേശം)) {
		എങ്കിൽ (fdt_magic(fdt) == FDT_MAGIC)
			തിരികെ -FDT_ERR_BADSTATE;
		അല്ലെങ്കിൽ എങ്കിൽ (fdt_magic(fdt) != FDT_SW_MAGIC)
			തിരികെ -FDT_ERR_BADMAGIC;
	}

	തിരികെ 0;
}

#define FDT_SW_PROBE(fdt) \
	{ \
		പൂർണ്ണസംഖ്യ err; \
		എങ്കിൽ ((err = fdt_sw_probe_(fdt)) != 0) \
			തിരികെ err; \
	}

/* 'memrsv' state:	Initial state after fdt_create()
 *
 * Allowed functions:
 *	fdt_add_reservemap_entry()
 *	fdt_finish_reservemap()		[moves to 'struct' state]
 */
സ്ഥിതം പൂർണ്ണസംഖ്യ fdt_sw_probe_memrsv_(ശൂന്യം *fdt)
{
	പൂർണ്ണസംഖ്യ err = fdt_sw_probe_(fdt);
	എങ്കിൽ (err)
		തിരികെ err;

	എങ്കിൽ (!can_assume(VALID_നിവേശം) && fdt_off_dt_strings(fdt) != 0)
		തിരികെ -FDT_ERR_BADSTATE;
	തിരികെ 0;
}

#define FDT_SW_PROBE_MEMRSV(fdt) \
	{ \
		പൂർണ്ണസംഖ്യ err; \
		എങ്കിൽ ((err = fdt_sw_probe_memrsv_(fdt)) != 0) \
			തിരികെ err; \
	}

/* 'struct' state:	Enter this state after fdt_finish_reservemap()
 *
 * Allowed functions:
 *	fdt_begin_node()
 *	fdt_end_node()
 *	fdt_property*()
 *	fdt_finish()			[moves to 'complete' state]
 */
സ്ഥിതം പൂർണ്ണസംഖ്യ fdt_sw_probe_struct_(ശൂന്യം *fdt)
{
	പൂർണ്ണസംഖ്യ err = fdt_sw_probe_(fdt);
	എങ്കിൽ (err)
		തിരികെ err;

	എങ്കിൽ (!can_assume(VALID_നിവേശം) &&
	    fdt_off_dt_strings(fdt) != fdt_totalsize(fdt))
		തിരികെ -FDT_ERR_BADSTATE;
	തിരികെ 0;
}

#define FDT_SW_PROBE_STRUCT(fdt) \
	{ \
		പൂർണ്ണസംഖ്യ err; \
		എങ്കിൽ ((err = fdt_sw_probe_struct_(fdt)) != 0) \
			തിരികെ err; \
	}

സ്ഥിതം വരിയിൽ uint32_t sw_flags(ശൂന്യം *fdt)
{
	/* assert: (fdt_magic(fdt) == FDT_SW_MAGIC) */
	തിരികെ fdt_last_comp_version(fdt);
}

/* 'complete' state:	Enter this state after fdt_finish()
 *
 * Allowed functions: none
 */

സ്ഥിതം ശൂന്യം *fdt_grab_space_(ശൂന്യം *fdt, size_t len)
{
	unsigned പൂർണ്ണസംഖ്യ offset = fdt_size_dt_struct(fdt);
	unsigned പൂർണ്ണസംഖ്യ spaceleft;

	spaceleft = fdt_totalsize(fdt) - fdt_off_dt_struct(fdt)
		- fdt_size_dt_strings(fdt);

	എങ്കിൽ ((offset + len < offset) || (offset + len > spaceleft))
		തിരികെ ശൂന്യ;

	fdt_set_size_dt_struct(fdt, offset + len);
	തിരികെ fdt_offset_ptr_w_(fdt, offset);
}

പൂർണ്ണസംഖ്യ fdt_create_with_flags(ശൂന്യം *buf, പൂർണ്ണസംഖ്യ bufsize, uint32_t flags)
{
	സ്ഥിരം പൂർണ്ണസംഖ്യ hdrsize = FDT_ALIGN(വലുപ്പം(ഘടന fdt_header),
				      വലുപ്പം(ഘടന fdt_reserve_entry));
	ശൂന്യം *fdt = buf;

	എങ്കിൽ (bufsize < hdrsize)
		തിരികെ -FDT_ERR_NOSPACE;

	എങ്കിൽ (flags & ~FDT_CREATE_FLAGS_ALL)
		തിരികെ -FDT_ERR_BADFLAGS;

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

	തിരികെ 0;
}

പൂർണ്ണസംഖ്യ fdt_create(ശൂന്യം *buf, പൂർണ്ണസംഖ്യ bufsize)
{
	തിരികെ fdt_create_with_flags(buf, bufsize, 0);
}

പൂർണ്ണസംഖ്യ fdt_resize(ശൂന്യം *fdt, ശൂന്യം *buf, പൂർണ്ണസംഖ്യ bufsize)
{
	size_t headsize, tailsize;
	അക്ഷരം *oldtail, *newtail;

	FDT_SW_PROBE(fdt);

	എങ്കിൽ (bufsize < 0)
		തിരികെ -FDT_ERR_NOSPACE;

	headsize = fdt_off_dt_struct(fdt) + fdt_size_dt_struct(fdt);
	tailsize = fdt_size_dt_strings(fdt);

	എങ്കിൽ (!can_assume(VALID_DTB) &&
	    headsize + tailsize > fdt_totalsize(fdt))
		തിരികെ -FDT_ERR_INTERNAL;

	എങ്കിൽ ((headsize + tailsize) > (unsigned)bufsize)
		തിരികെ -FDT_ERR_NOSPACE;

	oldtail = (അക്ഷരം *)fdt + fdt_totalsize(fdt) - tailsize;
	newtail = (അക്ഷരം *)buf + bufsize - tailsize;

	/* Two cases to avoid clobbering data if the old and new
	 * buffers partially overlap */
	എങ്കിൽ (buf <= fdt) {
		memmove(buf, fdt, headsize);
		memmove(newtail, oldtail, tailsize);
	} അല്ലെങ്കിൽ {
		memmove(newtail, oldtail, tailsize);
		memmove(buf, fdt, headsize);
	}

	fdt_set_totalsize(buf, bufsize);
	എങ്കിൽ (fdt_off_dt_strings(buf))
		fdt_set_off_dt_strings(buf, bufsize);

	തിരികെ 0;
}

പൂർണ്ണസംഖ്യ fdt_add_reservemap_entry(ശൂന്യം *fdt, uint64_t addr, uint64_t size)
{
	ഘടന fdt_reserve_entry *re;
	പൂർണ്ണസംഖ്യ offset;

	FDT_SW_PROBE_MEMRSV(fdt);

	offset = fdt_off_dt_struct(fdt);
	എങ്കിൽ ((offset + വലുപ്പം(*re)) > fdt_totalsize(fdt))
		തിരികെ -FDT_ERR_NOSPACE;

	re = (ഘടന fdt_reserve_entry *)((അക്ഷരം *)fdt + offset);
	re->address = cpu_to_fdt64(addr);
	re->size = cpu_to_fdt64(size);

	fdt_set_off_dt_struct(fdt, offset + വലുപ്പം(*re));

	തിരികെ 0;
}

പൂർണ്ണസംഖ്യ fdt_finish_reservemap(ശൂന്യം *fdt)
{
	പൂർണ്ണസംഖ്യ err = fdt_add_reservemap_entry(fdt, 0, 0);

	എങ്കിൽ (err)
		തിരികെ err;

	fdt_set_off_dt_strings(fdt, fdt_totalsize(fdt));
	തിരികെ 0;
}

പൂർണ്ണസംഖ്യ fdt_begin_കണ്ണി(ശൂന്യം *fdt, സ്ഥിരം അക്ഷരം *name)
{
	ഘടന fdt_കണ്ണി_header *nh;
	പൂർണ്ണസംഖ്യ namelen;

	FDT_SW_PROBE_STRUCT(fdt);

	namelen = strlen(name) + 1;
	nh = fdt_grab_space_(fdt, വലുപ്പം(*nh) + FDT_TAGALIGN(namelen));
	എങ്കിൽ (! nh)
		തിരികെ -FDT_ERR_NOSPACE;

	nh->tag = cpu_to_fdt32(FDT_BEGIN_കണ്ണി);
	memcpy(nh->name, name, namelen);
	തിരികെ 0;
}

പൂർണ്ണസംഖ്യ fdt_end_കണ്ണി(ശൂന്യം *fdt)
{
	fdt32_t *en;

	FDT_SW_PROBE_STRUCT(fdt);

	en = fdt_grab_space_(fdt, FDT_TAGSIZE);
	എങ്കിൽ (! en)
		തിരികെ -FDT_ERR_NOSPACE;

	*en = cpu_to_fdt32(FDT_END_കണ്ണി);
	തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ fdt_add_string_(ശൂന്യം *fdt, സ്ഥിരം അക്ഷരം *s)
{
	അക്ഷരം *strtab = (അക്ഷരം *)fdt + fdt_totalsize(fdt);
	unsigned പൂർണ്ണസംഖ്യ strtabsize = fdt_size_dt_strings(fdt);
	unsigned പൂർണ്ണസംഖ്യ len = strlen(s) + 1;
	unsigned പൂർണ്ണസംഖ്യ struct_മുകൾ, offset;

	offset = strtabsize + len;
	struct_മുകൾ = fdt_off_dt_struct(fdt) + fdt_size_dt_struct(fdt);
	എങ്കിൽ (fdt_totalsize(fdt) - offset < struct_മുകൾ)
		തിരികെ 0; /* no more room :( */

	memcpy(strtab - offset, s, len);
	fdt_set_size_dt_strings(fdt, strtabsize + len);
	തിരികെ -offset;
}

/* Must only be used to roll back in case of error */
സ്ഥിതം ശൂന്യം fdt_del_last_string_(ശൂന്യം *fdt, സ്ഥിരം അക്ഷരം *s)
{
	പൂർണ്ണസംഖ്യ strtabsize = fdt_size_dt_strings(fdt);
	പൂർണ്ണസംഖ്യ len = strlen(s) + 1;

	fdt_set_size_dt_strings(fdt, strtabsize - len);
}

സ്ഥിതം പൂർണ്ണസംഖ്യ fdt_find_add_string_(ശൂന്യം *fdt, സ്ഥിരം അക്ഷരം *s, പൂർണ്ണസംഖ്യ *allocated)
{
	അക്ഷരം *strtab = (അക്ഷരം *)fdt + fdt_totalsize(fdt);
	പൂർണ്ണസംഖ്യ strtabsize = fdt_size_dt_strings(fdt);
	സ്ഥിരം അക്ഷരം *p;

	*allocated = 0;

	p = fdt_find_string_(strtab - strtabsize, strtabsize, s);
	എങ്കിൽ (p)
		തിരികെ p - strtab;

	*allocated = 1;

	തിരികെ fdt_add_string_(fdt, s);
}

പൂർണ്ണസംഖ്യ fdt_property_placeholder(ശൂന്യം *fdt, സ്ഥിരം അക്ഷരം *name, പൂർണ്ണസംഖ്യ len, ശൂന്യം **valp)
{
	ഘടന fdt_property *prop;
	പൂർണ്ണസംഖ്യ nameoff;
	പൂർണ്ണസംഖ്യ allocated;

	FDT_SW_PROBE_STRUCT(fdt);

	/* String de-duplication can be slow, _NO_NAME_DEDUP skips it */
	എങ്കിൽ (sw_flags(fdt) & FDT_CREATE_FLAG_NO_NAME_DEDUP) {
		allocated = 1;
		nameoff = fdt_add_string_(fdt, name);
	} അല്ലെങ്കിൽ {
		nameoff = fdt_find_add_string_(fdt, name, &allocated);
	}
	എങ്കിൽ (nameoff == 0)
		തിരികെ -FDT_ERR_NOSPACE;

	prop = fdt_grab_space_(fdt, വലുപ്പം(*prop) + FDT_TAGALIGN(len));
	എങ്കിൽ (! prop) {
		എങ്കിൽ (allocated)
			fdt_del_last_string_(fdt, name);
		തിരികെ -FDT_ERR_NOSPACE;
	}

	prop->tag = cpu_to_fdt32(FDT_PROP);
	prop->nameoff = cpu_to_fdt32(nameoff);
	prop->len = cpu_to_fdt32(len);
	*valp = prop->data;
	തിരികെ 0;
}

പൂർണ്ണസംഖ്യ fdt_property(ശൂന്യം *fdt, സ്ഥിരം അക്ഷരം *name, സ്ഥിരം ശൂന്യം *val, പൂർണ്ണസംഖ്യ len)
{
	ശൂന്യം *ptr;
	പൂർണ്ണസംഖ്യ ret;

	ret = fdt_property_placeholder(fdt, name, len, &ptr);
	എങ്കിൽ (ret)
		തിരികെ ret;
	memcpy(ptr, val, len);
	തിരികെ 0;
}

പൂർണ്ണസംഖ്യ fdt_finish(ശൂന്യം *fdt)
{
	അക്ഷരം *p = (അക്ഷരം *)fdt;
	fdt32_t *end;
	പൂർണ്ണസംഖ്യ oldstroffset, newstroffset;
	uint32_t tag;
	പൂർണ്ണസംഖ്യ offset, nextoffset;

	FDT_SW_PROBE_STRUCT(fdt);

	/* Add terminator */
	end = fdt_grab_space_(fdt, വലുപ്പം(*end));
	എങ്കിൽ (! end)
		തിരികെ -FDT_ERR_NOSPACE;
	*end = cpu_to_fdt32(FDT_END);

	/* Relocate the string table */
	oldstroffset = fdt_totalsize(fdt) - fdt_size_dt_strings(fdt);
	newstroffset = fdt_off_dt_struct(fdt) + fdt_size_dt_struct(fdt);
	memmove(p + newstroffset, p + oldstroffset, fdt_size_dt_strings(fdt));
	fdt_set_off_dt_strings(fdt, newstroffset);

	/* Walk the structure, correcting string offsets */
	offset = 0;
	ആകുമ്പോൾ ((tag = fdt_next_tag(fdt, offset, &nextoffset)) != FDT_END) {
		എങ്കിൽ (tag == FDT_PROP) {
			ഘടന fdt_property *prop =
				fdt_offset_ptr_w_(fdt, offset);
			പൂർണ്ണസംഖ്യ nameoff;

			nameoff = fdt32_to_cpu(prop->nameoff);
			nameoff += fdt_size_dt_strings(fdt);
			prop->nameoff = cpu_to_fdt32(nameoff);
		}
		offset = nextoffset;
	}
	എങ്കിൽ (nextoffset < 0)
		തിരികെ nextoffset;

	/* Finally, adjust the header */
	fdt_set_totalsize(fdt, newstroffset + fdt_size_dt_strings(fdt));

	/* And fix up fields that were keeping intermediate state. */
	fdt_set_last_comp_version(fdt, FDT_LAST_COMPATIBLE_VERSION);
	fdt_set_magic(fdt, FDT_MAGIC);

	തിരികെ 0;
}
