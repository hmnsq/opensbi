// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#شامل "libfdt_env.h"

#شامل <fdt.h>
#شامل <libfdt.h>

#شامل "libfdt_internal.h"

ایستا صحیح fdt_sw_probe_(تهی *fdt)
{
	اگر (!can_assume(VALID_درون_داد)) {
		اگر (fdt_magic(fdt) == FDT_MAGIC)
			بازگشت -FDT_ERR_BADSTATE;
		وگرنه اگر (fdt_magic(fdt) != FDT_SW_MAGIC)
			بازگشت -FDT_ERR_BADMAGIC;
	}

	بازگشت 0;
}

#define FDT_SW_PROBE(fdt) \
	{ \
		صحیح err; \
		اگر ((err = fdt_sw_probe_(fdt)) != 0) \
			بازگشت err; \
	}

/* 'memrsv' state:	Initial state after fdt_create()
 *
 * Allowed functions:
 *	fdt_add_reservemap_entry()
 *	fdt_finish_reservemap()		[moves to 'struct' state]
 */
ایستا صحیح fdt_sw_probe_memrsv_(تهی *fdt)
{
	صحیح err = fdt_sw_probe_(fdt);
	اگر (err)
		بازگشت err;

	اگر (!can_assume(VALID_درون_داد) && fdt_off_dt_strings(fdt) != 0)
		بازگشت -FDT_ERR_BADSTATE;
	بازگشت 0;
}

#define FDT_SW_PROBE_MEMRSV(fdt) \
	{ \
		صحیح err; \
		اگر ((err = fdt_sw_probe_memrsv_(fdt)) != 0) \
			بازگشت err; \
	}

/* 'struct' state:	Enter this state after fdt_finish_reservemap()
 *
 * Allowed functions:
 *	fdt_begin_node()
 *	fdt_end_node()
 *	fdt_property*()
 *	fdt_finish()			[moves to 'complete' state]
 */
ایستا صحیح fdt_sw_probe_struct_(تهی *fdt)
{
	صحیح err = fdt_sw_probe_(fdt);
	اگر (err)
		بازگشت err;

	اگر (!can_assume(VALID_درون_داد) &&
	    fdt_off_dt_strings(fdt) != fdt_totalsize(fdt))
		بازگشت -FDT_ERR_BADSTATE;
	بازگشت 0;
}

#define FDT_SW_PROBE_STRUCT(fdt) \
	{ \
		صحیح err; \
		اگر ((err = fdt_sw_probe_struct_(fdt)) != 0) \
			بازگشت err; \
	}

ایستا درونخطی uint32_t sw_flags(تهی *fdt)
{
	/* assert: (fdt_magic(fdt) == FDT_SW_MAGIC) */
	بازگشت fdt_last_comp_version(fdt);
}

/* 'complete' state:	Enter this state after fdt_finish()
 *
 * Allowed functions: none
 */

ایستا تهی *fdt_grab_space_(تهی *fdt, size_t len)
{
	unsigned صحیح offset = fdt_size_dt_struct(fdt);
	unsigned صحیح spaceleft;

	spaceleft = fdt_totalsize(fdt) - fdt_off_dt_struct(fdt)
		- fdt_size_dt_strings(fdt);

	اگر ((offset + len < offset) || (offset + len > spaceleft))
		بازگشت پوچ;

	fdt_set_size_dt_struct(fdt, offset + len);
	بازگشت fdt_offset_ptr_w_(fdt, offset);
}

صحیح fdt_create_with_flags(تهی *buf, صحیح bufsize, uint32_t flags)
{
	ثابت صحیح hdrsize = FDT_ALIGN(اندازه(ساختار fdt_header),
				      اندازه(ساختار fdt_reserve_entry));
	تهی *fdt = buf;

	اگر (bufsize < hdrsize)
		بازگشت -FDT_ERR_NOSPACE;

	اگر (flags & ~FDT_CREATE_FLAGS_ALL)
		بازگشت -FDT_ERR_BADFLAGS;

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

	بازگشت 0;
}

صحیح fdt_create(تهی *buf, صحیح bufsize)
{
	بازگشت fdt_create_with_flags(buf, bufsize, 0);
}

صحیح fdt_resize(تهی *fdt, تهی *buf, صحیح bufsize)
{
	size_t headsize, tailsize;
	نویسه *oldtail, *newtail;

	FDT_SW_PROBE(fdt);

	اگر (bufsize < 0)
		بازگشت -FDT_ERR_NOSPACE;

	headsize = fdt_off_dt_struct(fdt) + fdt_size_dt_struct(fdt);
	tailsize = fdt_size_dt_strings(fdt);

	اگر (!can_assume(VALID_DTB) &&
	    headsize + tailsize > fdt_totalsize(fdt))
		بازگشت -FDT_ERR_INTERNAL;

	اگر ((headsize + tailsize) > (unsigned)bufsize)
		بازگشت -FDT_ERR_NOSPACE;

	oldtail = (نویسه *)fdt + fdt_totalsize(fdt) - tailsize;
	newtail = (نویسه *)buf + bufsize - tailsize;

	/* Two cases to avoid clobbering data if the old and new
	 * buffers partially overlap */
	اگر (buf <= fdt) {
		memmove(buf, fdt, headsize);
		memmove(newtail, oldtail, tailsize);
	} وگرنه {
		memmove(newtail, oldtail, tailsize);
		memmove(buf, fdt, headsize);
	}

	fdt_set_totalsize(buf, bufsize);
	اگر (fdt_off_dt_strings(buf))
		fdt_set_off_dt_strings(buf, bufsize);

	بازگشت 0;
}

صحیح fdt_add_reservemap_entry(تهی *fdt, uint64_t addr, uint64_t size)
{
	ساختار fdt_reserve_entry *re;
	صحیح offset;

	FDT_SW_PROBE_MEMRSV(fdt);

	offset = fdt_off_dt_struct(fdt);
	اگر ((offset + اندازه(*re)) > fdt_totalsize(fdt))
		بازگشت -FDT_ERR_NOSPACE;

	re = (ساختار fdt_reserve_entry *)((نویسه *)fdt + offset);
	re->address = cpu_to_fdt64(addr);
	re->size = cpu_to_fdt64(size);

	fdt_set_off_dt_struct(fdt, offset + اندازه(*re));

	بازگشت 0;
}

صحیح fdt_finish_reservemap(تهی *fdt)
{
	صحیح err = fdt_add_reservemap_entry(fdt, 0, 0);

	اگر (err)
		بازگشت err;

	fdt_set_off_dt_strings(fdt, fdt_totalsize(fdt));
	بازگشت 0;
}

صحیح fdt_begin_گره(تهی *fdt, ثابت نویسه *name)
{
	ساختار fdt_گره_header *nh;
	صحیح namelen;

	FDT_SW_PROBE_STRUCT(fdt);

	namelen = strlen(name) + 1;
	nh = fdt_grab_space_(fdt, اندازه(*nh) + FDT_TAGALIGN(namelen));
	اگر (! nh)
		بازگشت -FDT_ERR_NOSPACE;

	nh->tag = cpu_to_fdt32(FDT_BEGIN_گره);
	memcpy(nh->name, name, namelen);
	بازگشت 0;
}

صحیح fdt_end_گره(تهی *fdt)
{
	fdt32_t *en;

	FDT_SW_PROBE_STRUCT(fdt);

	en = fdt_grab_space_(fdt, FDT_TAGSIZE);
	اگر (! en)
		بازگشت -FDT_ERR_NOSPACE;

	*en = cpu_to_fdt32(FDT_END_گره);
	بازگشت 0;
}

ایستا صحیح fdt_add_string_(تهی *fdt, ثابت نویسه *s)
{
	نویسه *strtab = (نویسه *)fdt + fdt_totalsize(fdt);
	unsigned صحیح strtabsize = fdt_size_dt_strings(fdt);
	unsigned صحیح len = strlen(s) + 1;
	unsigned صحیح struct_بالاترین, offset;

	offset = strtabsize + len;
	struct_بالاترین = fdt_off_dt_struct(fdt) + fdt_size_dt_struct(fdt);
	اگر (fdt_totalsize(fdt) - offset < struct_بالاترین)
		بازگشت 0; /* no more room :( */

	memcpy(strtab - offset, s, len);
	fdt_set_size_dt_strings(fdt, strtabsize + len);
	بازگشت -offset;
}

/* Must only be used to roll back in case of error */
ایستا تهی fdt_del_last_string_(تهی *fdt, ثابت نویسه *s)
{
	صحیح strtabsize = fdt_size_dt_strings(fdt);
	صحیح len = strlen(s) + 1;

	fdt_set_size_dt_strings(fdt, strtabsize - len);
}

ایستا صحیح fdt_find_add_string_(تهی *fdt, ثابت نویسه *s, صحیح *allocated)
{
	نویسه *strtab = (نویسه *)fdt + fdt_totalsize(fdt);
	صحیح strtabsize = fdt_size_dt_strings(fdt);
	ثابت نویسه *p;

	*allocated = 0;

	p = fdt_find_string_(strtab - strtabsize, strtabsize, s);
	اگر (p)
		بازگشت p - strtab;

	*allocated = 1;

	بازگشت fdt_add_string_(fdt, s);
}

صحیح fdt_property_placeholder(تهی *fdt, ثابت نویسه *name, صحیح len, تهی **valp)
{
	ساختار fdt_property *prop;
	صحیح nameoff;
	صحیح allocated;

	FDT_SW_PROBE_STRUCT(fdt);

	/* String de-duplication can be slow, _NO_NAME_DEDUP skips it */
	اگر (sw_flags(fdt) & FDT_CREATE_FLAG_NO_NAME_DEDUP) {
		allocated = 1;
		nameoff = fdt_add_string_(fdt, name);
	} وگرنه {
		nameoff = fdt_find_add_string_(fdt, name, &allocated);
	}
	اگر (nameoff == 0)
		بازگشت -FDT_ERR_NOSPACE;

	prop = fdt_grab_space_(fdt, اندازه(*prop) + FDT_TAGALIGN(len));
	اگر (! prop) {
		اگر (allocated)
			fdt_del_last_string_(fdt, name);
		بازگشت -FDT_ERR_NOSPACE;
	}

	prop->tag = cpu_to_fdt32(FDT_PROP);
	prop->nameoff = cpu_to_fdt32(nameoff);
	prop->len = cpu_to_fdt32(len);
	*valp = prop->data;
	بازگشت 0;
}

صحیح fdt_property(تهی *fdt, ثابت نویسه *name, ثابت تهی *val, صحیح len)
{
	تهی *ptr;
	صحیح ret;

	ret = fdt_property_placeholder(fdt, name, len, &ptr);
	اگر (ret)
		بازگشت ret;
	memcpy(ptr, val, len);
	بازگشت 0;
}

صحیح fdt_finish(تهی *fdt)
{
	نویسه *p = (نویسه *)fdt;
	fdt32_t *end;
	صحیح oldstroffset, newstroffset;
	uint32_t tag;
	صحیح offset, nextoffset;

	FDT_SW_PROBE_STRUCT(fdt);

	/* Add terminator */
	end = fdt_grab_space_(fdt, اندازه(*end));
	اگر (! end)
		بازگشت -FDT_ERR_NOSPACE;
	*end = cpu_to_fdt32(FDT_END);

	/* Relocate the string table */
	oldstroffset = fdt_totalsize(fdt) - fdt_size_dt_strings(fdt);
	newstroffset = fdt_off_dt_struct(fdt) + fdt_size_dt_struct(fdt);
	memmove(p + newstroffset, p + oldstroffset, fdt_size_dt_strings(fdt));
	fdt_set_off_dt_strings(fdt, newstroffset);

	/* Walk the structure, correcting string offsets */
	offset = 0;
	وقتی ((tag = fdt_next_tag(fdt, offset, &nextoffset)) != FDT_END) {
		اگر (tag == FDT_PROP) {
			ساختار fdt_property *prop =
				fdt_offset_ptr_w_(fdt, offset);
			صحیح nameoff;

			nameoff = fdt32_to_cpu(prop->nameoff);
			nameoff += fdt_size_dt_strings(fdt);
			prop->nameoff = cpu_to_fdt32(nameoff);
		}
		offset = nextoffset;
	}
	اگر (nextoffset < 0)
		بازگشت nextoffset;

	/* Finally, adjust the header */
	fdt_set_totalsize(fdt, newstroffset + fdt_size_dt_strings(fdt));

	/* And fix up fields that were keeping intermediate state. */
	fdt_set_last_comp_version(fdt, FDT_LAST_COMPATIBLE_VERSION);
	fdt_set_magic(fdt, FDT_MAGIC);

	بازگشت 0;
}
