// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#अन्तर्भाव "libfdt_env.h"

#अन्तर्भाव <fdt.h>
#अन्तर्भाव <libfdt.h>

#अन्तर्भाव "libfdt_internal.h"

स्थिर पूर्णाङ्क fdt_sw_probe_(शून्य *fdt)
{
	सचे (!can_assume(VALID_निवेशः)) {
		सचे (fdt_magic(fdt) == FDT_MAGIC)
			पटिनिवत्त -FDT_ERR_BADSTATE;
		अञ्ञथा सचे (fdt_magic(fdt) != FDT_SW_MAGIC)
			पटिनिवत्त -FDT_ERR_BADMAGIC;
	}

	पटिनिवत्त 0;
}

#define FDT_SW_PROBE(fdt) \
	{ \
		पूर्णाङ्क err; \
		सचे ((err = fdt_sw_probe_(fdt)) != 0) \
			पटिनिवत्त err; \
	}

/* 'memrsv' state:	Initial state after fdt_create()
 *
 * Allowed functions:
 *	fdt_add_reservemap_entry()
 *	fdt_finish_reservemap()		[moves to 'struct' state]
 */
स्थिर पूर्णाङ्क fdt_sw_probe_memrsv_(शून्य *fdt)
{
	पूर्णाङ्क err = fdt_sw_probe_(fdt);
	सचे (err)
		पटिनिवत्त err;

	सचे (!can_assume(VALID_निवेशः) && fdt_off_dt_strings(fdt) != 0)
		पटिनिवत्त -FDT_ERR_BADSTATE;
	पटिनिवत्त 0;
}

#define FDT_SW_PROBE_MEMRSV(fdt) \
	{ \
		पूर्णाङ्क err; \
		सचे ((err = fdt_sw_probe_memrsv_(fdt)) != 0) \
			पटिनिवत्त err; \
	}

/* 'struct' state:	Enter this state after fdt_finish_reservemap()
 *
 * Allowed functions:
 *	fdt_begin_node()
 *	fdt_end_node()
 *	fdt_property*()
 *	fdt_finish()			[moves to 'complete' state]
 */
स्थिर पूर्णाङ्क fdt_sw_probe_struct_(शून्य *fdt)
{
	पूर्णाङ्क err = fdt_sw_probe_(fdt);
	सचे (err)
		पटिनिवत्त err;

	सचे (!can_assume(VALID_निवेशः) &&
	    fdt_off_dt_strings(fdt) != fdt_totalsize(fdt))
		पटिनिवत्त -FDT_ERR_BADSTATE;
	पटिनिवत्त 0;
}

#define FDT_SW_PROBE_STRUCT(fdt) \
	{ \
		पूर्णाङ्क err; \
		सचे ((err = fdt_sw_probe_struct_(fdt)) != 0) \
			पटिनिवत्त err; \
	}

स्थिर अन्तःस्थ uint32_t sw_flags(शून्य *fdt)
{
	/* assert: (fdt_magic(fdt) == FDT_SW_MAGIC) */
	पटिनिवत्त fdt_last_comp_version(fdt);
}

/* 'complete' state:	Enter this state after fdt_finish()
 *
 * Allowed functions: none
 */

स्थिर शून्य *fdt_grab_space_(शून्य *fdt, size_t len)
{
	unsigned पूर्णाङ्क offset = fdt_size_dt_struct(fdt);
	unsigned पूर्णाङ्क spaceleft;

	spaceleft = fdt_totalsize(fdt) - fdt_off_dt_struct(fdt)
		- fdt_size_dt_strings(fdt);

	सचे ((offset + len < offset) || (offset + len > spaceleft))
		पटिनिवत्त रिक्त;

	fdt_set_size_dt_struct(fdt, offset + len);
	पटिनिवत्त fdt_offset_ptr_w_(fdt, offset);
}

पूर्णाङ्क fdt_create_with_flags(शून्य *buf, पूर्णाङ्क bufsize, uint32_t flags)
{
	ध्रुव पूर्णाङ्क hdrsize = FDT_ALIGN(परिमाण(संरचना fdt_header),
				      परिमाण(संरचना fdt_reserve_entry));
	शून्य *fdt = buf;

	सचे (bufsize < hdrsize)
		पटिनिवत्त -FDT_ERR_NOSPACE;

	सचे (flags & ~FDT_CREATE_FLAGS_ALL)
		पटिनिवत्त -FDT_ERR_BADFLAGS;

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

	पटिनिवत्त 0;
}

पूर्णाङ्क fdt_create(शून्य *buf, पूर्णाङ्क bufsize)
{
	पटिनिवत्त fdt_create_with_flags(buf, bufsize, 0);
}

पूर्णाङ्क fdt_resize(शून्य *fdt, शून्य *buf, पूर्णाङ्क bufsize)
{
	size_t headsize, tailsize;
	वर्ण *oldtail, *newtail;

	FDT_SW_PROBE(fdt);

	सचे (bufsize < 0)
		पटिनिवत्त -FDT_ERR_NOSPACE;

	headsize = fdt_off_dt_struct(fdt) + fdt_size_dt_struct(fdt);
	tailsize = fdt_size_dt_strings(fdt);

	सचे (!can_assume(VALID_DTB) &&
	    headsize + tailsize > fdt_totalsize(fdt))
		पटिनिवत्त -FDT_ERR_INTERNAL;

	सचे ((headsize + tailsize) > (unsigned)bufsize)
		पटिनिवत्त -FDT_ERR_NOSPACE;

	oldtail = (वर्ण *)fdt + fdt_totalsize(fdt) - tailsize;
	newtail = (वर्ण *)buf + bufsize - tailsize;

	/* Two cases to avoid clobbering data if the old and new
	 * buffers partially overlap */
	सचे (buf <= fdt) {
		memmove(buf, fdt, headsize);
		memmove(newtail, oldtail, tailsize);
	} अञ्ञथा {
		memmove(newtail, oldtail, tailsize);
		memmove(buf, fdt, headsize);
	}

	fdt_set_totalsize(buf, bufsize);
	सचे (fdt_off_dt_strings(buf))
		fdt_set_off_dt_strings(buf, bufsize);

	पटिनिवत्त 0;
}

पूर्णाङ्क fdt_add_reservemap_entry(शून्य *fdt, uint64_t addr, uint64_t size)
{
	संरचना fdt_reserve_entry *re;
	पूर्णाङ्क offset;

	FDT_SW_PROBE_MEMRSV(fdt);

	offset = fdt_off_dt_struct(fdt);
	सचे ((offset + परिमाण(*re)) > fdt_totalsize(fdt))
		पटिनिवत्त -FDT_ERR_NOSPACE;

	re = (संरचना fdt_reserve_entry *)((वर्ण *)fdt + offset);
	re->address = cpu_to_fdt64(addr);
	re->size = cpu_to_fdt64(size);

	fdt_set_off_dt_struct(fdt, offset + परिमाण(*re));

	पटिनिवत्त 0;
}

पूर्णाङ्क fdt_finish_reservemap(शून्य *fdt)
{
	पूर्णाङ्क err = fdt_add_reservemap_entry(fdt, 0, 0);

	सचे (err)
		पटिनिवत्त err;

	fdt_set_off_dt_strings(fdt, fdt_totalsize(fdt));
	पटिनिवत्त 0;
}

पूर्णाङ्क fdt_begin_ग्रन्थिः(शून्य *fdt, ध्रुव वर्ण *name)
{
	संरचना fdt_ग्रन्थिः_header *nh;
	पूर्णाङ्क namelen;

	FDT_SW_PROBE_STRUCT(fdt);

	namelen = strlen(name) + 1;
	nh = fdt_grab_space_(fdt, परिमाण(*nh) + FDT_TAGALIGN(namelen));
	सचे (! nh)
		पटिनिवत्त -FDT_ERR_NOSPACE;

	nh->tag = cpu_to_fdt32(FDT_BEGIN_ग्रन्थिः);
	memcpy(nh->name, name, namelen);
	पटिनिवत्त 0;
}

पूर्णाङ्क fdt_end_ग्रन्थिः(शून्य *fdt)
{
	fdt32_t *en;

	FDT_SW_PROBE_STRUCT(fdt);

	en = fdt_grab_space_(fdt, FDT_TAGSIZE);
	सचे (! en)
		पटिनिवत्त -FDT_ERR_NOSPACE;

	*en = cpu_to_fdt32(FDT_END_ग्रन्थिः);
	पटिनिवत्त 0;
}

स्थिर पूर्णाङ्क fdt_add_string_(शून्य *fdt, ध्रुव वर्ण *s)
{
	वर्ण *strtab = (वर्ण *)fdt + fdt_totalsize(fdt);
	unsigned पूर्णाङ्क strtabsize = fdt_size_dt_strings(fdt);
	unsigned पूर्णाङ्क len = strlen(s) + 1;
	unsigned पूर्णाङ्क struct_शिखरम्, offset;

	offset = strtabsize + len;
	struct_शिखरम् = fdt_off_dt_struct(fdt) + fdt_size_dt_struct(fdt);
	सचे (fdt_totalsize(fdt) - offset < struct_शिखरम्)
		पटिनिवत्त 0; /* no more room :( */

	memcpy(strtab - offset, s, len);
	fdt_set_size_dt_strings(fdt, strtabsize + len);
	पटिनिवत्त -offset;
}

/* Must only be used to roll back in case of error */
स्थिर शून्य fdt_del_last_string_(शून्य *fdt, ध्रुव वर्ण *s)
{
	पूर्णाङ्क strtabsize = fdt_size_dt_strings(fdt);
	पूर्णाङ्क len = strlen(s) + 1;

	fdt_set_size_dt_strings(fdt, strtabsize - len);
}

स्थिर पूर्णाङ्क fdt_find_add_string_(शून्य *fdt, ध्रुव वर्ण *s, पूर्णाङ्क *allocated)
{
	वर्ण *strtab = (वर्ण *)fdt + fdt_totalsize(fdt);
	पूर्णाङ्क strtabsize = fdt_size_dt_strings(fdt);
	ध्रुव वर्ण *p;

	*allocated = 0;

	p = fdt_find_string_(strtab - strtabsize, strtabsize, s);
	सचे (p)
		पटिनिवत्त p - strtab;

	*allocated = 1;

	पटिनिवत्त fdt_add_string_(fdt, s);
}

पूर्णाङ्क fdt_property_placeholder(शून्य *fdt, ध्रुव वर्ण *name, पूर्णाङ्क len, शून्य **valp)
{
	संरचना fdt_property *prop;
	पूर्णाङ्क nameoff;
	पूर्णाङ्क allocated;

	FDT_SW_PROBE_STRUCT(fdt);

	/* String de-duplication can be slow, _NO_NAME_DEDUP skips it */
	सचे (sw_flags(fdt) & FDT_CREATE_FLAG_NO_NAME_DEDUP) {
		allocated = 1;
		nameoff = fdt_add_string_(fdt, name);
	} अञ्ञथा {
		nameoff = fdt_find_add_string_(fdt, name, &allocated);
	}
	सचे (nameoff == 0)
		पटिनिवत्त -FDT_ERR_NOSPACE;

	prop = fdt_grab_space_(fdt, परिमाण(*prop) + FDT_TAGALIGN(len));
	सचे (! prop) {
		सचे (allocated)
			fdt_del_last_string_(fdt, name);
		पटिनिवत्त -FDT_ERR_NOSPACE;
	}

	prop->tag = cpu_to_fdt32(FDT_PROP);
	prop->nameoff = cpu_to_fdt32(nameoff);
	prop->len = cpu_to_fdt32(len);
	*valp = prop->data;
	पटिनिवत्त 0;
}

पूर्णाङ्क fdt_property(शून्य *fdt, ध्रुव वर्ण *name, ध्रुव शून्य *val, पूर्णाङ्क len)
{
	शून्य *ptr;
	पूर्णाङ्क ret;

	ret = fdt_property_placeholder(fdt, name, len, &ptr);
	सचे (ret)
		पटिनिवत्त ret;
	memcpy(ptr, val, len);
	पटिनिवत्त 0;
}

पूर्णाङ्क fdt_finish(शून्य *fdt)
{
	वर्ण *p = (वर्ण *)fdt;
	fdt32_t *end;
	पूर्णाङ्क oldstroffset, newstroffset;
	uint32_t tag;
	पूर्णाङ्क offset, nextoffset;

	FDT_SW_PROBE_STRUCT(fdt);

	/* Add terminator */
	end = fdt_grab_space_(fdt, परिमाण(*end));
	सचे (! end)
		पटिनिवत्त -FDT_ERR_NOSPACE;
	*end = cpu_to_fdt32(FDT_END);

	/* Relocate the string table */
	oldstroffset = fdt_totalsize(fdt) - fdt_size_dt_strings(fdt);
	newstroffset = fdt_off_dt_struct(fdt) + fdt_size_dt_struct(fdt);
	memmove(p + newstroffset, p + oldstroffset, fdt_size_dt_strings(fdt));
	fdt_set_off_dt_strings(fdt, newstroffset);

	/* Walk the structure, correcting string offsets */
	offset = 0;
	याव ((tag = fdt_next_tag(fdt, offset, &nextoffset)) != FDT_END) {
		सचे (tag == FDT_PROP) {
			संरचना fdt_property *prop =
				fdt_offset_ptr_w_(fdt, offset);
			पूर्णाङ्क nameoff;

			nameoff = fdt32_to_cpu(prop->nameoff);
			nameoff += fdt_size_dt_strings(fdt);
			prop->nameoff = cpu_to_fdt32(nameoff);
		}
		offset = nextoffset;
	}
	सचे (nextoffset < 0)
		पटिनिवत्त nextoffset;

	/* Finally, adjust the header */
	fdt_set_totalsize(fdt, newstroffset + fdt_size_dt_strings(fdt));

	/* And fix up fields that were keeping intermediate state. */
	fdt_set_last_comp_version(fdt, FDT_LAST_COMPATIBLE_VERSION);
	fdt_set_magic(fdt, FDT_MAGIC);

	पटिनिवत्त 0;
}
