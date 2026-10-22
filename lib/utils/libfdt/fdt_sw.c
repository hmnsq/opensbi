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
	إذا (!can_assume(VALID_داخلہ)) {
		إذا (fdt_magic(fdt) == FDT_MAGIC)
			أرجع -FDT_ERR_BADSTATE;
		وإلا إذا (fdt_magic(fdt) != FDT_SW_MAGIC)
			أرجع -FDT_ERR_BADMAGIC;
	}

	أرجع 0;
}

#تعریف_کرو FDT_SW_PROBE(fdt) \
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

	إذا (!can_assume(VALID_داخلہ) && fdt_off_dt_لڑیاں(fdt) != 0)
		أرجع -FDT_ERR_BADSTATE;
	أرجع 0;
}

#تعریف_کرو FDT_SW_PROBE_MEMRSV(fdt) \
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

	إذا (!can_assume(VALID_داخلہ) &&
	    fdt_off_dt_لڑیاں(fdt) != fdt_totalsize(fdt))
		أرجع -FDT_ERR_BADSTATE;
	أرجع 0;
}

#تعریف_کرو FDT_SW_PROBE_STRUCT(fdt) \
	{ \
		صحيح err; \
		إذا ((err = fdt_sw_probe_struct_(fdt)) != 0) \
			أرجع err; \
	}

ساكن مضمن uint32_t sw_flags(فراغ *fdt)
{
	/* assert: (fdt_magic(fdt) == FDT_SW_MAGIC) */
	أرجع fdt_آخری_comp_نسخہ(fdt);
}

/* 'complete' state:	Enter this state after fdt_finish()
 *
 * Allowed functions: none
 */

ساكن فراغ *fdt_grab_space_(فراغ *fdt, size_t len)
{
	بنانشانی صحيح وتھ = fdt_ناپ_dt_struct(fdt);
	بنانشانی صحيح spaceleft;

	spaceleft = fdt_totalsize(fdt) - fdt_off_dt_struct(fdt)
		- fdt_ناپ_dt_لڑیاں(fdt);

	إذا ((وتھ + len < وتھ) || (وتھ + len > spaceleft))
		أرجع عدم;

	fdt_سیٹ_ناپ_dt_struct(fdt, وتھ + len);
	أرجع fdt_وتھ_ptr_w(fdt, وتھ);
}

صحيح fdt_create_with_flags(فراغ *buf, صحيح bufsize, uint32_t flags)
{
	ثابت صحيح hdrsize = FDT_ALIGN(حجم(بنية fdt_سرخی),
				      حجم(بنية fdt_reserve_اندراج));
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
	fdt_سیٹ_magic(fdt, FDT_SW_MAGIC);
	fdt_سیٹ_نسخہ(fdt, FDT_آخری_SUPPORTED_نسخہ);
	fdt_سیٹ_آخری_comp_نسخہ(fdt, flags);

	fdt_سیٹ_totalsize(fdt,  bufsize);

	fdt_سیٹ_off_mem_rsvmap(fdt, hdrsize);
	fdt_سیٹ_off_dt_struct(fdt, fdt_off_mem_rsvmap(fdt));
	fdt_سیٹ_off_dt_لڑیاں(fdt, 0);

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

	headsize = fdt_off_dt_struct(fdt) + fdt_ناپ_dt_struct(fdt);
	tailsize = fdt_ناپ_dt_لڑیاں(fdt);

	إذا (!can_assume(VALID_DTB) &&
	    headsize + tailsize > fdt_totalsize(fdt))
		أرجع -FDT_ERR_اندرونی;

	إذا ((headsize + tailsize) > (بنانشانی)bufsize)
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

	fdt_سیٹ_totalsize(buf, bufsize);
	إذا (fdt_off_dt_لڑیاں(buf))
		fdt_سیٹ_off_dt_لڑیاں(buf, bufsize);

	أرجع 0;
}

صحيح fdt_جوڑو_reservemap_اندراج(فراغ *fdt, uint64_t addr, uint64_t ناپ)
{
	بنية fdt_reserve_اندراج *re;
	صحيح وتھ;

	FDT_SW_PROBE_MEMRSV(fdt);

	وتھ = fdt_off_dt_struct(fdt);
	إذا ((وتھ + حجم(*re)) > fdt_totalsize(fdt))
		أرجع -FDT_ERR_NOSPACE;

	re = (بنية fdt_reserve_اندراج *)((حرف *)fdt + وتھ);
	re->پتہ = cpu_منزل_fdt64(addr);
	re->ناپ = cpu_منزل_fdt64(ناپ);

	fdt_سیٹ_off_dt_struct(fdt, وتھ + حجم(*re));

	أرجع 0;
}

صحيح fdt_انت_reservemap(فراغ *fdt)
{
	صحيح err = fdt_جوڑو_reservemap_اندراج(fdt, 0, 0);

	إذا (err)
		أرجع err;

	fdt_سیٹ_off_dt_لڑیاں(fdt, fdt_totalsize(fdt));
	أرجع 0;
}

صحيح fdt_begin_گرہ(فراغ *fdt, ثابت حرف *name)
{
	بنية fdt_گرہ_سرخی *nh;
	صحيح namelen;

	FDT_SW_PROBE_STRUCT(fdt);

	namelen = لڑی_لمبائی(name) + 1;
	nh = fdt_grab_space_(fdt, حجم(*nh) + FDT_TAGALIGN(namelen));
	إذا (! nh)
		أرجع -FDT_ERR_NOSPACE;

	nh->ٹیگ = cpu_منزل_fdt32(FDT_BEGIN_گرہ);
	حافظہ_نقل(nh->name, name, namelen);
	أرجع 0;
}

صحيح fdt_سرا_گرہ(فراغ *fdt)
{
	fdt32_t *en;

	FDT_SW_PROBE_STRUCT(fdt);

	en = fdt_grab_space_(fdt, FDT_TAGSIZE);
	إذا (! en)
		أرجع -FDT_ERR_NOSPACE;

	*en = cpu_منزل_fdt32(FDT_سرا_گرہ);
	أرجع 0;
}

ساكن صحيح fdt_جوڑو_string(فراغ *fdt, ثابت حرف *s)
{
	حرف *strtab = (حرف *)fdt + fdt_totalsize(fdt);
	بنانشانی صحيح strtabsize = fdt_ناپ_dt_لڑیاں(fdt);
	بنانشانی صحيح len = لڑی_لمبائی(s) + 1;
	بنانشانی صحيح struct_چوٹی, وتھ;

	وتھ = strtabsize + len;
	struct_چوٹی = fdt_off_dt_struct(fdt) + fdt_ناپ_dt_struct(fdt);
	إذا (fdt_totalsize(fdt) - وتھ < struct_چوٹی)
		أرجع 0; /* no more room :( */

	حافظہ_نقل(strtab - وتھ, s, len);
	fdt_سیٹ_ناپ_dt_لڑیاں(fdt, strtabsize + len);
	أرجع -وتھ;
}

/* Must only be used to roll back in case of error */
ساكن فراغ fdt_del_آخری_string(فراغ *fdt, ثابت حرف *s)
{
	صحيح strtabsize = fdt_ناپ_dt_لڑیاں(fdt);
	صحيح len = لڑی_لمبائی(s) + 1;

	fdt_سیٹ_ناپ_dt_لڑیاں(fdt, strtabsize - len);
}

ساكن صحيح fdt_لبھو_جوڑو_string(فراغ *fdt, ثابت حرف *s, صحيح *allocated)
{
	حرف *strtab = (حرف *)fdt + fdt_totalsize(fdt);
	صحيح strtabsize = fdt_ناپ_dt_لڑیاں(fdt);
	ثابت حرف *p;

	*allocated = 0;

	p = fdt_لبھو_string(strtab - strtabsize, strtabsize, s);
	إذا (p)
		أرجع p - strtab;

	*allocated = 1;

	أرجع fdt_جوڑو_string(fdt, s);
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
		nameoff = fdt_جوڑو_string(fdt, name);
	} وإلا {
		nameoff = fdt_لبھو_جوڑو_string(fdt, name, &allocated);
	}
	إذا (nameoff == 0)
		أرجع -FDT_ERR_NOSPACE;

	prop = fdt_grab_space_(fdt, حجم(*prop) + FDT_TAGALIGN(len));
	إذا (! prop) {
		إذا (allocated)
			fdt_del_آخری_string(fdt, name);
		أرجع -FDT_ERR_NOSPACE;
	}

	prop->ٹیگ = cpu_منزل_fdt32(FDT_PROP);
	prop->nameoff = cpu_منزل_fdt32(nameoff);
	prop->len = cpu_منزل_fdt32(len);
	*valp = prop->انکڑے;
	أرجع 0;
}

صحيح fdt_property(فراغ *fdt, ثابت حرف *name, ثابت فراغ *val, صحيح len)
{
	فراغ *ptr;
	صحيح ret;

	ret = fdt_property_placeholder(fdt, name, len, &ptr);
	إذا (ret)
		أرجع ret;
	حافظہ_نقل(ptr, val, len);
	أرجع 0;
}

صحيح fdt_انت(فراغ *fdt)
{
	حرف *p = (حرف *)fdt;
	fdt32_t *سرا;
	صحيح oldstroffset, newstroffset;
	uint32_t ٹیگ;
	صحيح وتھ, nextoffset;

	FDT_SW_PROBE_STRUCT(fdt);

	/* Add terminator */
	سرا = fdt_grab_space_(fdt, حجم(*سرا));
	إذا (! سرا)
		أرجع -FDT_ERR_NOSPACE;
	*سرا = cpu_منزل_fdt32(FDT_سرا);

	/* Relocate the string table */
	oldstroffset = fdt_totalsize(fdt) - fdt_ناپ_dt_لڑیاں(fdt);
	newstroffset = fdt_off_dt_struct(fdt) + fdt_ناپ_dt_struct(fdt);
	memmove(p + newstroffset, p + oldstroffset, fdt_ناپ_dt_لڑیاں(fdt));
	fdt_سیٹ_off_dt_لڑیاں(fdt, newstroffset);

	/* Walk the structure, correcting string offsets */
	وتھ = 0;
	طالما ((ٹیگ = fdt_اگلا_ٹیگ(fdt, وتھ, &nextoffset)) != FDT_سرا) {
		إذا (ٹیگ == FDT_PROP) {
			بنية fdt_property *prop =
				fdt_وتھ_ptr_w(fdt, وتھ);
			صحيح nameoff;

			nameoff = fdt32_منزل_cpu(prop->nameoff);
			nameoff += fdt_ناپ_dt_لڑیاں(fdt);
			prop->nameoff = cpu_منزل_fdt32(nameoff);
		}
		وتھ = nextoffset;
	}
	إذا (nextoffset < 0)
		أرجع nextoffset;

	/* Finally, adjust the header */
	fdt_سیٹ_totalsize(fdt, newstroffset + fdt_ناپ_dt_لڑیاں(fdt));

	/* And fix up fields that were keeping intermediate state. */
	fdt_سیٹ_آخری_comp_نسخہ(fdt, FDT_آخری_COMPATIBLE_نسخہ);
	fdt_سیٹ_magic(fdt, FDT_MAGIC);

	أرجع 0;
}
