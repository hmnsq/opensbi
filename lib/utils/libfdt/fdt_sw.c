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
	إذا (!can_assume(VALID_درون_داد)) {
		إذا (fdt_magic(fdt) == FDT_MAGIC)
			أرجع -FDT_ERR_BADSTATE;
		وإلا إذا (fdt_magic(fdt) != FDT_SW_MAGIC)
			أرجع -FDT_ERR_BADMAGIC;
	}

	أرجع 0;
}

#تعریف_کن FDT_SW_PROBE(fdt) \
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

	إذا (!can_assume(VALID_درون_داد) && fdt_off_dt_رشتهها(fdt) != 0)
		أرجع -FDT_ERR_BADSTATE;
	أرجع 0;
}

#تعریف_کن FDT_SW_PROBE_MEMRSV(fdt) \
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

	إذا (!can_assume(VALID_درون_داد) &&
	    fdt_off_dt_رشتهها(fdt) != fdt_totalsize(fdt))
		أرجع -FDT_ERR_BADSTATE;
	أرجع 0;
}

#تعریف_کن FDT_SW_PROBE_STRUCT(fdt) \
	{ \
		صحيح err; \
		إذا ((err = fdt_sw_probe_struct_(fdt)) != 0) \
			أرجع err; \
	}

ساكن مضمن uint32_t sw_flags(فراغ *fdt)
{
	/* assert: (fdt_magic(fdt) == FDT_SW_MAGIC) */
	أرجع fdt_آخرین_comp_نسخه(fdt);
}

/* 'complete' state:	Enter this state after fdt_finish()
 *
 * Allowed functions: none
 */

ساكن فراغ *fdt_grab_space_(فراغ *fdt, size_t len)
{
	بیعلامت صحيح جابه_جایی = fdt_اندازه_dt_struct(fdt);
	بیعلامت صحيح spaceleft;

	spaceleft = fdt_totalsize(fdt) - fdt_off_dt_struct(fdt)
		- fdt_اندازه_dt_رشتهها(fdt);

	إذا ((جابه_جایی + len < جابه_جایی) || (جابه_جایی + len > spaceleft))
		أرجع عدم;

	fdt_مجموعه_اندازه_dt_struct(fdt, جابه_جایی + len);
	أرجع fdt_جابه_جایی_ptr_w(fdt, جابه_جایی);
}

صحيح fdt_create_with_flags(فراغ *buf, صحيح bufsize, uint32_t flags)
{
	ثابت صحيح hdrsize = FDT_ALIGN(حجم(بنية fdt_سرآیند),
				      حجم(بنية fdt_reserve_مدخل));
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
	fdt_مجموعه_magic(fdt, FDT_SW_MAGIC);
	fdt_مجموعه_نسخه(fdt, FDT_آخرین_SUPPORTED_نسخه);
	fdt_مجموعه_آخرین_comp_نسخه(fdt, flags);

	fdt_مجموعه_totalsize(fdt,  bufsize);

	fdt_مجموعه_off_mem_rsvmap(fdt, hdrsize);
	fdt_مجموعه_off_dt_struct(fdt, fdt_off_mem_rsvmap(fdt));
	fdt_مجموعه_off_dt_رشتهها(fdt, 0);

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

	headsize = fdt_off_dt_struct(fdt) + fdt_اندازه_dt_struct(fdt);
	tailsize = fdt_اندازه_dt_رشتهها(fdt);

	إذا (!can_assume(VALID_DTB) &&
	    headsize + tailsize > fdt_totalsize(fdt))
		أرجع -FDT_ERR_داخلی;

	إذا ((headsize + tailsize) > (بیعلامت)bufsize)
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

	fdt_مجموعه_totalsize(buf, bufsize);
	إذا (fdt_off_dt_رشتهها(buf))
		fdt_مجموعه_off_dt_رشتهها(buf, bufsize);

	أرجع 0;
}

صحيح fdt_جمع_کن_reservemap_مدخل(فراغ *fdt, uint64_t addr, uint64_t اندازه)
{
	بنية fdt_reserve_مدخل *re;
	صحيح جابه_جایی;

	FDT_SW_PROBE_MEMRSV(fdt);

	جابه_جایی = fdt_off_dt_struct(fdt);
	إذا ((جابه_جایی + حجم(*re)) > fdt_totalsize(fdt))
		أرجع -FDT_ERR_NOSPACE;

	re = (بنية fdt_reserve_مدخل *)((حرف *)fdt + جابه_جایی);
	re->آدرس = cpu_مقصد_fdt64(addr);
	re->اندازه = cpu_مقصد_fdt64(اندازه);

	fdt_مجموعه_off_dt_struct(fdt, جابه_جایی + حجم(*re));

	أرجع 0;
}

صحيح fdt_پایان_reservemap(فراغ *fdt)
{
	صحيح err = fdt_جمع_کن_reservemap_مدخل(fdt, 0, 0);

	إذا (err)
		أرجع err;

	fdt_مجموعه_off_dt_رشتهها(fdt, fdt_totalsize(fdt));
	أرجع 0;
}

صحيح fdt_begin_گره(فراغ *fdt, ثابت حرف *name)
{
	بنية fdt_گره_سرآیند *nh;
	صحيح namelen;

	FDT_SW_PROBE_STRUCT(fdt);

	namelen = طول_رشته(name) + 1;
	nh = fdt_grab_space_(fdt, حجم(*nh) + FDT_TAGALIGN(namelen));
	إذا (! nh)
		أرجع -FDT_ERR_NOSPACE;

	nh->برچسب = cpu_مقصد_fdt32(FDT_BEGIN_گره);
	کپی_حافظه(nh->name, name, namelen);
	أرجع 0;
}

صحيح fdt_انتها_گره(فراغ *fdt)
{
	fdt32_t *en;

	FDT_SW_PROBE_STRUCT(fdt);

	en = fdt_grab_space_(fdt, FDT_TAGSIZE);
	إذا (! en)
		أرجع -FDT_ERR_NOSPACE;

	*en = cpu_مقصد_fdt32(FDT_انتها_گره);
	أرجع 0;
}

ساكن صحيح fdt_جمع_کن_string(فراغ *fdt, ثابت حرف *s)
{
	حرف *strtab = (حرف *)fdt + fdt_totalsize(fdt);
	بیعلامت صحيح strtabsize = fdt_اندازه_dt_رشتهها(fdt);
	بیعلامت صحيح len = طول_رشته(s) + 1;
	بیعلامت صحيح struct_بالاترین, جابه_جایی;

	جابه_جایی = strtabsize + len;
	struct_بالاترین = fdt_off_dt_struct(fdt) + fdt_اندازه_dt_struct(fdt);
	إذا (fdt_totalsize(fdt) - جابه_جایی < struct_بالاترین)
		أرجع 0; /* no more room :( */

	کپی_حافظه(strtab - جابه_جایی, s, len);
	fdt_مجموعه_اندازه_dt_رشتهها(fdt, strtabsize + len);
	أرجع -جابه_جایی;
}

/* Must only be used to roll back in case of error */
ساكن فراغ fdt_del_آخرین_string(فراغ *fdt, ثابت حرف *s)
{
	صحيح strtabsize = fdt_اندازه_dt_رشتهها(fdt);
	صحيح len = طول_رشته(s) + 1;

	fdt_مجموعه_اندازه_dt_رشتهها(fdt, strtabsize - len);
}

ساكن صحيح fdt_پیدا_کن_جمع_کن_string(فراغ *fdt, ثابت حرف *s, صحيح *allocated)
{
	حرف *strtab = (حرف *)fdt + fdt_totalsize(fdt);
	صحيح strtabsize = fdt_اندازه_dt_رشتهها(fdt);
	ثابت حرف *p;

	*allocated = 0;

	p = fdt_پیدا_کن_string(strtab - strtabsize, strtabsize, s);
	إذا (p)
		أرجع p - strtab;

	*allocated = 1;

	أرجع fdt_جمع_کن_string(fdt, s);
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
		nameoff = fdt_جمع_کن_string(fdt, name);
	} وإلا {
		nameoff = fdt_پیدا_کن_جمع_کن_string(fdt, name, &allocated);
	}
	إذا (nameoff == 0)
		أرجع -FDT_ERR_NOSPACE;

	prop = fdt_grab_space_(fdt, حجم(*prop) + FDT_TAGALIGN(len));
	إذا (! prop) {
		إذا (allocated)
			fdt_del_آخرین_string(fdt, name);
		أرجع -FDT_ERR_NOSPACE;
	}

	prop->برچسب = cpu_مقصد_fdt32(FDT_PROP);
	prop->nameoff = cpu_مقصد_fdt32(nameoff);
	prop->len = cpu_مقصد_fdt32(len);
	*valp = prop->داده;
	أرجع 0;
}

صحيح fdt_property(فراغ *fdt, ثابت حرف *name, ثابت فراغ *val, صحيح len)
{
	فراغ *ptr;
	صحيح ret;

	ret = fdt_property_placeholder(fdt, name, len, &ptr);
	إذا (ret)
		أرجع ret;
	کپی_حافظه(ptr, val, len);
	أرجع 0;
}

صحيح fdt_پایان(فراغ *fdt)
{
	حرف *p = (حرف *)fdt;
	fdt32_t *انتها;
	صحيح oldstroffset, newstroffset;
	uint32_t برچسب;
	صحيح جابه_جایی, nextoffset;

	FDT_SW_PROBE_STRUCT(fdt);

	/* Add terminator */
	انتها = fdt_grab_space_(fdt, حجم(*انتها));
	إذا (! انتها)
		أرجع -FDT_ERR_NOSPACE;
	*انتها = cpu_مقصد_fdt32(FDT_انتها);

	/* Relocate the string table */
	oldstroffset = fdt_totalsize(fdt) - fdt_اندازه_dt_رشتهها(fdt);
	newstroffset = fdt_off_dt_struct(fdt) + fdt_اندازه_dt_struct(fdt);
	memmove(p + newstroffset, p + oldstroffset, fdt_اندازه_dt_رشتهها(fdt));
	fdt_مجموعه_off_dt_رشتهها(fdt, newstroffset);

	/* Walk the structure, correcting string offsets */
	جابه_جایی = 0;
	طالما ((برچسب = fdt_بعدی_برچسب(fdt, جابه_جایی, &nextoffset)) != FDT_انتها) {
		إذا (برچسب == FDT_PROP) {
			بنية fdt_property *prop =
				fdt_جابه_جایی_ptr_w(fdt, جابه_جایی);
			صحيح nameoff;

			nameoff = fdt32_مقصد_cpu(prop->nameoff);
			nameoff += fdt_اندازه_dt_رشتهها(fdt);
			prop->nameoff = cpu_مقصد_fdt32(nameoff);
		}
		جابه_جایی = nextoffset;
	}
	إذا (nextoffset < 0)
		أرجع nextoffset;

	/* Finally, adjust the header */
	fdt_مجموعه_totalsize(fdt, newstroffset + fdt_اندازه_dt_رشتهها(fdt));

	/* And fix up fields that were keeping intermediate state. */
	fdt_مجموعه_آخرین_comp_نسخه(fdt, FDT_آخرین_COMPATIBLE_نسخه);
	fdt_مجموعه_magic(fdt, FDT_MAGIC);

	أرجع 0;
}
