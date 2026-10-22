// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#ضمن "libfdt_env.h"

#ضمن <fdt.h>
#ضمن <libfdt.h>

#ضمن "libfdt_internal.h"

ساكن صحيح fdt_sw_probe_(فراغ *fdt)
{
	إذا (!can_assume(VALID_تێخستن)) {
		إذا (fdt_magic(fdt) == FDT_MAGIC)
			أرجع -FDT_ERR_BADSTATE;
		وإلا إذا (fdt_magic(fdt) != FDT_SW_MAGIC)
			أرجع -FDT_ERR_BADMAGIC;
	}

	أرجع 0;
}

#define FDT_SW_PROBE(fdt) \
	{ \
		صحيح err; \
		إذا ((err = fdt_sw_probe_(fdt)) != 0) \
			أرجع err; \
	}

/* 'memrsv' state:	Initial state after fdt_create()
 *
 * Allowed functions:
 *	fdt_add_reservemap_entry()
 *	fdt_finish_reservemap()		[moves to 'struct' state]
 */
ساكن صحيح fdt_sw_probe_memrsv_(فراغ *fdt)
{
	صحيح err = fdt_sw_probe_(fdt);
	إذا (err)
		أرجع err;

	إذا (!can_assume(VALID_تێخستن) && fdt_off_dt_strings(fdt) != 0)
		أرجع -FDT_ERR_BADSTATE;
	أرجع 0;
}

#define FDT_SW_PROBE_MEMRSV(fdt) \
	{ \
		صحيح err; \
		إذا ((err = fdt_sw_probe_memrsv_(fdt)) != 0) \
			أرجع err; \
	}

/* 'struct' state:	Enter this state after fdt_finish_reservemap()
 *
 * Allowed functions:
 *	fdt_begin_node()
 *	fdt_end_node()
 *	fdt_property*()
 *	fdt_finish()			[moves to 'complete' state]
 */
ساكن صحيح fdt_sw_probe_struct_(فراغ *fdt)
{
	صحيح err = fdt_sw_probe_(fdt);
	إذا (err)
		أرجع err;

	إذا (!can_assume(VALID_تێخستن) &&
	    fdt_off_dt_strings(fdt) != fdt_totalsize(fdt))
		أرجع -FDT_ERR_BADSTATE;
	أرجع 0;
}

#define FDT_SW_PROBE_STRUCT(fdt) \
	{ \
		صحيح err; \
		إذا ((err = fdt_sw_probe_struct_(fdt)) != 0) \
			أرجع err; \
	}

ساكن مضمن uint32_t sw_flags(فراغ *fdt)
{
	/* assert: (fdt_magic(fdt) == FDT_SW_MAGIC) */
	أرجع fdt_last_comp_version(fdt);
}

/* 'complete' state:	Enter this state after fdt_finish()
 *
 * Allowed functions: none
 */

ساكن فراغ *fdt_grab_space_(فراغ *fdt, size_t len)
{
	unsigned صحيح offset = fdt_size_dt_struct(fdt);
	unsigned صحيح spaceleft;

	spaceleft = fdt_totalsize(fdt) - fdt_off_dt_struct(fdt)
		- fdt_size_dt_strings(fdt);

	إذا ((offset + len < offset) || (offset + len > spaceleft))
		أرجع عدم;

	fdt_set_size_dt_struct(fdt, offset + len);
	أرجع fdt_offset_ptr_w_(fdt, offset);
}

صحيح fdt_create_with_flags(فراغ *buf, صحيح bufsize, uint32_t flags)
{
	ثابت صحيح hdrsize = FDT_ALIGN(حجم(بنية fdt_header),
				      حجم(بنية fdt_reserve_entry));
	فراغ *fdt = buf;

	إذا (bufsize < hdrsize)
		أرجع -FDT_ERR_NOSPACE;

	إذا (flags & ~FDT_CREATE_FLAGS_ALL)
		أرجع -FDT_ERR_BADFLAGS;

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

	أرجع 0;
}

صحيح fdt_create(فراغ *buf, صحيح bufsize)
{
	أرجع fdt_create_with_flags(buf, bufsize, 0);
}

صحيح fdt_resize(فراغ *fdt, فراغ *buf, صحيح bufsize)
{
	size_t headsize, tailsize;
	حرف *oldtail, *newtail;

	FDT_SW_PROBE(fdt);

	إذا (bufsize < 0)
		أرجع -FDT_ERR_NOSPACE;

	headsize = fdt_off_dt_struct(fdt) + fdt_size_dt_struct(fdt);
	tailsize = fdt_size_dt_strings(fdt);

	إذا (!can_assume(VALID_DTB) &&
	    headsize + tailsize > fdt_totalsize(fdt))
		أرجع -FDT_ERR_INTERNAL;

	إذا ((headsize + tailsize) > (unsigned)bufsize)
		أرجع -FDT_ERR_NOSPACE;

	oldtail = (حرف *)fdt + fdt_totalsize(fdt) - tailsize;
	newtail = (حرف *)buf + bufsize - tailsize;

	/* Two cases to avoid clobbering data if the old and new
	 * buffers partially overlap */
	إذا (buf <= fdt) {
		memmove(buf, fdt, headsize);
		memmove(newtail, oldtail, tailsize);
	} وإلا {
		memmove(newtail, oldtail, tailsize);
		memmove(buf, fdt, headsize);
	}

	fdt_set_totalsize(buf, bufsize);
	إذا (fdt_off_dt_strings(buf))
		fdt_set_off_dt_strings(buf, bufsize);

	أرجع 0;
}

صحيح fdt_add_reservemap_entry(فراغ *fdt, uint64_t addr, uint64_t size)
{
	بنية fdt_reserve_entry *re;
	صحيح offset;

	FDT_SW_PROBE_MEMRSV(fdt);

	offset = fdt_off_dt_struct(fdt);
	إذا ((offset + حجم(*re)) > fdt_totalsize(fdt))
		أرجع -FDT_ERR_NOSPACE;

	re = (بنية fdt_reserve_entry *)((حرف *)fdt + offset);
	re->address = cpu_to_fdt64(addr);
	re->size = cpu_to_fdt64(size);

	fdt_set_off_dt_struct(fdt, offset + حجم(*re));

	أرجع 0;
}

صحيح fdt_finish_reservemap(فراغ *fdt)
{
	صحيح err = fdt_add_reservemap_entry(fdt, 0, 0);

	إذا (err)
		أرجع err;

	fdt_set_off_dt_strings(fdt, fdt_totalsize(fdt));
	أرجع 0;
}

صحيح fdt_begin_گرێ(فراغ *fdt, ثابت حرف *name)
{
	بنية fdt_گرێ_header *nh;
	صحيح namelen;

	FDT_SW_PROBE_STRUCT(fdt);

	namelen = strlen(name) + 1;
	nh = fdt_grab_space_(fdt, حجم(*nh) + FDT_TAGALIGN(namelen));
	إذا (! nh)
		أرجع -FDT_ERR_NOSPACE;

	nh->tag = cpu_to_fdt32(FDT_BEGIN_گرێ);
	memcpy(nh->name, name, namelen);
	أرجع 0;
}

صحيح fdt_end_گرێ(فراغ *fdt)
{
	fdt32_t *en;

	FDT_SW_PROBE_STRUCT(fdt);

	en = fdt_grab_space_(fdt, FDT_TAGSIZE);
	إذا (! en)
		أرجع -FDT_ERR_NOSPACE;

	*en = cpu_to_fdt32(FDT_END_گرێ);
	أرجع 0;
}

ساكن صحيح fdt_add_string_(فراغ *fdt, ثابت حرف *s)
{
	حرف *strtab = (حرف *)fdt + fdt_totalsize(fdt);
	unsigned صحيح strtabsize = fdt_size_dt_strings(fdt);
	unsigned صحيح len = strlen(s) + 1;
	unsigned صحيح struct_لووتکە, offset;

	offset = strtabsize + len;
	struct_لووتکە = fdt_off_dt_struct(fdt) + fdt_size_dt_struct(fdt);
	إذا (fdt_totalsize(fdt) - offset < struct_لووتکە)
		أرجع 0; /* no more room :( */

	memcpy(strtab - offset, s, len);
	fdt_set_size_dt_strings(fdt, strtabsize + len);
	أرجع -offset;
}

/* Must only be used to roll back in case of error */
ساكن فراغ fdt_del_last_string_(فراغ *fdt, ثابت حرف *s)
{
	صحيح strtabsize = fdt_size_dt_strings(fdt);
	صحيح len = strlen(s) + 1;

	fdt_set_size_dt_strings(fdt, strtabsize - len);
}

ساكن صحيح fdt_find_add_string_(فراغ *fdt, ثابت حرف *s, صحيح *allocated)
{
	حرف *strtab = (حرف *)fdt + fdt_totalsize(fdt);
	صحيح strtabsize = fdt_size_dt_strings(fdt);
	ثابت حرف *p;

	*allocated = 0;

	p = fdt_find_string_(strtab - strtabsize, strtabsize, s);
	إذا (p)
		أرجع p - strtab;

	*allocated = 1;

	أرجع fdt_add_string_(fdt, s);
}

صحيح fdt_property_placeholder(فراغ *fdt, ثابت حرف *name, صحيح len, فراغ **valp)
{
	بنية fdt_property *prop;
	صحيح nameoff;
	صحيح allocated;

	FDT_SW_PROBE_STRUCT(fdt);

	/* String de-duplication can be slow, _NO_NAME_DEDUP skips it */
	إذا (sw_flags(fdt) & FDT_CREATE_FLAG_NO_NAME_DEDUP) {
		allocated = 1;
		nameoff = fdt_add_string_(fdt, name);
	} وإلا {
		nameoff = fdt_find_add_string_(fdt, name, &allocated);
	}
	إذا (nameoff == 0)
		أرجع -FDT_ERR_NOSPACE;

	prop = fdt_grab_space_(fdt, حجم(*prop) + FDT_TAGALIGN(len));
	إذا (! prop) {
		إذا (allocated)
			fdt_del_last_string_(fdt, name);
		أرجع -FDT_ERR_NOSPACE;
	}

	prop->tag = cpu_to_fdt32(FDT_PROP);
	prop->nameoff = cpu_to_fdt32(nameoff);
	prop->len = cpu_to_fdt32(len);
	*valp = prop->data;
	أرجع 0;
}

صحيح fdt_property(فراغ *fdt, ثابت حرف *name, ثابت فراغ *val, صحيح len)
{
	فراغ *ptr;
	صحيح ret;

	ret = fdt_property_placeholder(fdt, name, len, &ptr);
	إذا (ret)
		أرجع ret;
	memcpy(ptr, val, len);
	أرجع 0;
}

صحيح fdt_finish(فراغ *fdt)
{
	حرف *p = (حرف *)fdt;
	fdt32_t *end;
	صحيح oldstroffset, newstroffset;
	uint32_t tag;
	صحيح offset, nextoffset;

	FDT_SW_PROBE_STRUCT(fdt);

	/* Add terminator */
	end = fdt_grab_space_(fdt, حجم(*end));
	إذا (! end)
		أرجع -FDT_ERR_NOSPACE;
	*end = cpu_to_fdt32(FDT_END);

	/* Relocate the string table */
	oldstroffset = fdt_totalsize(fdt) - fdt_size_dt_strings(fdt);
	newstroffset = fdt_off_dt_struct(fdt) + fdt_size_dt_struct(fdt);
	memmove(p + newstroffset, p + oldstroffset, fdt_size_dt_strings(fdt));
	fdt_set_off_dt_strings(fdt, newstroffset);

	/* Walk the structure, correcting string offsets */
	offset = 0;
	طالما ((tag = fdt_next_tag(fdt, offset, &nextoffset)) != FDT_END) {
		إذا (tag == FDT_PROP) {
			بنية fdt_property *prop =
				fdt_offset_ptr_w_(fdt, offset);
			صحيح nameoff;

			nameoff = fdt32_to_cpu(prop->nameoff);
			nameoff += fdt_size_dt_strings(fdt);
			prop->nameoff = cpu_to_fdt32(nameoff);
		}
		offset = nextoffset;
	}
	إذا (nextoffset < 0)
		أرجع nextoffset;

	/* Finally, adjust the header */
	fdt_set_totalsize(fdt, newstroffset + fdt_size_dt_strings(fdt));

	/* And fix up fields that were keeping intermediate state. */
	fdt_set_last_comp_version(fdt, FDT_LAST_COMPATIBLE_VERSION);
	fdt_set_magic(fdt, FDT_MAGIC);

	أرجع 0;
}
