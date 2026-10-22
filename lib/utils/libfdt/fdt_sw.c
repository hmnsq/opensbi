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
	إذا (!can_assume(VALID_داخلا)) {
		إذا (fdt_magic(fdt) == FDT_MAGIC)
			أرجع -FDT_ERR_BADSTATE;
		وإلا إذا (fdt_magic(fdt) != FDT_SW_MAGIC)
			أرجع -FDT_ERR_BADMAGIC;
	}

	أرجع 0;
}

#وصف_ڪريو FDT_SW_PROBE(fdt) \
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

	إذا (!can_assume(VALID_داخلا) && fdt_off_dt_لڙيون(fdt) != 0)
		أرجع -FDT_ERR_BADSTATE;
	أرجع 0;
}

#وصف_ڪريو FDT_SW_PROBE_MEMRSV(fdt) \
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

	إذا (!can_assume(VALID_داخلا) &&
	    fdt_off_dt_لڙيون(fdt) != fdt_totalsize(fdt))
		أرجع -FDT_ERR_BADSTATE;
	أرجع 0;
}

#وصف_ڪريو FDT_SW_PROBE_STRUCT(fdt) \
	{ \
		صحيح err; \
		إذا ((err = fdt_sw_probe_struct_(fdt)) != 0) \
			أرجع err; \
	}

ساكن مضمن uint32_t sw_flags(فراغ *fdt)
{
	/* assert: (fdt_magic(fdt) == FDT_SW_MAGIC) */
	أرجع fdt_آخري_comp_نسخو(fdt);
}

/* 'complete' state:	Enter this state after fdt_finish()
 *
 * Allowed functions: none
 */

ساكن فراغ *fdt_grab_space_(فراغ *fdt, size_t len)
{
	بنانشان صحيح وٿي = fdt_سائيز_dt_struct(fdt);
	بنانشان صحيح spaceleft;

	spaceleft = fdt_totalsize(fdt) - fdt_off_dt_struct(fdt)
		- fdt_سائيز_dt_لڙيون(fdt);

	إذا ((وٿي + len < وٿي) || (وٿي + len > spaceleft))
		أرجع عدم;

	fdt_سيٽ_سائيز_dt_struct(fdt, وٿي + len);
	أرجع fdt_وٿي_ptr_w(fdt, وٿي);
}

صحيح fdt_create_with_flags(فراغ *buf, صحيح bufsize, uint32_t flags)
{
	ثابت صحيح hdrsize = FDT_ALIGN(حجم(بنية fdt_سرو),
				      حجم(بنية fdt_reserve_داخلا));
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
	fdt_سيٽ_magic(fdt, FDT_SW_MAGIC);
	fdt_سيٽ_نسخو(fdt, FDT_آخري_SUPPORTED_نسخو);
	fdt_سيٽ_آخري_comp_نسخو(fdt, flags);

	fdt_سيٽ_totalsize(fdt,  bufsize);

	fdt_سيٽ_off_mem_rsvmap(fdt, hdrsize);
	fdt_سيٽ_off_dt_struct(fdt, fdt_off_mem_rsvmap(fdt));
	fdt_سيٽ_off_dt_لڙيون(fdt, 0);

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

	headsize = fdt_off_dt_struct(fdt) + fdt_سائيز_dt_struct(fdt);
	tailsize = fdt_سائيز_dt_لڙيون(fdt);

	إذا (!can_assume(VALID_DTB) &&
	    headsize + tailsize > fdt_totalsize(fdt))
		أرجع -FDT_ERR_اندروني;

	إذا ((headsize + tailsize) > (بنانشان)bufsize)
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

	fdt_سيٽ_totalsize(buf, bufsize);
	إذا (fdt_off_dt_لڙيون(buf))
		fdt_سيٽ_off_dt_لڙيون(buf, bufsize);

	أرجع 0;
}

صحيح fdt_جوڙيو_reservemap_داخلا(فراغ *fdt, uint64_t addr, uint64_t سائيز)
{
	بنية fdt_reserve_داخلا *re;
	صحيح وٿي;

	FDT_SW_PROBE_MEMRSV(fdt);

	وٿي = fdt_off_dt_struct(fdt);
	إذا ((وٿي + حجم(*re)) > fdt_totalsize(fdt))
		أرجع -FDT_ERR_NOSPACE;

	re = (بنية fdt_reserve_داخلا *)((حرف *)fdt + وٿي);
	re->پتو = cpu_منزل_fdt64(addr);
	re->سائيز = cpu_منزل_fdt64(سائيز);

	fdt_سيٽ_off_dt_struct(fdt, وٿي + حجم(*re));

	أرجع 0;
}

صحيح fdt_پڄاڻي_reservemap(فراغ *fdt)
{
	صحيح err = fdt_جوڙيو_reservemap_داخلا(fdt, 0, 0);

	إذا (err)
		أرجع err;

	fdt_سيٽ_off_dt_لڙيون(fdt, fdt_totalsize(fdt));
	أرجع 0;
}

صحيح fdt_begin_ڳنڍ(فراغ *fdt, ثابت حرف *name)
{
	بنية fdt_ڳنڍ_سرو *nh;
	صحيح namelen;

	FDT_SW_PROBE_STRUCT(fdt);

	namelen = لڙي_ڊيگهه(name) + 1;
	nh = fdt_grab_space_(fdt, حجم(*nh) + FDT_TAGALIGN(namelen));
	إذا (! nh)
		أرجع -FDT_ERR_NOSPACE;

	nh->ٽيگ = cpu_منزل_fdt32(FDT_BEGIN_ڳنڍ);
	يادگيري_نقل(nh->name, name, namelen);
	أرجع 0;
}

صحيح fdt_پڇاڙي_ڳنڍ(فراغ *fdt)
{
	fdt32_t *en;

	FDT_SW_PROBE_STRUCT(fdt);

	en = fdt_grab_space_(fdt, FDT_TAGSIZE);
	إذا (! en)
		أرجع -FDT_ERR_NOSPACE;

	*en = cpu_منزل_fdt32(FDT_پڇاڙي_ڳنڍ);
	أرجع 0;
}

ساكن صحيح fdt_جوڙيو_string(فراغ *fdt, ثابت حرف *s)
{
	حرف *strtab = (حرف *)fdt + fdt_totalsize(fdt);
	بنانشان صحيح strtabsize = fdt_سائيز_dt_لڙيون(fdt);
	بنانشان صحيح len = لڙي_ڊيگهه(s) + 1;
	بنانشان صحيح struct_چوٽي, وٿي;

	وٿي = strtabsize + len;
	struct_چوٽي = fdt_off_dt_struct(fdt) + fdt_سائيز_dt_struct(fdt);
	إذا (fdt_totalsize(fdt) - وٿي < struct_چوٽي)
		أرجع 0; /* no more room :( */

	يادگيري_نقل(strtab - وٿي, s, len);
	fdt_سيٽ_سائيز_dt_لڙيون(fdt, strtabsize + len);
	أرجع -وٿي;
}

/* Must only be used to roll back in case of error */
ساكن فراغ fdt_del_آخري_string(فراغ *fdt, ثابت حرف *s)
{
	صحيح strtabsize = fdt_سائيز_dt_لڙيون(fdt);
	صحيح len = لڙي_ڊيگهه(s) + 1;

	fdt_سيٽ_سائيز_dt_لڙيون(fdt, strtabsize - len);
}

ساكن صحيح fdt_ڳوليو_جوڙيو_string(فراغ *fdt, ثابت حرف *s, صحيح *allocated)
{
	حرف *strtab = (حرف *)fdt + fdt_totalsize(fdt);
	صحيح strtabsize = fdt_سائيز_dt_لڙيون(fdt);
	ثابت حرف *p;

	*allocated = 0;

	p = fdt_ڳوليو_string(strtab - strtabsize, strtabsize, s);
	إذا (p)
		أرجع p - strtab;

	*allocated = 1;

	أرجع fdt_جوڙيو_string(fdt, s);
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
		nameoff = fdt_جوڙيو_string(fdt, name);
	} وإلا {
		nameoff = fdt_ڳوليو_جوڙيو_string(fdt, name, &allocated);
	}
	إذا (nameoff == 0)
		أرجع -FDT_ERR_NOSPACE;

	prop = fdt_grab_space_(fdt, حجم(*prop) + FDT_TAGALIGN(len));
	إذا (! prop) {
		إذا (allocated)
			fdt_del_آخري_string(fdt, name);
		أرجع -FDT_ERR_NOSPACE;
	}

	prop->ٽيگ = cpu_منزل_fdt32(FDT_PROP);
	prop->nameoff = cpu_منزل_fdt32(nameoff);
	prop->len = cpu_منزل_fdt32(len);
	*valp = prop->معلومات;
	أرجع 0;
}

صحيح fdt_property(فراغ *fdt, ثابت حرف *name, ثابت فراغ *val, صحيح len)
{
	فراغ *ptr;
	صحيح ret;

	ret = fdt_property_placeholder(fdt, name, len, &ptr);
	إذا (ret)
		أرجع ret;
	يادگيري_نقل(ptr, val, len);
	أرجع 0;
}

صحيح fdt_پڄاڻي(فراغ *fdt)
{
	حرف *p = (حرف *)fdt;
	fdt32_t *پڇاڙي;
	صحيح oldstroffset, newstroffset;
	uint32_t ٽيگ;
	صحيح وٿي, nextoffset;

	FDT_SW_PROBE_STRUCT(fdt);

	/* Add terminator */
	پڇاڙي = fdt_grab_space_(fdt, حجم(*پڇاڙي));
	إذا (! پڇاڙي)
		أرجع -FDT_ERR_NOSPACE;
	*پڇاڙي = cpu_منزل_fdt32(FDT_پڇاڙي);

	/* Relocate the string table */
	oldstroffset = fdt_totalsize(fdt) - fdt_سائيز_dt_لڙيون(fdt);
	newstroffset = fdt_off_dt_struct(fdt) + fdt_سائيز_dt_struct(fdt);
	memmove(p + newstroffset, p + oldstroffset, fdt_سائيز_dt_لڙيون(fdt));
	fdt_سيٽ_off_dt_لڙيون(fdt, newstroffset);

	/* Walk the structure, correcting string offsets */
	وٿي = 0;
	طالما ((ٽيگ = fdt_ايندڙ_ٽيگ(fdt, وٿي, &nextoffset)) != FDT_پڇاڙي) {
		إذا (ٽيگ == FDT_PROP) {
			بنية fdt_property *prop =
				fdt_وٿي_ptr_w(fdt, وٿي);
			صحيح nameoff;

			nameoff = fdt32_منزل_cpu(prop->nameoff);
			nameoff += fdt_سائيز_dt_لڙيون(fdt);
			prop->nameoff = cpu_منزل_fdt32(nameoff);
		}
		وٿي = nextoffset;
	}
	إذا (nextoffset < 0)
		أرجع nextoffset;

	/* Finally, adjust the header */
	fdt_سيٽ_totalsize(fdt, newstroffset + fdt_سائيز_dt_لڙيون(fdt));

	/* And fix up fields that were keeping intermediate state. */
	fdt_سيٽ_آخري_comp_نسخو(fdt, FDT_آخري_COMPATIBLE_نسخو);
	fdt_سيٽ_magic(fdt, FDT_MAGIC);

	أرجع 0;
}
