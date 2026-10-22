// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#অন্তর্ভাব "libfdt_env.h"

#অন্তর্ভাব <fdt.h>
#অন্তর্ভাব <libfdt.h>

#অন্তর্ভাব "libfdt_internal.h"

স্থির পূর্ণসংখ্যা fdt_sw_probe_(শূন্য *fdt)
{
	যদি (!can_assume(VALID_প্রবেশ)) {
		যদি (fdt_magic(fdt) == FDT_MAGIC)
			ফেরত -FDT_ERR_BADSTATE;
		অন্যথা যদি (fdt_magic(fdt) != FDT_SW_MAGIC)
			ফেরত -FDT_ERR_BADMAGIC;
	}

	ফেরত 0;
}

#define FDT_SW_PROBE(fdt) \
	{ \
		পূর্ণসংখ্যা err; \
		যদি ((err = fdt_sw_probe_(fdt)) != 0) \
			ফেরত err; \
	}

/* 'memrsv' state:	Initial state after fdt_create()
 *
 * Allowed functions:
 *	fdt_add_reservemap_entry()
 *	fdt_finish_reservemap()		[moves to 'struct' state]
 */
স্থির পূর্ণসংখ্যা fdt_sw_probe_memrsv_(শূন্য *fdt)
{
	পূর্ণসংখ্যা err = fdt_sw_probe_(fdt);
	যদি (err)
		ফেরত err;

	যদি (!can_assume(VALID_প্রবেশ) && fdt_off_dt_strings(fdt) != 0)
		ফেরত -FDT_ERR_BADSTATE;
	ফেরত 0;
}

#define FDT_SW_PROBE_MEMRSV(fdt) \
	{ \
		পূর্ণসংখ্যা err; \
		যদি ((err = fdt_sw_probe_memrsv_(fdt)) != 0) \
			ফেরত err; \
	}

/* 'struct' state:	Enter this state after fdt_finish_reservemap()
 *
 * Allowed functions:
 *	fdt_begin_node()
 *	fdt_end_node()
 *	fdt_property*()
 *	fdt_finish()			[moves to 'complete' state]
 */
স্থির পূর্ণসংখ্যা fdt_sw_probe_struct_(শূন্য *fdt)
{
	পূর্ণসংখ্যা err = fdt_sw_probe_(fdt);
	যদি (err)
		ফেরত err;

	যদি (!can_assume(VALID_প্রবেশ) &&
	    fdt_off_dt_strings(fdt) != fdt_totalsize(fdt))
		ফেরত -FDT_ERR_BADSTATE;
	ফেরত 0;
}

#define FDT_SW_PROBE_STRUCT(fdt) \
	{ \
		পূর্ণসংখ্যা err; \
		যদি ((err = fdt_sw_probe_struct_(fdt)) != 0) \
			ফেরত err; \
	}

স্থির অন্তঃরেখ uint32_t sw_flags(শূন্য *fdt)
{
	/* assert: (fdt_magic(fdt) == FDT_SW_MAGIC) */
	ফেরত fdt_last_comp_version(fdt);
}

/* 'complete' state:	Enter this state after fdt_finish()
 *
 * Allowed functions: none
 */

স্থির শূন্য *fdt_grab_space_(শূন্য *fdt, size_t len)
{
	unsigned পূর্ণসংখ্যা offset = fdt_আকার_dt_struct(fdt);
	unsigned পূর্ণসংখ্যা spaceleft;

	spaceleft = fdt_totalsize(fdt) - fdt_off_dt_struct(fdt)
		- fdt_আকার_dt_strings(fdt);

	যদি ((offset + len < offset) || (offset + len > spaceleft))
		ফেরত ফাঁকা;

	fdt_set_আকার_dt_struct(fdt, offset + len);
	ফেরত fdt_offset_ptr_w_(fdt, offset);
}

পূর্ণসংখ্যা fdt_create_with_flags(শূন্য *buf, পূর্ণসংখ্যা bufsize, uint32_t flags)
{
	ধ্রুবক পূর্ণসংখ্যা hdrsize = FDT_ALIGN(আকার(কাঠামো fdt_header),
				      আকার(কাঠামো fdt_reserve_entry));
	শূন্য *fdt = buf;

	যদি (bufsize < hdrsize)
		ফেরত -FDT_ERR_NOSPACE;

	যদি (flags & ~FDT_CREATE_FLAGS_ALL)
		ফেরত -FDT_ERR_BADFLAGS;

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

	ফেরত 0;
}

পূর্ণসংখ্যা fdt_create(শূন্য *buf, পূর্ণসংখ্যা bufsize)
{
	ফেরত fdt_create_with_flags(buf, bufsize, 0);
}

পূর্ণসংখ্যা fdt_resize(শূন্য *fdt, শূন্য *buf, পূর্ণসংখ্যা bufsize)
{
	size_t headsize, tailsize;
	অক্ষর *oldtail, *newtail;

	FDT_SW_PROBE(fdt);

	যদি (bufsize < 0)
		ফেরত -FDT_ERR_NOSPACE;

	headsize = fdt_off_dt_struct(fdt) + fdt_আকার_dt_struct(fdt);
	tailsize = fdt_আকার_dt_strings(fdt);

	যদি (!can_assume(VALID_DTB) &&
	    headsize + tailsize > fdt_totalsize(fdt))
		ফেরত -FDT_ERR_INTERNAL;

	যদি ((headsize + tailsize) > (unsigned)bufsize)
		ফেরত -FDT_ERR_NOSPACE;

	oldtail = (অক্ষর *)fdt + fdt_totalsize(fdt) - tailsize;
	newtail = (অক্ষর *)buf + bufsize - tailsize;

	/* Two cases to avoid clobbering data if the old and new
	 * buffers partially overlap */
	যদি (buf <= fdt) {
		memmove(buf, fdt, headsize);
		memmove(newtail, oldtail, tailsize);
	} অন্যথা {
		memmove(newtail, oldtail, tailsize);
		memmove(buf, fdt, headsize);
	}

	fdt_set_totalsize(buf, bufsize);
	যদি (fdt_off_dt_strings(buf))
		fdt_set_off_dt_strings(buf, bufsize);

	ফেরত 0;
}

পূর্ণসংখ্যা fdt_add_reservemap_entry(শূন্য *fdt, uint64_t addr, uint64_t আকার_2)
{
	কাঠামো fdt_reserve_entry *re;
	পূর্ণসংখ্যা offset;

	FDT_SW_PROBE_MEMRSV(fdt);

	offset = fdt_off_dt_struct(fdt);
	যদি ((offset + আকার(*re)) > fdt_totalsize(fdt))
		ফেরত -FDT_ERR_NOSPACE;

	re = (কাঠামো fdt_reserve_entry *)((অক্ষর *)fdt + offset);
	re->address = cpu_to_fdt64(addr);
	re->আকার_2 = cpu_to_fdt64(আকার_2);

	fdt_set_off_dt_struct(fdt, offset + আকার(*re));

	ফেরত 0;
}

পূর্ণসংখ্যা fdt_finish_reservemap(শূন্য *fdt)
{
	পূর্ণসংখ্যা err = fdt_add_reservemap_entry(fdt, 0, 0);

	যদি (err)
		ফেরত err;

	fdt_set_off_dt_strings(fdt, fdt_totalsize(fdt));
	ফেরত 0;
}

পূর্ণসংখ্যা fdt_begin_গ্রন্থি(শূন্য *fdt, ধ্রুবক অক্ষর *name)
{
	কাঠামো fdt_গ্রন্থি_header *nh;
	পূর্ণসংখ্যা namelen;

	FDT_SW_PROBE_STRUCT(fdt);

	namelen = strlen(name) + 1;
	nh = fdt_grab_space_(fdt, আকার(*nh) + FDT_TAGALIGN(namelen));
	যদি (! nh)
		ফেরত -FDT_ERR_NOSPACE;

	nh->tag = cpu_to_fdt32(FDT_BEGIN_গ্রন্থি);
	memcpy(nh->name, name, namelen);
	ফেরত 0;
}

পূর্ণসংখ্যা fdt_end_গ্রন্থি(শূন্য *fdt)
{
	fdt32_t *en;

	FDT_SW_PROBE_STRUCT(fdt);

	en = fdt_grab_space_(fdt, FDT_TAGSIZE);
	যদি (! en)
		ফেরত -FDT_ERR_NOSPACE;

	*en = cpu_to_fdt32(FDT_END_গ্রন্থি);
	ফেরত 0;
}

স্থির পূর্ণসংখ্যা fdt_add_string_(শূন্য *fdt, ধ্রুবক অক্ষর *s)
{
	অক্ষর *strtab = (অক্ষর *)fdt + fdt_totalsize(fdt);
	unsigned পূর্ণসংখ্যা strtabsize = fdt_আকার_dt_strings(fdt);
	unsigned পূর্ণসংখ্যা len = strlen(s) + 1;
	unsigned পূর্ণসংখ্যা struct_শীর্ষ, offset;

	offset = strtabsize + len;
	struct_শীর্ষ = fdt_off_dt_struct(fdt) + fdt_আকার_dt_struct(fdt);
	যদি (fdt_totalsize(fdt) - offset < struct_শীর্ষ)
		ফেরত 0; /* no more room :( */

	memcpy(strtab - offset, s, len);
	fdt_set_আকার_dt_strings(fdt, strtabsize + len);
	ফেরত -offset;
}

/* Must only be used to roll back in case of error */
স্থির শূন্য fdt_del_last_string_(শূন্য *fdt, ধ্রুবক অক্ষর *s)
{
	পূর্ণসংখ্যা strtabsize = fdt_আকার_dt_strings(fdt);
	পূর্ণসংখ্যা len = strlen(s) + 1;

	fdt_set_আকার_dt_strings(fdt, strtabsize - len);
}

স্থির পূর্ণসংখ্যা fdt_find_add_string_(শূন্য *fdt, ধ্রুবক অক্ষর *s, পূর্ণসংখ্যা *allocated)
{
	অক্ষর *strtab = (অক্ষর *)fdt + fdt_totalsize(fdt);
	পূর্ণসংখ্যা strtabsize = fdt_আকার_dt_strings(fdt);
	ধ্রুবক অক্ষর *p;

	*allocated = 0;

	p = fdt_find_string_(strtab - strtabsize, strtabsize, s);
	যদি (p)
		ফেরত p - strtab;

	*allocated = 1;

	ফেরত fdt_add_string_(fdt, s);
}

পূর্ণসংখ্যা fdt_property_placeholder(শূন্য *fdt, ধ্রুবক অক্ষর *name, পূর্ণসংখ্যা len, শূন্য **valp)
{
	কাঠামো fdt_property *prop;
	পূর্ণসংখ্যা nameoff;
	পূর্ণসংখ্যা allocated;

	FDT_SW_PROBE_STRUCT(fdt);

	/* String de-duplication can be slow, _NO_NAME_DEDUP skips it */
	যদি (sw_flags(fdt) & FDT_CREATE_FLAG_NO_NAME_DEDUP) {
		allocated = 1;
		nameoff = fdt_add_string_(fdt, name);
	} অন্যথা {
		nameoff = fdt_find_add_string_(fdt, name, &allocated);
	}
	যদি (nameoff == 0)
		ফেরত -FDT_ERR_NOSPACE;

	prop = fdt_grab_space_(fdt, আকার(*prop) + FDT_TAGALIGN(len));
	যদি (! prop) {
		যদি (allocated)
			fdt_del_last_string_(fdt, name);
		ফেরত -FDT_ERR_NOSPACE;
	}

	prop->tag = cpu_to_fdt32(FDT_PROP);
	prop->nameoff = cpu_to_fdt32(nameoff);
	prop->len = cpu_to_fdt32(len);
	*valp = prop->তথ্য;
	ফেরত 0;
}

পূর্ণসংখ্যা fdt_property(শূন্য *fdt, ধ্রুবক অক্ষর *name, ধ্রুবক শূন্য *val, পূর্ণসংখ্যা len)
{
	শূন্য *ptr;
	পূর্ণসংখ্যা ret;

	ret = fdt_property_placeholder(fdt, name, len, &ptr);
	যদি (ret)
		ফেরত ret;
	memcpy(ptr, val, len);
	ফেরত 0;
}

পূর্ণসংখ্যা fdt_finish(শূন্য *fdt)
{
	অক্ষর *p = (অক্ষর *)fdt;
	fdt32_t *end;
	পূর্ণসংখ্যা oldstroffset, newstroffset;
	uint32_t tag;
	পূর্ণসংখ্যা offset, nextoffset;

	FDT_SW_PROBE_STRUCT(fdt);

	/* Add terminator */
	end = fdt_grab_space_(fdt, আকার(*end));
	যদি (! end)
		ফেরত -FDT_ERR_NOSPACE;
	*end = cpu_to_fdt32(FDT_END);

	/* Relocate the string table */
	oldstroffset = fdt_totalsize(fdt) - fdt_আকার_dt_strings(fdt);
	newstroffset = fdt_off_dt_struct(fdt) + fdt_আকার_dt_struct(fdt);
	memmove(p + newstroffset, p + oldstroffset, fdt_আকার_dt_strings(fdt));
	fdt_set_off_dt_strings(fdt, newstroffset);

	/* Walk the structure, correcting string offsets */
	offset = 0;
	যখন ((tag = fdt_পরবর্তী_tag(fdt, offset, &nextoffset)) != FDT_END) {
		যদি (tag == FDT_PROP) {
			কাঠামো fdt_property *prop =
				fdt_offset_ptr_w_(fdt, offset);
			পূর্ণসংখ্যা nameoff;

			nameoff = fdt32_to_cpu(prop->nameoff);
			nameoff += fdt_আকার_dt_strings(fdt);
			prop->nameoff = cpu_to_fdt32(nameoff);
		}
		offset = nextoffset;
	}
	যদি (nextoffset < 0)
		ফেরত nextoffset;

	/* Finally, adjust the header */
	fdt_set_totalsize(fdt, newstroffset + fdt_আকার_dt_strings(fdt));

	/* And fix up fields that were keeping intermediate state. */
	fdt_set_last_comp_version(fdt, FDT_LAST_COMPATIBLE_VERSION);
	fdt_set_magic(fdt, FDT_MAGIC);

	ফেরত 0;
}
