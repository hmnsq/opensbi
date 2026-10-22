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
	إذا (!can_assume(VALID_ننوت)) {
		إذا (fdt_magic(fdt) == FDT_MAGIC)
			أرجع -FDT_ERR_BADSTATE;
		وإلا إذا (fdt_magic(fdt) != FDT_SW_MAGIC)
			أرجع -FDT_ERR_BADMAGIC;
	}

	أرجع 0;
}

#تعریف_کړه FDT_SW_PROBE(fdt) \
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

	إذا (!can_assume(VALID_ننوت) && fdt_off_dt_مزي_ګانې(fdt) != 0)
		أرجع -FDT_ERR_BADSTATE;
	أرجع 0;
}

#تعریف_کړه FDT_SW_PROBE_MEMRSV(fdt) \
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

	إذا (!can_assume(VALID_ننوت) &&
	    fdt_off_dt_مزي_ګانې(fdt) != fdt_totalsize(fdt))
		أرجع -FDT_ERR_BADSTATE;
	أرجع 0;
}

#تعریف_کړه FDT_SW_PROBE_STRUCT(fdt) \
	{ \
		صحيح err; \
		إذا ((err = fdt_sw_probe_struct_(fdt)) != 0) \
			أرجع err; \
	}

ساكن مضمن uint32_t sw_flags(فراغ *fdt)
{
	/* assert: (fdt_magic(fdt) == FDT_SW_MAGIC) */
	أرجع fdt_وروستی_comp_نسخه(fdt);
}

/* 'complete' state:	Enter this state after fdt_finish()
 *
 * Allowed functions: none
 */

ساكن فراغ *fdt_grab_space_(فراغ *fdt, size_t len)
{
	بینښه صحيح دننه_واټن = fdt_اندازه_dt_struct(fdt);
	بینښه صحيح spaceleft;

	spaceleft = fdt_totalsize(fdt) - fdt_off_dt_struct(fdt)
		- fdt_اندازه_dt_مزي_ګانې(fdt);

	إذا ((دننه_واټن + len < دننه_واټن) || (دننه_واټن + len > spaceleft))
		أرجع عدم;

	fdt_سېټ_اندازه_dt_struct(fdt, دننه_واټن + len);
	أرجع fdt_دننه_واټن_ptr_w(fdt, دننه_واټن);
}

صحيح fdt_create_with_flags(فراغ *buf, صحيح bufsize, uint32_t flags)
{
	ثابت صحيح hdrsize = FDT_ALIGN(حجم(بنية fdt_سرلیک),
				      حجم(بنية fdt_reserve_ثبت));
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
	fdt_سېټ_magic(fdt, FDT_SW_MAGIC);
	fdt_سېټ_نسخه(fdt, FDT_وروستی_SUPPORTED_نسخه);
	fdt_سېټ_وروستی_comp_نسخه(fdt, flags);

	fdt_سېټ_totalsize(fdt,  bufsize);

	fdt_سېټ_off_mem_rsvmap(fdt, hdrsize);
	fdt_سېټ_off_dt_struct(fdt, fdt_off_mem_rsvmap(fdt));
	fdt_سېټ_off_dt_مزي_ګانې(fdt, 0);

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
	tailsize = fdt_اندازه_dt_مزي_ګانې(fdt);

	إذا (!can_assume(VALID_DTB) &&
	    headsize + tailsize > fdt_totalsize(fdt))
		أرجع -FDT_ERR_دننی;

	إذا ((headsize + tailsize) > (بینښه)bufsize)
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

	fdt_سېټ_totalsize(buf, bufsize);
	إذا (fdt_off_dt_مزي_ګانې(buf))
		fdt_سېټ_off_dt_مزي_ګانې(buf, bufsize);

	أرجع 0;
}

صحيح fdt_جمع_کړه_reservemap_ثبت(فراغ *fdt, uint64_t addr, uint64_t اندازه)
{
	بنية fdt_reserve_ثبت *re;
	صحيح دننه_واټن;

	FDT_SW_PROBE_MEMRSV(fdt);

	دننه_واټن = fdt_off_dt_struct(fdt);
	إذا ((دننه_واټن + حجم(*re)) > fdt_totalsize(fdt))
		أرجع -FDT_ERR_NOSPACE;

	re = (بنية fdt_reserve_ثبت *)((حرف *)fdt + دننه_واټن);
	re->پته = cpu_منزل_fdt64(addr);
	re->اندازه = cpu_منزل_fdt64(اندازه);

	fdt_سېټ_off_dt_struct(fdt, دننه_واټن + حجم(*re));

	أرجع 0;
}

صحيح fdt_پای_reservemap(فراغ *fdt)
{
	صحيح err = fdt_جمع_کړه_reservemap_ثبت(fdt, 0, 0);

	إذا (err)
		أرجع err;

	fdt_سېټ_off_dt_مزي_ګانې(fdt, fdt_totalsize(fdt));
	أرجع 0;
}

صحيح fdt_begin_غوټه(فراغ *fdt, ثابت حرف *name)
{
	بنية fdt_غوټه_سرلیک *nh;
	صحيح namelen;

	FDT_SW_PROBE_STRUCT(fdt);

	namelen = مزي_اوږدوالی(name) + 1;
	nh = fdt_grab_space_(fdt, حجم(*nh) + FDT_TAGALIGN(namelen));
	إذا (! nh)
		أرجع -FDT_ERR_NOSPACE;

	nh->نښان = cpu_منزل_fdt32(FDT_BEGIN_غوټه);
	حافظه_نقل(nh->name, name, namelen);
	أرجع 0;
}

صحيح fdt_پای_ټکی_غوټه(فراغ *fdt)
{
	fdt32_t *en;

	FDT_SW_PROBE_STRUCT(fdt);

	en = fdt_grab_space_(fdt, FDT_TAGSIZE);
	إذا (! en)
		أرجع -FDT_ERR_NOSPACE;

	*en = cpu_منزل_fdt32(FDT_پای_ټکی_غوټه);
	أرجع 0;
}

ساكن صحيح fdt_جمع_کړه_string(فراغ *fdt, ثابت حرف *s)
{
	حرف *strtab = (حرف *)fdt + fdt_totalsize(fdt);
	بینښه صحيح strtabsize = fdt_اندازه_dt_مزي_ګانې(fdt);
	بینښه صحيح len = مزي_اوږدوالی(s) + 1;
	بینښه صحيح struct_سر, دننه_واټن;

	دننه_واټن = strtabsize + len;
	struct_سر = fdt_off_dt_struct(fdt) + fdt_اندازه_dt_struct(fdt);
	إذا (fdt_totalsize(fdt) - دننه_واټن < struct_سر)
		أرجع 0; /* no more room :( */

	حافظه_نقل(strtab - دننه_واټن, s, len);
	fdt_سېټ_اندازه_dt_مزي_ګانې(fdt, strtabsize + len);
	أرجع -دننه_واټن;
}

/* Must only be used to roll back in case of error */
ساكن فراغ fdt_del_وروستی_string(فراغ *fdt, ثابت حرف *s)
{
	صحيح strtabsize = fdt_اندازه_dt_مزي_ګانې(fdt);
	صحيح len = مزي_اوږدوالی(s) + 1;

	fdt_سېټ_اندازه_dt_مزي_ګانې(fdt, strtabsize - len);
}

ساكن صحيح fdt_پیدا_کړه_جمع_کړه_string(فراغ *fdt, ثابت حرف *s, صحيح *allocated)
{
	حرف *strtab = (حرف *)fdt + fdt_totalsize(fdt);
	صحيح strtabsize = fdt_اندازه_dt_مزي_ګانې(fdt);
	ثابت حرف *p;

	*allocated = 0;

	p = fdt_پیدا_کړه_string(strtab - strtabsize, strtabsize, s);
	إذا (p)
		أرجع p - strtab;

	*allocated = 1;

	أرجع fdt_جمع_کړه_string(fdt, s);
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
		nameoff = fdt_جمع_کړه_string(fdt, name);
	} وإلا {
		nameoff = fdt_پیدا_کړه_جمع_کړه_string(fdt, name, &allocated);
	}
	إذا (nameoff == 0)
		أرجع -FDT_ERR_NOSPACE;

	prop = fdt_grab_space_(fdt, حجم(*prop) + FDT_TAGALIGN(len));
	إذا (! prop) {
		إذا (allocated)
			fdt_del_وروستی_string(fdt, name);
		أرجع -FDT_ERR_NOSPACE;
	}

	prop->نښان = cpu_منزل_fdt32(FDT_PROP);
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
	حافظه_نقل(ptr, val, len);
	أرجع 0;
}

صحيح fdt_پای(فراغ *fdt)
{
	حرف *p = (حرف *)fdt;
	fdt32_t *پای_ټکی;
	صحيح oldstroffset, newstroffset;
	uint32_t نښان;
	صحيح دننه_واټن, nextoffset;

	FDT_SW_PROBE_STRUCT(fdt);

	/* Add terminator */
	پای_ټکی = fdt_grab_space_(fdt, حجم(*پای_ټکی));
	إذا (! پای_ټکی)
		أرجع -FDT_ERR_NOSPACE;
	*پای_ټکی = cpu_منزل_fdt32(FDT_پای_ټکی);

	/* Relocate the string table */
	oldstroffset = fdt_totalsize(fdt) - fdt_اندازه_dt_مزي_ګانې(fdt);
	newstroffset = fdt_off_dt_struct(fdt) + fdt_اندازه_dt_struct(fdt);
	memmove(p + newstroffset, p + oldstroffset, fdt_اندازه_dt_مزي_ګانې(fdt));
	fdt_سېټ_off_dt_مزي_ګانې(fdt, newstroffset);

	/* Walk the structure, correcting string offsets */
	دننه_واټن = 0;
	طالما ((نښان = fdt_بل_نښان(fdt, دننه_واټن, &nextoffset)) != FDT_پای_ټکی) {
		إذا (نښان == FDT_PROP) {
			بنية fdt_property *prop =
				fdt_دننه_واټن_ptr_w(fdt, دننه_واټن);
			صحيح nameoff;

			nameoff = fdt32_منزل_cpu(prop->nameoff);
			nameoff += fdt_اندازه_dt_مزي_ګانې(fdt);
			prop->nameoff = cpu_منزل_fdt32(nameoff);
		}
		دننه_واټن = nextoffset;
	}
	إذا (nextoffset < 0)
		أرجع nextoffset;

	/* Finally, adjust the header */
	fdt_سېټ_totalsize(fdt, newstroffset + fdt_اندازه_dt_مزي_ګانې(fdt));

	/* And fix up fields that were keeping intermediate state. */
	fdt_سېټ_وروستی_comp_نسخه(fdt, FDT_وروستی_COMPATIBLE_نسخه);
	fdt_سېټ_magic(fdt, FDT_MAGIC);

	أرجع 0;
}
