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
	यदि (!can_assume(VALID_निवेशः)) {
		यदि (fdt_magic(fdt) == FDT_MAGIC)
			प्रत्यागम -FDT_ERR_BADSTATE;
		अन्यथा यदि (fdt_magic(fdt) != FDT_SW_MAGIC)
			प्रत्यागम -FDT_ERR_BADMAGIC;
	}

	प्रत्यागम 0;
}

#परिभाषय FDT_SW_PROBE(fdt) \
	{ \
		पूर्णाङ्क err; \
		यदि ((err = fdt_sw_probe_(fdt)) != 0) \
			प्रत्यागम err; \
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
	यदि (err)
		प्रत्यागम err;

	यदि (!can_assume(VALID_निवेशः) && fdt_off_dt_वाक्यानि(fdt) != 0)
		प्रत्यागम -FDT_ERR_BADSTATE;
	प्रत्यागम 0;
}

#परिभाषय FDT_SW_PROBE_MEMRSV(fdt) \
	{ \
		पूर्णाङ्क err; \
		यदि ((err = fdt_sw_probe_memrsv_(fdt)) != 0) \
			प्रत्यागम err; \
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
	यदि (err)
		प्रत्यागम err;

	यदि (!can_assume(VALID_निवेशः) &&
	    fdt_off_dt_वाक्यानि(fdt) != fdt_totalsize(fdt))
		प्रत्यागम -FDT_ERR_BADSTATE;
	प्रत्यागम 0;
}

#परिभाषय FDT_SW_PROBE_STRUCT(fdt) \
	{ \
		पूर्णाङ्क err; \
		यदि ((err = fdt_sw_probe_struct_(fdt)) != 0) \
			प्रत्यागम err; \
	}

स्थिर अन्तःस्थ uint32_t sw_flags(शून्य *fdt)
{
	/* assert: (fdt_magic(fdt) == FDT_SW_MAGIC) */
	प्रत्यागम fdt_चरमः_comp_संस्करणम्(fdt);
}

/* 'complete' state:	Enter this state after fdt_finish()
 *
 * Allowed functions: none
 */

स्थिर शून्य *fdt_grab_space_(शून्य *fdt, size_t len)
{
	अचिह्नित पूर्णाङ्क विस्थापनम् = fdt_आकार_dt_struct(fdt);
	अचिह्नित पूर्णाङ्क spaceleft;

	spaceleft = fdt_totalsize(fdt) - fdt_off_dt_struct(fdt)
		- fdt_आकार_dt_वाक्यानि(fdt);

	यदि ((विस्थापनम् + len < विस्थापनम्) || (विस्थापनम् + len > spaceleft))
		प्रत्यागम रिक्त;

	fdt_समुच्चयः_आकार_dt_struct(fdt, विस्थापनम् + len);
	प्रत्यागम fdt_विस्थापनम्_ptr_w(fdt, विस्थापनम्);
}

पूर्णाङ्क fdt_create_with_flags(शून्य *buf, पूर्णाङ्क bufsize, uint32_t flags)
{
	ध्रुव पूर्णाङ्क hdrsize = FDT_ALIGN(परिमाण(संरचना fdt_शीर्षकम्),
				      परिमाण(संरचना fdt_reserve_प्रविष्टिः));
	शून्य *fdt = buf;

	यदि (bufsize < hdrsize)
		प्रत्यागम -FDT_ERR_NOSPACE;

	यदि (flags & ~FDT_CREATE_FLAGS_ALL)
		प्रत्यागम -FDT_ERR_BADFLAGS;

	स्मृतिं_पूरय(buf, 0, bufsize);

	/*
	 * magic and last_comp_version keep intermediate state during the fdt
	 * creation process, which is replaced with the proper FDT format by
	 * fdt_finish().
	 *
	 * flags should be accessed with sw_flags().
	 */
	fdt_समुच्चयः_magic(fdt, FDT_SW_MAGIC);
	fdt_समुच्चयः_संस्करणम्(fdt, FDT_चरमः_SUPPORTED_संस्करणम्);
	fdt_समुच्चयः_चरमः_comp_संस्करणम्(fdt, flags);

	fdt_समुच्चयः_totalsize(fdt,  bufsize);

	fdt_समुच्चयः_off_mem_rsvmap(fdt, hdrsize);
	fdt_समुच्चयः_off_dt_struct(fdt, fdt_off_mem_rsvmap(fdt));
	fdt_समुच्चयः_off_dt_वाक्यानि(fdt, 0);

	प्रत्यागम 0;
}

पूर्णाङ्क fdt_create(शून्य *buf, पूर्णाङ्क bufsize)
{
	प्रत्यागम fdt_create_with_flags(buf, bufsize, 0);
}

पूर्णाङ्क fdt_resize(शून्य *fdt, शून्य *buf, पूर्णाङ्क bufsize)
{
	size_t headsize, tailsize;
	वर्ण *oldtail, *newtail;

	FDT_SW_PROBE(fdt);

	यदि (bufsize < 0)
		प्रत्यागम -FDT_ERR_NOSPACE;

	headsize = fdt_off_dt_struct(fdt) + fdt_आकार_dt_struct(fdt);
	tailsize = fdt_आकार_dt_वाक्यानि(fdt);

	यदि (!can_assume(VALID_DTB) &&
	    headsize + tailsize > fdt_totalsize(fdt))
		प्रत्यागम -FDT_ERR_आन्तरिकम्;

	यदि ((headsize + tailsize) > (अचिह्नित)bufsize)
		प्रत्यागम -FDT_ERR_NOSPACE;

	oldtail = (वर्ण *)fdt + fdt_totalsize(fdt) - tailsize;
	newtail = (वर्ण *)buf + bufsize - tailsize;

	/* Two cases to avoid clobbering data if the old and new
	 * buffers partially overlap */
	यदि (buf <= fdt) {
		memmove(buf, fdt, headsize);
		memmove(newtail, oldtail, tailsize);
	} अन्यथा {
		memmove(newtail, oldtail, tailsize);
		memmove(buf, fdt, headsize);
	}

	fdt_समुच्चयः_totalsize(buf, bufsize);
	यदि (fdt_off_dt_वाक्यानि(buf))
		fdt_समुच्चयः_off_dt_वाक्यानि(buf, bufsize);

	प्रत्यागम 0;
}

पूर्णाङ्क fdt_सङ्कलय_reservemap_प्रविष्टिः(शून्य *fdt, uint64_t addr, uint64_t आकार)
{
	संरचना fdt_reserve_प्रविष्टिः *re;
	पूर्णाङ्क विस्थापनम्;

	FDT_SW_PROBE_MEMRSV(fdt);

	विस्थापनम् = fdt_off_dt_struct(fdt);
	यदि ((विस्थापनम् + परिमाण(*re)) > fdt_totalsize(fdt))
		प्रत्यागम -FDT_ERR_NOSPACE;

	re = (संरचना fdt_reserve_प्रविष्टिः *)((वर्ण *)fdt + विस्थापनम्);
	re->स्थानाङ्कः = cpu_गन्तव्यम्_fdt64(addr);
	re->आकार = cpu_गन्तव्यम्_fdt64(आकार);

	fdt_समुच्चयः_off_dt_struct(fdt, विस्थापनम् + परिमाण(*re));

	प्रत्यागम 0;
}

पूर्णाङ्क fdt_finish_reservemap(शून्य *fdt)
{
	पूर्णाङ्क err = fdt_सङ्कलय_reservemap_प्रविष्टिः(fdt, 0, 0);

	यदि (err)
		प्रत्यागम err;

	fdt_समुच्चयः_off_dt_वाक्यानि(fdt, fdt_totalsize(fdt));
	प्रत्यागम 0;
}

पूर्णाङ्क fdt_begin_ग्रन्थिः(शून्य *fdt, ध्रुव वर्ण *name)
{
	संरचना fdt_ग्रन्थिः_शीर्षकम् *nh;
	पूर्णाङ्क namelen;

	FDT_SW_PROBE_STRUCT(fdt);

	namelen = वाक्यदैर्घ्यम्(name) + 1;
	nh = fdt_grab_space_(fdt, परिमाण(*nh) + FDT_TAGALIGN(namelen));
	यदि (! nh)
		प्रत्यागम -FDT_ERR_NOSPACE;

	nh->चिह्नकम् = cpu_गन्तव्यम्_fdt32(FDT_BEGIN_ग्रन्थिः);
	स्मृतिं_प्रतिलिख(nh->name, name, namelen);
	प्रत्यागम 0;
}

पूर्णाङ्क fdt_अन्तः_ग्रन्थिः(शून्य *fdt)
{
	fdt32_t *en;

	FDT_SW_PROBE_STRUCT(fdt);

	en = fdt_grab_space_(fdt, FDT_TAGSIZE);
	यदि (! en)
		प्रत्यागम -FDT_ERR_NOSPACE;

	*en = cpu_गन्तव्यम्_fdt32(FDT_अन्तः_ग्रन्थिः);
	प्रत्यागम 0;
}

स्थिर पूर्णाङ्क fdt_सङ्कलय_string(शून्य *fdt, ध्रुव वर्ण *s)
{
	वर्ण *strtab = (वर्ण *)fdt + fdt_totalsize(fdt);
	अचिह्नित पूर्णाङ्क strtabsize = fdt_आकार_dt_वाक्यानि(fdt);
	अचिह्नित पूर्णाङ्क len = वाक्यदैर्घ्यम्(s) + 1;
	अचिह्नित पूर्णाङ्क struct_शिखरम्, विस्थापनम्;

	विस्थापनम् = strtabsize + len;
	struct_शिखरम् = fdt_off_dt_struct(fdt) + fdt_आकार_dt_struct(fdt);
	यदि (fdt_totalsize(fdt) - विस्थापनम् < struct_शिखरम्)
		प्रत्यागम 0; /* no more room :( */

	स्मृतिं_प्रतिलिख(strtab - विस्थापनम्, s, len);
	fdt_समुच्चयः_आकार_dt_वाक्यानि(fdt, strtabsize + len);
	प्रत्यागम -विस्थापनम्;
}

/* Must only be used to roll back in case of error */
स्थिर शून्य fdt_del_चरमः_string(शून्य *fdt, ध्रुव वर्ण *s)
{
	पूर्णाङ्क strtabsize = fdt_आकार_dt_वाक्यानि(fdt);
	पूर्णाङ्क len = वाक्यदैर्घ्यम्(s) + 1;

	fdt_समुच्चयः_आकार_dt_वाक्यानि(fdt, strtabsize - len);
}

स्थिर पूर्णाङ्क fdt_find_सङ्कलय_string(शून्य *fdt, ध्रुव वर्ण *s, पूर्णाङ्क *allocated)
{
	वर्ण *strtab = (वर्ण *)fdt + fdt_totalsize(fdt);
	पूर्णाङ्क strtabsize = fdt_आकार_dt_वाक्यानि(fdt);
	ध्रुव वर्ण *p;

	*allocated = 0;

	p = fdt_find_string_(strtab - strtabsize, strtabsize, s);
	यदि (p)
		प्रत्यागम p - strtab;

	*allocated = 1;

	प्रत्यागम fdt_सङ्कलय_string(fdt, s);
}

पूर्णाङ्क fdt_property_placeholder(शून्य *fdt, ध्रुव वर्ण *name, पूर्णाङ्क len, शून्य **valp)
{
	संरचना fdt_property *prop;
	पूर्णाङ्क nameoff;
	पूर्णाङ्क allocated;

	FDT_SW_PROBE_STRUCT(fdt);

	/* String de-duplication can be slow, _NO_NAME_DEDUP skips it */
	यदि (sw_flags(fdt) & FDT_CREATE_FLAG_NO_NAME_DEDUP) {
		allocated = 1;
		nameoff = fdt_सङ्कलय_string(fdt, name);
	} अन्यथा {
		nameoff = fdt_find_सङ्कलय_string(fdt, name, &allocated);
	}
	यदि (nameoff == 0)
		प्रत्यागम -FDT_ERR_NOSPACE;

	prop = fdt_grab_space_(fdt, परिमाण(*prop) + FDT_TAGALIGN(len));
	यदि (! prop) {
		यदि (allocated)
			fdt_del_चरमः_string(fdt, name);
		प्रत्यागम -FDT_ERR_NOSPACE;
	}

	prop->चिह्नकम् = cpu_गन्तव्यम्_fdt32(FDT_PROP);
	prop->nameoff = cpu_गन्तव्यम्_fdt32(nameoff);
	prop->len = cpu_गन्तव्यम्_fdt32(len);
	*valp = prop->दत्त;
	प्रत्यागम 0;
}

पूर्णाङ्क fdt_property(शून्य *fdt, ध्रुव वर्ण *name, ध्रुव शून्य *val, पूर्णाङ्क len)
{
	शून्य *ptr;
	पूर्णाङ्क ret;

	ret = fdt_property_placeholder(fdt, name, len, &ptr);
	यदि (ret)
		प्रत्यागम ret;
	स्मृतिं_प्रतिलिख(ptr, val, len);
	प्रत्यागम 0;
}

पूर्णाङ्क fdt_finish(शून्य *fdt)
{
	वर्ण *p = (वर्ण *)fdt;
	fdt32_t *अन्तः_2;
	पूर्णाङ्क oldstroffset, newstroffset;
	uint32_t चिह्नकम्;
	पूर्णाङ्क विस्थापनम्, nextoffset;

	FDT_SW_PROBE_STRUCT(fdt);

	/* Add terminator */
	अन्तः_2 = fdt_grab_space_(fdt, परिमाण(*अन्तः_2));
	यदि (! अन्तः_2)
		प्रत्यागम -FDT_ERR_NOSPACE;
	*अन्तः_2 = cpu_गन्तव्यम्_fdt32(FDT_अन्तः);

	/* Relocate the string table */
	oldstroffset = fdt_totalsize(fdt) - fdt_आकार_dt_वाक्यानि(fdt);
	newstroffset = fdt_off_dt_struct(fdt) + fdt_आकार_dt_struct(fdt);
	memmove(p + newstroffset, p + oldstroffset, fdt_आकार_dt_वाक्यानि(fdt));
	fdt_समुच्चयः_off_dt_वाक्यानि(fdt, newstroffset);

	/* Walk the structure, correcting string offsets */
	विस्थापनम् = 0;
	यावत् ((चिह्नकम् = fdt_अग्र_चिह्नकम्(fdt, विस्थापनम्, &nextoffset)) != FDT_अन्तः) {
		यदि (चिह्नकम् == FDT_PROP) {
			संरचना fdt_property *prop =
				fdt_विस्थापनम्_ptr_w(fdt, विस्थापनम्);
			पूर्णाङ्क nameoff;

			nameoff = fdt32_गन्तव्यम्_cpu(prop->nameoff);
			nameoff += fdt_आकार_dt_वाक्यानि(fdt);
			prop->nameoff = cpu_गन्तव्यम्_fdt32(nameoff);
		}
		विस्थापनम् = nextoffset;
	}
	यदि (nextoffset < 0)
		प्रत्यागम nextoffset;

	/* Finally, adjust the header */
	fdt_समुच्चयः_totalsize(fdt, newstroffset + fdt_आकार_dt_वाक्यानि(fdt));

	/* And fix up fields that were keeping intermediate state. */
	fdt_समुच्चयः_चरमः_comp_संस्करणम्(fdt, FDT_चरमः_COMPATIBLE_संस्करणम्);
	fdt_समुच्चयः_magic(fdt, FDT_MAGIC);

	प्रत्यागम 0;
}
