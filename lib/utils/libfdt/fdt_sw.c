// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#ଅନ୍ତର୍ଭାଵ "libfdt_env.h"

#ଅନ୍ତର୍ଭାଵ <fdt.h>
#ଅନ୍ତର୍ଭାଵ <libfdt.h>

#ଅନ୍ତର୍ଭାଵ "libfdt_internal.h"

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_sw_probe_(ଶୂନ୍ୟ *fdt)
{
	ଯଦି (!can_assume(VALID_ନିବେଶ)) {
		ଯଦି (fdt_magic(fdt) == FDT_MAGIC)
			ଫେରନ୍ତୁ -FDT_ERR_BADSTATE;
		ଅନ୍ୟଥା ଯଦି (fdt_magic(fdt) != FDT_SW_MAGIC)
			ଫେରନ୍ତୁ -FDT_ERR_BADMAGIC;
	}

	ଫେରନ୍ତୁ 0;
}

#define FDT_SW_PROBE(fdt) \
	{ \
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା err; \
		ଯଦି ((err = fdt_sw_probe_(fdt)) != 0) \
			ଫେରନ୍ତୁ err; \
	}

/* 'memrsv' state:	Initial state after fdt_create()
 *
 * Allowed functions:
 *	fdt_add_reservemap_entry()
 *	fdt_finish_reservemap()		[moves to 'struct' state]
 */
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_sw_probe_memrsv_(ଶୂନ୍ୟ *fdt)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା err = fdt_sw_probe_(fdt);
	ଯଦି (err)
		ଫେରନ୍ତୁ err;

	ଯଦି (!can_assume(VALID_ନିବେଶ) && fdt_off_dt_strings(fdt) != 0)
		ଫେରନ୍ତୁ -FDT_ERR_BADSTATE;
	ଫେରନ୍ତୁ 0;
}

#define FDT_SW_PROBE_MEMRSV(fdt) \
	{ \
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା err; \
		ଯଦି ((err = fdt_sw_probe_memrsv_(fdt)) != 0) \
			ଫେରନ୍ତୁ err; \
	}

/* 'struct' state:	Enter this state after fdt_finish_reservemap()
 *
 * Allowed functions:
 *	fdt_begin_node()
 *	fdt_end_node()
 *	fdt_property*()
 *	fdt_finish()			[moves to 'complete' state]
 */
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_sw_probe_struct_(ଶୂନ୍ୟ *fdt)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା err = fdt_sw_probe_(fdt);
	ଯଦି (err)
		ଫେରନ୍ତୁ err;

	ଯଦି (!can_assume(VALID_ନିବେଶ) &&
	    fdt_off_dt_strings(fdt) != fdt_totalsize(fdt))
		ଫେରନ୍ତୁ -FDT_ERR_BADSTATE;
	ଫେରନ୍ତୁ 0;
}

#define FDT_SW_PROBE_STRUCT(fdt) \
	{ \
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା err; \
		ଯଦି ((err = fdt_sw_probe_struct_(fdt)) != 0) \
			ଫେରନ୍ତୁ err; \
	}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ uint32_t sw_flags(ଶୂନ୍ୟ *fdt)
{
	/* assert: (fdt_magic(fdt) == FDT_SW_MAGIC) */
	ଫେରନ୍ତୁ fdt_last_comp_version(fdt);
}

/* 'complete' state:	Enter this state after fdt_finish()
 *
 * Allowed functions: none
 */

ସ୍ଥିର ଶୂନ୍ୟ *fdt_grab_space_(ଶୂନ୍ୟ *fdt, size_t len)
{
	unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା offset = fdt_size_dt_struct(fdt);
	unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା spaceleft;

	spaceleft = fdt_totalsize(fdt) - fdt_off_dt_struct(fdt)
		- fdt_size_dt_strings(fdt);

	ଯଦି ((offset + len < offset) || (offset + len > spaceleft))
		ଫେରନ୍ତୁ ଖାଲି;

	fdt_set_size_dt_struct(fdt, offset + len);
	ଫେରନ୍ତୁ fdt_offset_ptr_w_(fdt, offset);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_create_with_flags(ଶୂନ୍ୟ *buf, ପୂର୍ଣ୍ଣସଂଖ୍ୟା bufsize, uint32_t flags)
{
	ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା hdrsize = FDT_ALIGN(ଆକାର(ସଂରଚନା fdt_header),
				      ଆକାର(ସଂରଚନା fdt_reserve_entry));
	ଶୂନ୍ୟ *fdt = buf;

	ଯଦି (bufsize < hdrsize)
		ଫେରନ୍ତୁ -FDT_ERR_NOSPACE;

	ଯଦି (flags & ~FDT_CREATE_FLAGS_ALL)
		ଫେରନ୍ତୁ -FDT_ERR_BADFLAGS;

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

	ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_create(ଶୂନ୍ୟ *buf, ପୂର୍ଣ୍ଣସଂଖ୍ୟା bufsize)
{
	ଫେରନ୍ତୁ fdt_create_with_flags(buf, bufsize, 0);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_resize(ଶୂନ୍ୟ *fdt, ଶୂନ୍ୟ *buf, ପୂର୍ଣ୍ଣସଂଖ୍ୟା bufsize)
{
	size_t headsize, tailsize;
	ଅକ୍ଷର *oldtail, *newtail;

	FDT_SW_PROBE(fdt);

	ଯଦି (bufsize < 0)
		ଫେରନ୍ତୁ -FDT_ERR_NOSPACE;

	headsize = fdt_off_dt_struct(fdt) + fdt_size_dt_struct(fdt);
	tailsize = fdt_size_dt_strings(fdt);

	ଯଦି (!can_assume(VALID_DTB) &&
	    headsize + tailsize > fdt_totalsize(fdt))
		ଫେରନ୍ତୁ -FDT_ERR_INTERNAL;

	ଯଦି ((headsize + tailsize) > (unsigned)bufsize)
		ଫେରନ୍ତୁ -FDT_ERR_NOSPACE;

	oldtail = (ଅକ୍ଷର *)fdt + fdt_totalsize(fdt) - tailsize;
	newtail = (ଅକ୍ଷର *)buf + bufsize - tailsize;

	/* Two cases to avoid clobbering data if the old and new
	 * buffers partially overlap */
	ଯଦି (buf <= fdt) {
		memmove(buf, fdt, headsize);
		memmove(newtail, oldtail, tailsize);
	} ଅନ୍ୟଥା {
		memmove(newtail, oldtail, tailsize);
		memmove(buf, fdt, headsize);
	}

	fdt_set_totalsize(buf, bufsize);
	ଯଦି (fdt_off_dt_strings(buf))
		fdt_set_off_dt_strings(buf, bufsize);

	ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_add_reservemap_entry(ଶୂନ୍ୟ *fdt, uint64_t addr, uint64_t size)
{
	ସଂରଚନା fdt_reserve_entry *re;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା offset;

	FDT_SW_PROBE_MEMRSV(fdt);

	offset = fdt_off_dt_struct(fdt);
	ଯଦି ((offset + ଆକାର(*re)) > fdt_totalsize(fdt))
		ଫେରନ୍ତୁ -FDT_ERR_NOSPACE;

	re = (ସଂରଚନା fdt_reserve_entry *)((ଅକ୍ଷର *)fdt + offset);
	re->address = cpu_to_fdt64(addr);
	re->size = cpu_to_fdt64(size);

	fdt_set_off_dt_struct(fdt, offset + ଆକାର(*re));

	ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_finish_reservemap(ଶୂନ୍ୟ *fdt)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା err = fdt_add_reservemap_entry(fdt, 0, 0);

	ଯଦି (err)
		ଫେରନ୍ତୁ err;

	fdt_set_off_dt_strings(fdt, fdt_totalsize(fdt));
	ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_begin_ଗଣ୍ଠି(ଶୂନ୍ୟ *fdt, ଅଚଳ ଅକ୍ଷର *name)
{
	ସଂରଚନା fdt_ଗଣ୍ଠି_header *nh;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା namelen;

	FDT_SW_PROBE_STRUCT(fdt);

	namelen = strlen(name) + 1;
	nh = fdt_grab_space_(fdt, ଆକାର(*nh) + FDT_TAGALIGN(namelen));
	ଯଦି (! nh)
		ଫେରନ୍ତୁ -FDT_ERR_NOSPACE;

	nh->tag = cpu_to_fdt32(FDT_BEGIN_ଗଣ୍ଠି);
	memcpy(nh->name, name, namelen);
	ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_end_ଗଣ୍ଠି(ଶୂନ୍ୟ *fdt)
{
	fdt32_t *en;

	FDT_SW_PROBE_STRUCT(fdt);

	en = fdt_grab_space_(fdt, FDT_TAGSIZE);
	ଯଦି (! en)
		ଫେରନ୍ତୁ -FDT_ERR_NOSPACE;

	*en = cpu_to_fdt32(FDT_END_ଗଣ୍ଠି);
	ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_add_string_(ଶୂନ୍ୟ *fdt, ଅଚଳ ଅକ୍ଷର *s)
{
	ଅକ୍ଷର *strtab = (ଅକ୍ଷର *)fdt + fdt_totalsize(fdt);
	unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା strtabsize = fdt_size_dt_strings(fdt);
	unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା len = strlen(s) + 1;
	unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା struct_ଶୀର୍ଷ, offset;

	offset = strtabsize + len;
	struct_ଶୀର୍ଷ = fdt_off_dt_struct(fdt) + fdt_size_dt_struct(fdt);
	ଯଦି (fdt_totalsize(fdt) - offset < struct_ଶୀର୍ଷ)
		ଫେରନ୍ତୁ 0; /* no more room :( */

	memcpy(strtab - offset, s, len);
	fdt_set_size_dt_strings(fdt, strtabsize + len);
	ଫେରନ୍ତୁ -offset;
}

/* Must only be used to roll back in case of error */
ସ୍ଥିର ଶୂନ୍ୟ fdt_del_last_string_(ଶୂନ୍ୟ *fdt, ଅଚଳ ଅକ୍ଷର *s)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା strtabsize = fdt_size_dt_strings(fdt);
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା len = strlen(s) + 1;

	fdt_set_size_dt_strings(fdt, strtabsize - len);
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_find_add_string_(ଶୂନ୍ୟ *fdt, ଅଚଳ ଅକ୍ଷର *s, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *allocated)
{
	ଅକ୍ଷର *strtab = (ଅକ୍ଷର *)fdt + fdt_totalsize(fdt);
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା strtabsize = fdt_size_dt_strings(fdt);
	ଅଚଳ ଅକ୍ଷର *p;

	*allocated = 0;

	p = fdt_find_string_(strtab - strtabsize, strtabsize, s);
	ଯଦି (p)
		ଫେରନ୍ତୁ p - strtab;

	*allocated = 1;

	ଫେରନ୍ତୁ fdt_add_string_(fdt, s);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_property_placeholder(ଶୂନ୍ୟ *fdt, ଅଚଳ ଅକ୍ଷର *name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା len, ଶୂନ୍ୟ **valp)
{
	ସଂରଚନା fdt_property *prop;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା nameoff;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା allocated;

	FDT_SW_PROBE_STRUCT(fdt);

	/* String de-duplication can be slow, _NO_NAME_DEDUP skips it */
	ଯଦି (sw_flags(fdt) & FDT_CREATE_FLAG_NO_NAME_DEDUP) {
		allocated = 1;
		nameoff = fdt_add_string_(fdt, name);
	} ଅନ୍ୟଥା {
		nameoff = fdt_find_add_string_(fdt, name, &allocated);
	}
	ଯଦି (nameoff == 0)
		ଫେରନ୍ତୁ -FDT_ERR_NOSPACE;

	prop = fdt_grab_space_(fdt, ଆକାର(*prop) + FDT_TAGALIGN(len));
	ଯଦି (! prop) {
		ଯଦି (allocated)
			fdt_del_last_string_(fdt, name);
		ଫେରନ୍ତୁ -FDT_ERR_NOSPACE;
	}

	prop->tag = cpu_to_fdt32(FDT_PROP);
	prop->nameoff = cpu_to_fdt32(nameoff);
	prop->len = cpu_to_fdt32(len);
	*valp = prop->data;
	ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_property(ଶୂନ୍ୟ *fdt, ଅଚଳ ଅକ୍ଷର *name, ଅଚଳ ଶୂନ୍ୟ *val, ପୂର୍ଣ୍ଣସଂଖ୍ୟା len)
{
	ଶୂନ୍ୟ *ptr;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret;

	ret = fdt_property_placeholder(fdt, name, len, &ptr);
	ଯଦି (ret)
		ଫେରନ୍ତୁ ret;
	memcpy(ptr, val, len);
	ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_finish(ଶୂନ୍ୟ *fdt)
{
	ଅକ୍ଷର *p = (ଅକ୍ଷର *)fdt;
	fdt32_t *end;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା oldstroffset, newstroffset;
	uint32_t tag;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା offset, nextoffset;

	FDT_SW_PROBE_STRUCT(fdt);

	/* Add terminator */
	end = fdt_grab_space_(fdt, ଆକାର(*end));
	ଯଦି (! end)
		ଫେରନ୍ତୁ -FDT_ERR_NOSPACE;
	*end = cpu_to_fdt32(FDT_END);

	/* Relocate the string table */
	oldstroffset = fdt_totalsize(fdt) - fdt_size_dt_strings(fdt);
	newstroffset = fdt_off_dt_struct(fdt) + fdt_size_dt_struct(fdt);
	memmove(p + newstroffset, p + oldstroffset, fdt_size_dt_strings(fdt));
	fdt_set_off_dt_strings(fdt, newstroffset);

	/* Walk the structure, correcting string offsets */
	offset = 0;
	ଯେତେବେଳେ ((tag = fdt_next_tag(fdt, offset, &nextoffset)) != FDT_END) {
		ଯଦି (tag == FDT_PROP) {
			ସଂରଚନା fdt_property *prop =
				fdt_offset_ptr_w_(fdt, offset);
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା nameoff;

			nameoff = fdt32_to_cpu(prop->nameoff);
			nameoff += fdt_size_dt_strings(fdt);
			prop->nameoff = cpu_to_fdt32(nameoff);
		}
		offset = nextoffset;
	}
	ଯଦି (nextoffset < 0)
		ଫେରନ୍ତୁ nextoffset;

	/* Finally, adjust the header */
	fdt_set_totalsize(fdt, newstroffset + fdt_size_dt_strings(fdt));

	/* And fix up fields that were keeping intermediate state. */
	fdt_set_last_comp_version(fdt, FDT_LAST_COMPATIBLE_VERSION);
	fdt_set_magic(fdt, FDT_MAGIC);

	ଫେରନ୍ତୁ 0;
}
