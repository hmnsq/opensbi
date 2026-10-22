// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#अन्तर्भाव "libfdt_env.h"

#अन्तर्भाव <fdt.h>
#अन्तर्भाव <libfdt.h>

#अन्तर्भाव "libfdt_internal.h"

स्थिर पूर्णांक fdt_sw_probe_(शून्य *fdt)
{
	जर (!can_assume(VALID_आदान)) {
		जर (fdt_magic(fdt) == FDT_MAGIC)
			परत -FDT_ERR_BADSTATE;
		अन्यथा जर (fdt_magic(fdt) != FDT_SW_MAGIC)
			परत -FDT_ERR_BADMAGIC;
	}

	परत 0;
}

#define FDT_SW_PROBE(fdt) \
	{ \
		पूर्णांक err; \
		जर ((err = fdt_sw_probe_(fdt)) != 0) \
			परत err; \
	}

/* 'memrsv' state:	Initial state after fdt_create()
 *
 * Allowed functions:
 *	fdt_add_reservemap_entry()
 *	fdt_finish_reservemap()		[moves to 'struct' state]
 */
स्थिर पूर्णांक fdt_sw_probe_memrsv_(शून्य *fdt)
{
	पूर्णांक err = fdt_sw_probe_(fdt);
	जर (err)
		परत err;

	जर (!can_assume(VALID_आदान) && fdt_off_dt_strings(fdt) != 0)
		परत -FDT_ERR_BADSTATE;
	परत 0;
}

#define FDT_SW_PROBE_MEMRSV(fdt) \
	{ \
		पूर्णांक err; \
		जर ((err = fdt_sw_probe_memrsv_(fdt)) != 0) \
			परत err; \
	}

/* 'struct' state:	Enter this state after fdt_finish_reservemap()
 *
 * Allowed functions:
 *	fdt_begin_node()
 *	fdt_end_node()
 *	fdt_property*()
 *	fdt_finish()			[moves to 'complete' state]
 */
स्थिर पूर्णांक fdt_sw_probe_struct_(शून्य *fdt)
{
	पूर्णांक err = fdt_sw_probe_(fdt);
	जर (err)
		परत err;

	जर (!can_assume(VALID_आदान) &&
	    fdt_off_dt_strings(fdt) != fdt_totalsize(fdt))
		परत -FDT_ERR_BADSTATE;
	परत 0;
}

#define FDT_SW_PROBE_STRUCT(fdt) \
	{ \
		पूर्णांक err; \
		जर ((err = fdt_sw_probe_struct_(fdt)) != 0) \
			परत err; \
	}

स्थिर ओळीत uint32_t sw_flags(शून्य *fdt)
{
	/* assert: (fdt_magic(fdt) == FDT_SW_MAGIC) */
	परत fdt_last_comp_version(fdt);
}

/* 'complete' state:	Enter this state after fdt_finish()
 *
 * Allowed functions: none
 */

स्थिर शून्य *fdt_grab_space_(शून्य *fdt, माप_प्रकार len)
{
	अचिन्हित पूर्णांक offset = fdt_size_dt_struct(fdt);
	अचिन्हित पूर्णांक spaceleft;

	spaceleft = fdt_totalsize(fdt) - fdt_off_dt_struct(fdt)
		- fdt_size_dt_strings(fdt);

	जर ((offset + len < offset) || (offset + len > spaceleft))
		परत रिक्त;

	fdt_set_size_dt_struct(fdt, offset + len);
	परत fdt_offset_ptr_w_(fdt, offset);
}

पूर्णांक fdt_create_with_flags(शून्य *buf, पूर्णांक bufsize, uint32_t flags)
{
	स्थिरांक पूर्णांक hdrsize = FDT_ALIGN(आकार(संरचना fdt_header),
				      आकार(संरचना fdt_reserve_entry));
	शून्य *fdt = buf;

	जर (bufsize < hdrsize)
		परत -FDT_ERR_NOSPACE;

	जर (flags & ~FDT_CREATE_FLAGS_ALL)
		परत -FDT_ERR_BADFLAGS;

	स_रखो(buf, 0, bufsize);

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

	परत 0;
}

पूर्णांक fdt_create(शून्य *buf, पूर्णांक bufsize)
{
	परत fdt_create_with_flags(buf, bufsize, 0);
}

पूर्णांक fdt_resize(शून्य *fdt, शून्य *buf, पूर्णांक bufsize)
{
	माप_प्रकार headsize, tailsize;
	अक्षर *oldtail, *newtail;

	FDT_SW_PROBE(fdt);

	जर (bufsize < 0)
		परत -FDT_ERR_NOSPACE;

	headsize = fdt_off_dt_struct(fdt) + fdt_size_dt_struct(fdt);
	tailsize = fdt_size_dt_strings(fdt);

	जर (!can_assume(VALID_DTB) &&
	    headsize + tailsize > fdt_totalsize(fdt))
		परत -FDT_ERR_INTERNAL;

	जर ((headsize + tailsize) > (अचिन्हित)bufsize)
		परत -FDT_ERR_NOSPACE;

	oldtail = (अक्षर *)fdt + fdt_totalsize(fdt) - tailsize;
	newtail = (अक्षर *)buf + bufsize - tailsize;

	/* Two cases to avoid clobbering data if the old and new
	 * buffers partially overlap */
	जर (buf <= fdt) {
		स_हटाओ(buf, fdt, headsize);
		स_हटाओ(newtail, oldtail, tailsize);
	} अन्यथा {
		स_हटाओ(newtail, oldtail, tailsize);
		स_हटाओ(buf, fdt, headsize);
	}

	fdt_set_totalsize(buf, bufsize);
	जर (fdt_off_dt_strings(buf))
		fdt_set_off_dt_strings(buf, bufsize);

	परत 0;
}

पूर्णांक fdt_add_reservemap_entry(शून्य *fdt, uint64_t addr, uint64_t size)
{
	संरचना fdt_reserve_entry *re;
	पूर्णांक offset;

	FDT_SW_PROBE_MEMRSV(fdt);

	offset = fdt_off_dt_struct(fdt);
	जर ((offset + आकार(*re)) > fdt_totalsize(fdt))
		परत -FDT_ERR_NOSPACE;

	re = (संरचना fdt_reserve_entry *)((अक्षर *)fdt + offset);
	re->address = cpu_to_fdt64(addr);
	re->size = cpu_to_fdt64(size);

	fdt_set_off_dt_struct(fdt, offset + आकार(*re));

	परत 0;
}

पूर्णांक fdt_finish_reservemap(शून्य *fdt)
{
	पूर्णांक err = fdt_add_reservemap_entry(fdt, 0, 0);

	जर (err)
		परत err;

	fdt_set_off_dt_strings(fdt, fdt_totalsize(fdt));
	परत 0;
}

पूर्णांक fdt_begin_गाठ(शून्य *fdt, स्थिरांक अक्षर *name)
{
	संरचना fdt_गाठ_header *nh;
	पूर्णांक namelen;

	FDT_SW_PROBE_STRUCT(fdt);

	namelen = म_माप(name) + 1;
	nh = fdt_grab_space_(fdt, आकार(*nh) + FDT_TAGALIGN(namelen));
	जर (! nh)
		परत -FDT_ERR_NOSPACE;

	nh->tag = cpu_to_fdt32(FDT_BEGIN_गाठ);
	स_नकल(nh->name, name, namelen);
	परत 0;
}

पूर्णांक fdt_end_गाठ(शून्य *fdt)
{
	fdt32_t *en;

	FDT_SW_PROBE_STRUCT(fdt);

	en = fdt_grab_space_(fdt, FDT_TAGSIZE);
	जर (! en)
		परत -FDT_ERR_NOSPACE;

	*en = cpu_to_fdt32(FDT_END_गाठ);
	परत 0;
}

स्थिर पूर्णांक fdt_add_string_(शून्य *fdt, स्थिरांक अक्षर *s)
{
	अक्षर *strtab = (अक्षर *)fdt + fdt_totalsize(fdt);
	अचिन्हित पूर्णांक strtabsize = fdt_size_dt_strings(fdt);
	अचिन्हित पूर्णांक len = म_माप(s) + 1;
	अचिन्हित पूर्णांक struct_शीर्ष, offset;

	offset = strtabsize + len;
	struct_शीर्ष = fdt_off_dt_struct(fdt) + fdt_size_dt_struct(fdt);
	जर (fdt_totalsize(fdt) - offset < struct_शीर्ष)
		परत 0; /* no more room :( */

	स_नकल(strtab - offset, s, len);
	fdt_set_size_dt_strings(fdt, strtabsize + len);
	परत -offset;
}

/* Must only be used to roll back in case of error */
स्थिर शून्य fdt_del_last_string_(शून्य *fdt, स्थिरांक अक्षर *s)
{
	पूर्णांक strtabsize = fdt_size_dt_strings(fdt);
	पूर्णांक len = म_माप(s) + 1;

	fdt_set_size_dt_strings(fdt, strtabsize - len);
}

स्थिर पूर्णांक fdt_find_add_string_(शून्य *fdt, स्थिरांक अक्षर *s, पूर्णांक *allocated)
{
	अक्षर *strtab = (अक्षर *)fdt + fdt_totalsize(fdt);
	पूर्णांक strtabsize = fdt_size_dt_strings(fdt);
	स्थिरांक अक्षर *p;

	*allocated = 0;

	p = fdt_find_string_(strtab - strtabsize, strtabsize, s);
	जर (p)
		परत p - strtab;

	*allocated = 1;

	परत fdt_add_string_(fdt, s);
}

पूर्णांक fdt_property_placeholder(शून्य *fdt, स्थिरांक अक्षर *name, पूर्णांक len, शून्य **valp)
{
	संरचना fdt_property *prop;
	पूर्णांक nameoff;
	पूर्णांक allocated;

	FDT_SW_PROBE_STRUCT(fdt);

	/* String de-duplication can be slow, _NO_NAME_DEDUP skips it */
	जर (sw_flags(fdt) & FDT_CREATE_FLAG_NO_NAME_DEDUP) {
		allocated = 1;
		nameoff = fdt_add_string_(fdt, name);
	} अन्यथा {
		nameoff = fdt_find_add_string_(fdt, name, &allocated);
	}
	जर (nameoff == 0)
		परत -FDT_ERR_NOSPACE;

	prop = fdt_grab_space_(fdt, आकार(*prop) + FDT_TAGALIGN(len));
	जर (! prop) {
		जर (allocated)
			fdt_del_last_string_(fdt, name);
		परत -FDT_ERR_NOSPACE;
	}

	prop->tag = cpu_to_fdt32(FDT_PROP);
	prop->nameoff = cpu_to_fdt32(nameoff);
	prop->len = cpu_to_fdt32(len);
	*valp = prop->data;
	परत 0;
}

पूर्णांक fdt_property(शून्य *fdt, स्थिरांक अक्षर *name, स्थिरांक शून्य *val, पूर्णांक len)
{
	शून्य *ptr;
	पूर्णांक ret;

	ret = fdt_property_placeholder(fdt, name, len, &ptr);
	जर (ret)
		परत ret;
	स_नकल(ptr, val, len);
	परत 0;
}

पूर्णांक fdt_finish(शून्य *fdt)
{
	अक्षर *p = (अक्षर *)fdt;
	fdt32_t *end;
	पूर्णांक oldstroffset, newstroffset;
	uint32_t tag;
	पूर्णांक offset, nextoffset;

	FDT_SW_PROBE_STRUCT(fdt);

	/* Add terminator */
	end = fdt_grab_space_(fdt, आकार(*end));
	जर (! end)
		परत -FDT_ERR_NOSPACE;
	*end = cpu_to_fdt32(FDT_END);

	/* Relocate the string table */
	oldstroffset = fdt_totalsize(fdt) - fdt_size_dt_strings(fdt);
	newstroffset = fdt_off_dt_struct(fdt) + fdt_size_dt_struct(fdt);
	स_हटाओ(p + newstroffset, p + oldstroffset, fdt_size_dt_strings(fdt));
	fdt_set_off_dt_strings(fdt, newstroffset);

	/* Walk the structure, correcting string offsets */
	offset = 0;
	पर्यंत ((tag = fdt_next_tag(fdt, offset, &nextoffset)) != FDT_END) {
		जर (tag == FDT_PROP) {
			संरचना fdt_property *prop =
				fdt_offset_ptr_w_(fdt, offset);
			पूर्णांक nameoff;

			nameoff = fdt32_to_cpu(prop->nameoff);
			nameoff += fdt_size_dt_strings(fdt);
			prop->nameoff = cpu_to_fdt32(nameoff);
		}
		offset = nextoffset;
	}
	जर (nextoffset < 0)
		परत nextoffset;

	/* Finally, adjust the header */
	fdt_set_totalsize(fdt, newstroffset + fdt_size_dt_strings(fdt));

	/* And fix up fields that were keeping intermediate state. */
	fdt_set_last_comp_version(fdt, FDT_LAST_COMPATIBLE_VERSION);
	fdt_set_magic(fdt, FDT_MAGIC);

	परत 0;
}
