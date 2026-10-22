// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#অন্তর্ভাৱ "libfdt_env.h"

#অন্তর্ভাৱ <fdt.h>
#অন্তর্ভাৱ <libfdt.h>

#অন্তর্ভাৱ "libfdt_internal.h"

স্থির পূর্ণাঙ্ক fdt_sw_probe_(শূন্য *fdt)
{
	যদি (!can_assume(VALID_প্ৰবেশ)) {
		যদি (fdt_magic(fdt) == FDT_MAGIC)
			ঘূৰাই -FDT_ERR_BADSTATE;
		অন্যথা যদি (fdt_magic(fdt) != FDT_SW_MAGIC)
			ঘূৰাই -FDT_ERR_BADMAGIC;
	}

	ঘূৰাই 0;
}

#define FDT_SW_PROBE(fdt) \
	{ \
		পূর্ণাঙ্ক err; \
		যদি ((err = fdt_sw_probe_(fdt)) != 0) \
			ঘূৰাই err; \
	}

/* 'memrsv' state:	Initial state after fdt_create()
 *
 * Allowed functions:
 *	fdt_add_reservemap_entry()
 *	fdt_finish_reservemap()		[moves to 'struct' state]
 */
স্থির পূর্ণাঙ্ক fdt_sw_probe_memrsv_(শূন্য *fdt)
{
	পূর্ণাঙ্ক err = fdt_sw_probe_(fdt);
	যদি (err)
		ঘূৰাই err;

	যদি (!can_assume(VALID_প্ৰবেশ) && fdt_off_dt_strings(fdt) != 0)
		ঘূৰাই -FDT_ERR_BADSTATE;
	ঘূৰাই 0;
}

#define FDT_SW_PROBE_MEMRSV(fdt) \
	{ \
		পূর্ণাঙ্ক err; \
		যদি ((err = fdt_sw_probe_memrsv_(fdt)) != 0) \
			ঘূৰাই err; \
	}

/* 'struct' state:	Enter this state after fdt_finish_reservemap()
 *
 * Allowed functions:
 *	fdt_begin_node()
 *	fdt_end_node()
 *	fdt_property*()
 *	fdt_finish()			[moves to 'complete' state]
 */
স্থির পূর্ণাঙ্ক fdt_sw_probe_struct_(শূন্য *fdt)
{
	পূর্ণাঙ্ক err = fdt_sw_probe_(fdt);
	যদি (err)
		ঘূৰাই err;

	যদি (!can_assume(VALID_প্ৰবেশ) &&
	    fdt_off_dt_strings(fdt) != fdt_totalsize(fdt))
		ঘূৰাই -FDT_ERR_BADSTATE;
	ঘূৰাই 0;
}

#define FDT_SW_PROBE_STRUCT(fdt) \
	{ \
		পূর্ণাঙ্ক err; \
		যদি ((err = fdt_sw_probe_struct_(fdt)) != 0) \
			ঘূৰাই err; \
	}

স্থির অন্তঃস্থ uint32_t sw_flags(শূন্য *fdt)
{
	/* assert: (fdt_magic(fdt) == FDT_SW_MAGIC) */
	ঘূৰাই fdt_last_comp_version(fdt);
}

/* 'complete' state:	Enter this state after fdt_finish()
 *
 * Allowed functions: none
 */

স্থির শূন্য *fdt_grab_space_(শূন্য *fdt, size_t len)
{
	unsigned পূর্ণাঙ্ক offset = fdt_size_dt_struct(fdt);
	unsigned পূর্ণাঙ্ক spaceleft;

	spaceleft = fdt_totalsize(fdt) - fdt_off_dt_struct(fdt)
		- fdt_size_dt_strings(fdt);

	যদি ((offset + len < offset) || (offset + len > spaceleft))
		ঘূৰাই রিক্ত;

	fdt_set_size_dt_struct(fdt, offset + len);
	ঘূৰাই fdt_offset_ptr_w_(fdt, offset);
}

পূর্ণাঙ্ক fdt_create_with_flags(শূন্য *buf, পূর্ণাঙ্ক bufsize, uint32_t flags)
{
	ধ্রুৱ পূর্ণাঙ্ক hdrsize = FDT_ALIGN(পরিমাণ(সংরচনা fdt_header),
				      পরিমাণ(সংরচনা fdt_reserve_entry));
	শূন্য *fdt = buf;

	যদি (bufsize < hdrsize)
		ঘূৰাই -FDT_ERR_NOSPACE;

	যদি (flags & ~FDT_CREATE_FLAGS_ALL)
		ঘূৰাই -FDT_ERR_BADFLAGS;

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

	ঘূৰাই 0;
}

পূর্ণাঙ্ক fdt_create(শূন্য *buf, পূর্ণাঙ্ক bufsize)
{
	ঘূৰাই fdt_create_with_flags(buf, bufsize, 0);
}

পূর্ণাঙ্ক fdt_resize(শূন্য *fdt, শূন্য *buf, পূর্ণাঙ্ক bufsize)
{
	size_t headsize, tailsize;
	ৱর্ণ *oldtail, *newtail;

	FDT_SW_PROBE(fdt);

	যদি (bufsize < 0)
		ঘূৰাই -FDT_ERR_NOSPACE;

	headsize = fdt_off_dt_struct(fdt) + fdt_size_dt_struct(fdt);
	tailsize = fdt_size_dt_strings(fdt);

	যদি (!can_assume(VALID_DTB) &&
	    headsize + tailsize > fdt_totalsize(fdt))
		ঘূৰাই -FDT_ERR_INTERNAL;

	যদি ((headsize + tailsize) > (unsigned)bufsize)
		ঘূৰাই -FDT_ERR_NOSPACE;

	oldtail = (ৱর্ণ *)fdt + fdt_totalsize(fdt) - tailsize;
	newtail = (ৱর্ণ *)buf + bufsize - tailsize;

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

	ঘূৰাই 0;
}

পূর্ণাঙ্ক fdt_add_reservemap_entry(শূন্য *fdt, uint64_t addr, uint64_t size)
{
	সংরচনা fdt_reserve_entry *re;
	পূর্ণাঙ্ক offset;

	FDT_SW_PROBE_MEMRSV(fdt);

	offset = fdt_off_dt_struct(fdt);
	যদি ((offset + পরিমাণ(*re)) > fdt_totalsize(fdt))
		ঘূৰাই -FDT_ERR_NOSPACE;

	re = (সংরচনা fdt_reserve_entry *)((ৱর্ণ *)fdt + offset);
	re->address = cpu_to_fdt64(addr);
	re->size = cpu_to_fdt64(size);

	fdt_set_off_dt_struct(fdt, offset + পরিমাণ(*re));

	ঘূৰাই 0;
}

পূর্ণাঙ্ক fdt_finish_reservemap(শূন্য *fdt)
{
	পূর্ণাঙ্ক err = fdt_add_reservemap_entry(fdt, 0, 0);

	যদি (err)
		ঘূৰাই err;

	fdt_set_off_dt_strings(fdt, fdt_totalsize(fdt));
	ঘূৰাই 0;
}

পূর্ণাঙ্ক fdt_begin_গ্ৰন্থি(শূন্য *fdt, ধ্রুৱ ৱর্ণ *name)
{
	সংরচনা fdt_গ্ৰন্থি_header *nh;
	পূর্ণাঙ্ক namelen;

	FDT_SW_PROBE_STRUCT(fdt);

	namelen = strlen(name) + 1;
	nh = fdt_grab_space_(fdt, পরিমাণ(*nh) + FDT_TAGALIGN(namelen));
	যদি (! nh)
		ঘূৰাই -FDT_ERR_NOSPACE;

	nh->tag = cpu_to_fdt32(FDT_BEGIN_গ্ৰন্থি);
	memcpy(nh->name, name, namelen);
	ঘূৰাই 0;
}

পূর্ণাঙ্ক fdt_end_গ্ৰন্থি(শূন্য *fdt)
{
	fdt32_t *en;

	FDT_SW_PROBE_STRUCT(fdt);

	en = fdt_grab_space_(fdt, FDT_TAGSIZE);
	যদি (! en)
		ঘূৰাই -FDT_ERR_NOSPACE;

	*en = cpu_to_fdt32(FDT_END_গ্ৰন্থি);
	ঘূৰাই 0;
}

স্থির পূর্ণাঙ্ক fdt_add_string_(শূন্য *fdt, ধ্রুৱ ৱর্ণ *s)
{
	ৱর্ণ *strtab = (ৱর্ণ *)fdt + fdt_totalsize(fdt);
	unsigned পূর্ণাঙ্ক strtabsize = fdt_size_dt_strings(fdt);
	unsigned পূর্ণাঙ্ক len = strlen(s) + 1;
	unsigned পূর্ণাঙ্ক struct_শীৰ্ষ, offset;

	offset = strtabsize + len;
	struct_শীৰ্ষ = fdt_off_dt_struct(fdt) + fdt_size_dt_struct(fdt);
	যদি (fdt_totalsize(fdt) - offset < struct_শীৰ্ষ)
		ঘূৰাই 0; /* no more room :( */

	memcpy(strtab - offset, s, len);
	fdt_set_size_dt_strings(fdt, strtabsize + len);
	ঘূৰাই -offset;
}

/* Must only be used to roll back in case of error */
স্থির শূন্য fdt_del_last_string_(শূন্য *fdt, ধ্রুৱ ৱর্ণ *s)
{
	পূর্ণাঙ্ক strtabsize = fdt_size_dt_strings(fdt);
	পূর্ণাঙ্ক len = strlen(s) + 1;

	fdt_set_size_dt_strings(fdt, strtabsize - len);
}

স্থির পূর্ণাঙ্ক fdt_find_add_string_(শূন্য *fdt, ধ্রুৱ ৱর্ণ *s, পূর্ণাঙ্ক *allocated)
{
	ৱর্ণ *strtab = (ৱর্ণ *)fdt + fdt_totalsize(fdt);
	পূর্ণাঙ্ক strtabsize = fdt_size_dt_strings(fdt);
	ধ্রুৱ ৱর্ণ *p;

	*allocated = 0;

	p = fdt_find_string_(strtab - strtabsize, strtabsize, s);
	যদি (p)
		ঘূৰাই p - strtab;

	*allocated = 1;

	ঘূৰাই fdt_add_string_(fdt, s);
}

পূর্ণাঙ্ক fdt_property_placeholder(শূন্য *fdt, ধ্রুৱ ৱর্ণ *name, পূর্ণাঙ্ক len, শূন্য **valp)
{
	সংরচনা fdt_property *prop;
	পূর্ণাঙ্ক nameoff;
	পূর্ণাঙ্ক allocated;

	FDT_SW_PROBE_STRUCT(fdt);

	/* String de-duplication can be slow, _NO_NAME_DEDUP skips it */
	যদি (sw_flags(fdt) & FDT_CREATE_FLAG_NO_NAME_DEDUP) {
		allocated = 1;
		nameoff = fdt_add_string_(fdt, name);
	} অন্যথা {
		nameoff = fdt_find_add_string_(fdt, name, &allocated);
	}
	যদি (nameoff == 0)
		ঘূৰাই -FDT_ERR_NOSPACE;

	prop = fdt_grab_space_(fdt, পরিমাণ(*prop) + FDT_TAGALIGN(len));
	যদি (! prop) {
		যদি (allocated)
			fdt_del_last_string_(fdt, name);
		ঘূৰাই -FDT_ERR_NOSPACE;
	}

	prop->tag = cpu_to_fdt32(FDT_PROP);
	prop->nameoff = cpu_to_fdt32(nameoff);
	prop->len = cpu_to_fdt32(len);
	*valp = prop->data;
	ঘূৰাই 0;
}

পূর্ণাঙ্ক fdt_property(শূন্য *fdt, ধ্রুৱ ৱর্ণ *name, ধ্রুৱ শূন্য *val, পূর্ণাঙ্ক len)
{
	শূন্য *ptr;
	পূর্ণাঙ্ক ret;

	ret = fdt_property_placeholder(fdt, name, len, &ptr);
	যদি (ret)
		ঘূৰাই ret;
	memcpy(ptr, val, len);
	ঘূৰাই 0;
}

পূর্ণাঙ্ক fdt_finish(শূন্য *fdt)
{
	ৱর্ণ *p = (ৱর্ণ *)fdt;
	fdt32_t *end;
	পূর্ণাঙ্ক oldstroffset, newstroffset;
	uint32_t tag;
	পূর্ণাঙ্ক offset, nextoffset;

	FDT_SW_PROBE_STRUCT(fdt);

	/* Add terminator */
	end = fdt_grab_space_(fdt, পরিমাণ(*end));
	যদি (! end)
		ঘূৰাই -FDT_ERR_NOSPACE;
	*end = cpu_to_fdt32(FDT_END);

	/* Relocate the string table */
	oldstroffset = fdt_totalsize(fdt) - fdt_size_dt_strings(fdt);
	newstroffset = fdt_off_dt_struct(fdt) + fdt_size_dt_struct(fdt);
	memmove(p + newstroffset, p + oldstroffset, fdt_size_dt_strings(fdt));
	fdt_set_off_dt_strings(fdt, newstroffset);

	/* Walk the structure, correcting string offsets */
	offset = 0;
	যেতিয়ালৈ ((tag = fdt_next_tag(fdt, offset, &nextoffset)) != FDT_END) {
		যদি (tag == FDT_PROP) {
			সংরচনা fdt_property *prop =
				fdt_offset_ptr_w_(fdt, offset);
			পূর্ণাঙ্ক nameoff;

			nameoff = fdt32_to_cpu(prop->nameoff);
			nameoff += fdt_size_dt_strings(fdt);
			prop->nameoff = cpu_to_fdt32(nameoff);
		}
		offset = nextoffset;
	}
	যদি (nextoffset < 0)
		ঘূৰাই nextoffset;

	/* Finally, adjust the header */
	fdt_set_totalsize(fdt, newstroffset + fdt_size_dt_strings(fdt));

	/* And fix up fields that were keeping intermediate state. */
	fdt_set_last_comp_version(fdt, FDT_LAST_COMPATIBLE_VERSION);
	fdt_set_magic(fdt, FDT_MAGIC);

	ঘূৰাই 0;
}
