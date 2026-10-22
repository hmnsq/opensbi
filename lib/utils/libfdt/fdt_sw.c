// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#شامل "libfdt_env.h"

#شامل <fdt.h>
#شامل <libfdt.h>

#شامل "libfdt_internal.h"

ساکن صحیح fdt_sw_probe_(خالی *fdt)
{
	اگر (!can_assume(VALID_داخلہ)) {
		اگر (fdt_magic(fdt) == FDT_MAGIC)
			واپس -FDT_ERR_BADSTATE;
		ورنہ اگر (fdt_magic(fdt) != FDT_SW_MAGIC)
			واپس -FDT_ERR_BADMAGIC;
	}

	واپس 0;
}

#تعریف_کرو FDT_SW_PROBE(fdt) \
	{ \
		صحیح err; \
		اگر ((err = fdt_sw_probe_(fdt)) != 0) \
			واپس err; \
	}

/* 'memrsv' state:	Initial state after fdt_create()
 *
 * Allowed functions:
 *	fdt_add_reservemap_entry()
 *	fdt_finish_reservemap()		[moves to 'struct' state]
 */
ساکن صحیح fdt_sw_probe_memrsv_(خالی *fdt)
{
	صحیح err = fdt_sw_probe_(fdt);
	اگر (err)
		واپس err;

	اگر (!can_assume(VALID_داخلہ) && fdt_off_dt_لڑیاں(fdt) != 0)
		واپس -FDT_ERR_BADSTATE;
	واپس 0;
}

#تعریف_کرو FDT_SW_PROBE_MEMRSV(fdt) \
	{ \
		صحیح err; \
		اگر ((err = fdt_sw_probe_memrsv_(fdt)) != 0) \
			واپس err; \
	}

/* 'struct' state:	Enter this state after fdt_finish_reservemap()
 *
 * Allowed functions:
 *	fdt_begin_node()
 *	fdt_end_node()
 *	fdt_property*()
 *	fdt_finish()			[moves to 'complete' state]
 */
ساکن صحیح fdt_sw_probe_struct_(خالی *fdt)
{
	صحیح err = fdt_sw_probe_(fdt);
	اگر (err)
		واپس err;

	اگر (!can_assume(VALID_داخلہ) &&
	    fdt_off_dt_لڑیاں(fdt) != fdt_totalsize(fdt))
		واپس -FDT_ERR_BADSTATE;
	واپس 0;
}

#تعریف_کرو FDT_SW_PROBE_STRUCT(fdt) \
	{ \
		صحیح err; \
		اگر ((err = fdt_sw_probe_struct_(fdt)) != 0) \
			واپس err; \
	}

ساکن اندرونی uint32_t sw_flags(خالی *fdt)
{
	/* assert: (fdt_magic(fdt) == FDT_SW_MAGIC) */
	واپس fdt_آخری_comp_نسخہ(fdt);
}

/* 'complete' state:	Enter this state after fdt_finish()
 *
 * Allowed functions: none
 */

ساکن خالی *fdt_grab_space_(خالی *fdt, size_t len)
{
	غیرعلامتی صحیح انحراف = fdt_سائز_dt_struct(fdt);
	غیرعلامتی صحیح spaceleft;

	spaceleft = fdt_totalsize(fdt) - fdt_off_dt_struct(fdt)
		- fdt_سائز_dt_لڑیاں(fdt);

	اگر ((انحراف + len < انحراف) || (انحراف + len > spaceleft))
		واپس تہی;

	fdt_سیٹ_سائز_dt_struct(fdt, انحراف + len);
	واپس fdt_انحراف_ptr_w(fdt, انحراف);
}

صحیح fdt_create_with_flags(خالی *buf, صحیح bufsize, uint32_t flags)
{
	ثابت صحیح hdrsize = FDT_ALIGN(حجم(ساخت fdt_سرخی),
				      حجم(ساخت fdt_reserve_اندراج));
	خالی *fdt = buf;

	اگر (bufsize < hdrsize)
		واپس -FDT_ERR_NOSPACE;

	اگر (flags & ~FDT_CREATE_FLAGS_ALL)
		واپس -FDT_ERR_BADFLAGS;

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

	واپس 0;
}

صحیح fdt_create(خالی *buf, صحیح bufsize)
{
	واپس fdt_create_with_flags(buf, bufsize, 0);
}

صحیح fdt_resize(خالی *fdt, خالی *buf, صحیح bufsize)
{
	size_t headsize, tailsize;
	حرف *oldtail, *newtail;

	FDT_SW_PROBE(fdt);

	اگر (bufsize < 0)
		واپس -FDT_ERR_NOSPACE;

	headsize = fdt_off_dt_struct(fdt) + fdt_سائز_dt_struct(fdt);
	tailsize = fdt_سائز_dt_لڑیاں(fdt);

	اگر (!can_assume(VALID_DTB) &&
	    headsize + tailsize > fdt_totalsize(fdt))
		واپس -FDT_ERR_اندرونی;

	اگر ((headsize + tailsize) > (غیرعلامتی)bufsize)
		واپس -FDT_ERR_NOSPACE;

	oldtail = (حرف *)fdt + fdt_totalsize(fdt) - tailsize;
	newtail = (حرف *)buf + bufsize - tailsize;

	/* Two cases to avoid clobbering data if the old and new
	 * buffers partially overlap */
	اگر (buf <= fdt) {
		memmove(buf, fdt, headsize);
		memmove(newtail, oldtail, tailsize);
	} ورنہ {
		memmove(newtail, oldtail, tailsize);
		memmove(buf, fdt, headsize);
	}

	fdt_سیٹ_totalsize(buf, bufsize);
	اگر (fdt_off_dt_لڑیاں(buf))
		fdt_سیٹ_off_dt_لڑیاں(buf, bufsize);

	واپس 0;
}

صحیح fdt_جوڑو_reservemap_اندراج(خالی *fdt, uint64_t addr, uint64_t سائز)
{
	ساخت fdt_reserve_اندراج *re;
	صحیح انحراف;

	FDT_SW_PROBE_MEMRSV(fdt);

	انحراف = fdt_off_dt_struct(fdt);
	اگر ((انحراف + حجم(*re)) > fdt_totalsize(fdt))
		واپس -FDT_ERR_NOSPACE;

	re = (ساخت fdt_reserve_اندراج *)((حرف *)fdt + انحراف);
	re->پتہ = cpu_مقصد_fdt64(addr);
	re->سائز = cpu_مقصد_fdt64(سائز);

	fdt_سیٹ_off_dt_struct(fdt, انحراف + حجم(*re));

	واپس 0;
}

صحیح fdt_اختتام_reservemap(خالی *fdt)
{
	صحیح err = fdt_جوڑو_reservemap_اندراج(fdt, 0, 0);

	اگر (err)
		واپس err;

	fdt_سیٹ_off_dt_لڑیاں(fdt, fdt_totalsize(fdt));
	واپس 0;
}

صحیح fdt_begin_گرہ(خالی *fdt, ثابت حرف *name)
{
	ساخت fdt_گرہ_سرخی *nh;
	صحیح namelen;

	FDT_SW_PROBE_STRUCT(fdt);

	namelen = لڑی_لمبائی(name) + 1;
	nh = fdt_grab_space_(fdt, حجم(*nh) + FDT_TAGALIGN(namelen));
	اگر (! nh)
		واپس -FDT_ERR_NOSPACE;

	nh->لیبل = cpu_مقصد_fdt32(FDT_BEGIN_گرہ);
	حافظہ_نقل(nh->name, name, namelen);
	واپس 0;
}

صحیح fdt_انتہا_گرہ(خالی *fdt)
{
	fdt32_t *en;

	FDT_SW_PROBE_STRUCT(fdt);

	en = fdt_grab_space_(fdt, FDT_TAGSIZE);
	اگر (! en)
		واپس -FDT_ERR_NOSPACE;

	*en = cpu_مقصد_fdt32(FDT_انتہا_گرہ);
	واپس 0;
}

ساکن صحیح fdt_جوڑو_string(خالی *fdt, ثابت حرف *s)
{
	حرف *strtab = (حرف *)fdt + fdt_totalsize(fdt);
	غیرعلامتی صحیح strtabsize = fdt_سائز_dt_لڑیاں(fdt);
	غیرعلامتی صحیح len = لڑی_لمبائی(s) + 1;
	غیرعلامتی صحیح struct_چوٹی, انحراف;

	انحراف = strtabsize + len;
	struct_چوٹی = fdt_off_dt_struct(fdt) + fdt_سائز_dt_struct(fdt);
	اگر (fdt_totalsize(fdt) - انحراف < struct_چوٹی)
		واپس 0; /* no more room :( */

	حافظہ_نقل(strtab - انحراف, s, len);
	fdt_سیٹ_سائز_dt_لڑیاں(fdt, strtabsize + len);
	واپس -انحراف;
}

/* Must only be used to roll back in case of error */
ساکن خالی fdt_del_آخری_string(خالی *fdt, ثابت حرف *s)
{
	صحیح strtabsize = fdt_سائز_dt_لڑیاں(fdt);
	صحیح len = لڑی_لمبائی(s) + 1;

	fdt_سیٹ_سائز_dt_لڑیاں(fdt, strtabsize - len);
}

ساکن صحیح fdt_ڈھونڈو_جوڑو_string(خالی *fdt, ثابت حرف *s, صحیح *allocated)
{
	حرف *strtab = (حرف *)fdt + fdt_totalsize(fdt);
	صحیح strtabsize = fdt_سائز_dt_لڑیاں(fdt);
	ثابت حرف *p;

	*allocated = 0;

	p = fdt_ڈھونڈو_string(strtab - strtabsize, strtabsize, s);
	اگر (p)
		واپس p - strtab;

	*allocated = 1;

	واپس fdt_جوڑو_string(fdt, s);
}

صحیح fdt_property_placeholder(خالی *fdt, ثابت حرف *name, صحیح len, خالی **valp)
{
	ساخت fdt_property *prop;
	صحیح nameoff;
	صحیح allocated;

	FDT_SW_PROBE_STRUCT(fdt);

	/* String de-duplication can be slow, _NO_NAME_DEDUP skips it */
	اگر (sw_flags(fdt) & FDT_CREATE_FLAG_NO_NAME_DEDUP) {
		allocated = 1;
		nameoff = fdt_جوڑو_string(fdt, name);
	} ورنہ {
		nameoff = fdt_ڈھونڈو_جوڑو_string(fdt, name, &allocated);
	}
	اگر (nameoff == 0)
		واپس -FDT_ERR_NOSPACE;

	prop = fdt_grab_space_(fdt, حجم(*prop) + FDT_TAGALIGN(len));
	اگر (! prop) {
		اگر (allocated)
			fdt_del_آخری_string(fdt, name);
		واپس -FDT_ERR_NOSPACE;
	}

	prop->لیبل = cpu_مقصد_fdt32(FDT_PROP);
	prop->nameoff = cpu_مقصد_fdt32(nameoff);
	prop->len = cpu_مقصد_fdt32(len);
	*valp = prop->ڈیٹا;
	واپس 0;
}

صحیح fdt_property(خالی *fdt, ثابت حرف *name, ثابت خالی *val, صحیح len)
{
	خالی *ptr;
	صحیح ret;

	ret = fdt_property_placeholder(fdt, name, len, &ptr);
	اگر (ret)
		واپس ret;
	حافظہ_نقل(ptr, val, len);
	واپس 0;
}

صحیح fdt_اختتام(خالی *fdt)
{
	حرف *p = (حرف *)fdt;
	fdt32_t *انتہا;
	صحیح oldstroffset, newstroffset;
	uint32_t لیبل;
	صحیح انحراف, nextoffset;

	FDT_SW_PROBE_STRUCT(fdt);

	/* Add terminator */
	انتہا = fdt_grab_space_(fdt, حجم(*انتہا));
	اگر (! انتہا)
		واپس -FDT_ERR_NOSPACE;
	*انتہا = cpu_مقصد_fdt32(FDT_انتہا);

	/* Relocate the string table */
	oldstroffset = fdt_totalsize(fdt) - fdt_سائز_dt_لڑیاں(fdt);
	newstroffset = fdt_off_dt_struct(fdt) + fdt_سائز_dt_struct(fdt);
	memmove(p + newstroffset, p + oldstroffset, fdt_سائز_dt_لڑیاں(fdt));
	fdt_سیٹ_off_dt_لڑیاں(fdt, newstroffset);

	/* Walk the structure, correcting string offsets */
	انحراف = 0;
	جب ((لیبل = fdt_اگلا_لیبل(fdt, انحراف, &nextoffset)) != FDT_انتہا) {
		اگر (لیبل == FDT_PROP) {
			ساخت fdt_property *prop =
				fdt_انحراف_ptr_w(fdt, انحراف);
			صحیح nameoff;

			nameoff = fdt32_مقصد_cpu(prop->nameoff);
			nameoff += fdt_سائز_dt_لڑیاں(fdt);
			prop->nameoff = cpu_مقصد_fdt32(nameoff);
		}
		انحراف = nextoffset;
	}
	اگر (nextoffset < 0)
		واپس nextoffset;

	/* Finally, adjust the header */
	fdt_سیٹ_totalsize(fdt, newstroffset + fdt_سائز_dt_لڑیاں(fdt));

	/* And fix up fields that were keeping intermediate state. */
	fdt_سیٹ_آخری_comp_نسخہ(fdt, FDT_آخری_COMPATIBLE_نسخہ);
	fdt_سیٹ_magic(fdt, FDT_MAGIC);

	واپس 0;
}
