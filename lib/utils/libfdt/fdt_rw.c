// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#شامل "libfdt_env.h"

#شامل <fdt.h>
#شامل <libfdt.h>

#شامل "libfdt_internal.h"

ساکن صحیح fdt_بلاکیں_misordered(ثابت خالی *fdt,
				  صحیح mem_rsv_سائز, صحیح struct_سائز)
{
	واپس (fdt_off_mem_rsvmap(fdt) < FDT_ALIGN(حجم(ساخت fdt_سرخی), 8))
		|| (fdt_off_dt_struct(fdt) <
		    (fdt_off_mem_rsvmap(fdt) + mem_rsv_سائز))
		|| (fdt_off_dt_لڑیاں(fdt) <
		    (fdt_off_dt_struct(fdt) + struct_سائز))
		|| (fdt_totalsize(fdt) <
		    (fdt_off_dt_لڑیاں(fdt) + fdt_سائز_dt_لڑیاں(fdt)));
}

ساکن صحیح fdt_rw_probe_(خالی *fdt)
{
	اگر (can_assume(VALID_DTB))
		واپس 0;
	FDT_RO_PROBE(fdt);

	اگر (!can_assume(LATEST) && fdt_نسخہ(fdt) < 17)
		واپس -FDT_ERR_BADVERSION;
	اگر (fdt_بلاکیں_misordered(fdt, حجم(ساخت fdt_reserve_اندراج),
				   fdt_سائز_dt_struct(fdt)))
		واپس -FDT_ERR_BADLAYOUT;
	اگر (!can_assume(LATEST) && fdt_نسخہ(fdt) > 17)
		fdt_سیٹ_نسخہ(fdt, 17);

	واپس 0;
}

#تعریف_کرو FDT_RW_PROBE(fdt) \
	{ \
		صحیح err_; \
		اگر ((err_ = fdt_rw_probe_(fdt)) != 0) \
			واپس err_; \
	}

ساکن اندرونی غیرعلامتی صحیح fdt_ڈیٹا_سائز(خالی *fdt)
{
	واپس fdt_off_dt_لڑیاں(fdt) + fdt_سائز_dt_لڑیاں(fdt);
}

ساکن صحیح fdt_splice_(خالی *fdt, خالی *splicepoint, صحیح oldlen, صحیح newlen)
{
	حرف *p = splicepoint;
	غیرعلامتی صحیح dsize = fdt_ڈیٹا_سائز(fdt);
	size_t soff = p - (حرف *)fdt;

	اگر ((oldlen < 0) || (soff + oldlen < soff) || (soff + oldlen > dsize))
		واپس -FDT_ERR_BADOFFSET;
	اگر ((p < (حرف *)fdt) || (dsize + newlen < (غیرعلامتی)oldlen))
		واپس -FDT_ERR_BADOFFSET;
	اگر (dsize - oldlen + newlen > fdt_totalsize(fdt))
		واپس -FDT_ERR_NOSPACE;
	memmove(p + newlen, p + oldlen, ((حرف *)fdt + dsize) - (p + oldlen));
	واپس 0;
}

ساکن صحیح fdt_splice_mem_rsv_(خالی *fdt, ساخت fdt_reserve_اندراج *p,
			       صحیح oldn, صحیح newn)
{
	صحیح delta = (newn - oldn) * حجم(*p);
	صحیح err;
	err = fdt_splice_(fdt, p, oldn * حجم(*p), newn * حجم(*p));
	اگر (err)
		واپس err;
	fdt_سیٹ_off_dt_struct(fdt, fdt_off_dt_struct(fdt) + delta);
	fdt_سیٹ_off_dt_لڑیاں(fdt, fdt_off_dt_لڑیاں(fdt) + delta);
	واپس 0;
}

ساکن صحیح fdt_splice_struct_(خالی *fdt, خالی *p,
			      صحیح oldlen, صحیح newlen)
{
	صحیح delta = newlen - oldlen;
	صحیح err;

	اگر ((err = fdt_splice_(fdt, p, oldlen, newlen)))
		واپس err;

	fdt_سیٹ_سائز_dt_struct(fdt, fdt_سائز_dt_struct(fdt) + delta);
	fdt_سیٹ_off_dt_لڑیاں(fdt, fdt_off_dt_لڑیاں(fdt) + delta);
	واپس 0;
}

/* Must only be used to roll back in case of error */
ساکن خالی fdt_del_آخری_string(خالی *fdt, ثابت حرف *s)
{
	صحیح newlen = لڑی_لمبائی(s) + 1;

	fdt_سیٹ_سائز_dt_لڑیاں(fdt, fdt_سائز_dt_لڑیاں(fdt) - newlen);
}

ساکن صحیح fdt_splice_string_(خالی *fdt, صحیح newlen)
{
	خالی *p = (حرف *)fdt
		+ fdt_off_dt_لڑیاں(fdt) + fdt_سائز_dt_لڑیاں(fdt);
	صحیح err;

	اگر ((err = fdt_splice_(fdt, p, 0, newlen)))
		واپس err;

	fdt_سیٹ_سائز_dt_لڑیاں(fdt, fdt_سائز_dt_لڑیاں(fdt) + newlen);
	واپس 0;
}

/**
 * fdt_find_add_string_() - Find or allocate a string
 *
 * @fdt: pointer to the device tree to check/adjust
 * @s: string to find/add
 * @allocated: Set to 0 if the string was found, 1 if not found and so
 *	allocated. Ignored if can_assume(NO_ROLLBACK)
 * @return offset of string in the string table (whether found or added)
 */
ساکن صحیح fdt_ڈھونڈو_جوڑو_string(خالی *fdt, ثابت حرف *s, صحیح *allocated)
{
	حرف *strtab = (حرف *)fdt + fdt_off_dt_لڑیاں(fdt);
	ثابت حرف *p;
	حرف *new;
	صحیح len = لڑی_لمبائی(s) + 1;
	صحیح err;

	اگر (!can_assume(NO_ROLLBACK))
		*allocated = 0;

	p = fdt_ڈھونڈو_string(strtab, fdt_سائز_dt_لڑیاں(fdt), s);
	اگر (p)
		/* found it */
		واپس (p - strtab);

	new = strtab + fdt_سائز_dt_لڑیاں(fdt);
	err = fdt_splice_string_(fdt, len);
	اگر (err)
		واپس err;

	اگر (!can_assume(NO_ROLLBACK))
		*allocated = 1;

	حافظہ_نقل(new, s, len);
	واپس (new - strtab);
}

صحیح fdt_جوڑو_mem_rsv(خالی *fdt, uint64_t پتہ, uint64_t سائز)
{
	ساخت fdt_reserve_اندراج *re;
	صحیح err;

	FDT_RW_PROBE(fdt);

	re = fdt_mem_rsv_w_(fdt, fdt_عدد_mem_rsv(fdt));
	err = fdt_splice_mem_rsv_(fdt, re, 0, 1);
	اگر (err)
		واپس err;

	re->پتہ = cpu_مقصد_fdt64(پتہ);
	re->سائز = cpu_مقصد_fdt64(سائز);
	واپس 0;
}

صحیح fdt_del_mem_rsv(خالی *fdt, صحیح n)
{
	ساخت fdt_reserve_اندراج *re = fdt_mem_rsv_w_(fdt, n);

	FDT_RW_PROBE(fdt);

	اگر (n >= fdt_عدد_mem_rsv(fdt))
		واپس -FDT_ERR_NOTFOUND;

	واپس fdt_splice_mem_rsv_(fdt, re, 1, 0);
}

ساکن صحیح fdt_resize_property_(خالی *fdt, صحیح nodeoffset, ثابت حرف *name,
				صحیح len, ساخت fdt_property **prop)
{
	صحیح oldlen;
	صحیح err;

	*prop = fdt_get_property_w(fdt, nodeoffset, name, &oldlen);
	اگر (!*prop)
		واپس oldlen;

	اگر ((err = fdt_splice_struct_(fdt, (*prop)->ڈیٹا, FDT_TAGALIGN(oldlen),
				      FDT_TAGALIGN(len))))
		واپس err;

	(*prop)->len = cpu_مقصد_fdt32(len);
	واپس 0;
}

ساکن صحیح fdt_جوڑو_property(خالی *fdt, صحیح nodeoffset, ثابت حرف *name,
			     صحیح len, ساخت fdt_property **prop)
{
	صحیح proplen;
	صحیح nextoffset;
	صحیح namestroff;
	صحیح err;
	صحیح allocated;

	اگر ((nextoffset = fdt_check_گرہ_انحراف(fdt, nodeoffset)) < 0)
		واپس nextoffset;

	namestroff = fdt_ڈھونڈو_جوڑو_string(fdt, name, &allocated);
	اگر (namestroff < 0)
		واپس namestroff;

	*prop = fdt_انحراف_ptr_w(fdt, nextoffset);
	proplen = حجم(**prop) + FDT_TAGALIGN(len);

	err = fdt_splice_struct_(fdt, *prop, 0, proplen);
	اگر (err) {
		/* Delete the string if we failed to add it */
		اگر (!can_assume(NO_ROLLBACK) && allocated)
			fdt_del_آخری_string(fdt, name);
		واپس err;
	}

	(*prop)->لیبل = cpu_مقصد_fdt32(FDT_PROP);
	(*prop)->nameoff = cpu_مقصد_fdt32(namestroff);
	(*prop)->len = cpu_مقصد_fdt32(len);
	واپس 0;
}

صحیح fdt_سیٹ_name(خالی *fdt, صحیح nodeoffset, ثابت حرف *name)
{
	حرف *namep;
	صحیح oldlen, newlen;
	صحیح err;

	FDT_RW_PROBE(fdt);

	namep = (حرف *)(uintptr_t)fdt_get_name(fdt, nodeoffset, &oldlen);
	اگر (!namep)
		واپس oldlen;

	newlen = لڑی_لمبائی(name);

	err = fdt_splice_struct_(fdt, namep, FDT_TAGALIGN(oldlen+1),
				 FDT_TAGALIGN(newlen+1));
	اگر (err)
		واپس err;

	حافظہ_نقل(namep, name, newlen+1);
	واپس 0;
}

صحیح fdt_setprop_placeholder(خالی *fdt, صحیح nodeoffset, ثابت حرف *name,
			    صحیح len, خالی **prop_ڈیٹا)
{
	ساخت fdt_property *prop;
	صحیح err;

	FDT_RW_PROBE(fdt);

	err = fdt_resize_property_(fdt, nodeoffset, name, len, &prop);
	اگر (err == -FDT_ERR_NOTFOUND)
		err = fdt_جوڑو_property(fdt, nodeoffset, name, len, &prop);
	اگر (err)
		واپس err;

	*prop_ڈیٹا = prop->ڈیٹا;
	واپس 0;
}

صحیح fdt_setprop(خالی *fdt, صحیح nodeoffset, ثابت حرف *name,
		ثابت خالی *val, صحیح len)
{
	خالی *prop_ڈیٹا;
	صحیح err;

	err = fdt_setprop_placeholder(fdt, nodeoffset, name, len, &prop_ڈیٹا);
	اگر (err)
		واپس err;

	اگر (len)
		حافظہ_نقل(prop_ڈیٹا, val, len);
	واپس 0;
}

صحیح fdt_appendprop(خالی *fdt, صحیح nodeoffset, ثابت حرف *name,
		   ثابت خالی *val, صحیح len)
{
	ساخت fdt_property *prop;
	صحیح err, oldlen, newlen;

	FDT_RW_PROBE(fdt);

	prop = fdt_get_property_w(fdt, nodeoffset, name, &oldlen);
	اگر (prop) {
		newlen = len + oldlen;
		err = fdt_splice_struct_(fdt, prop->ڈیٹا,
					 FDT_TAGALIGN(oldlen),
					 FDT_TAGALIGN(newlen));
		اگر (err)
			واپس err;
		prop->len = cpu_مقصد_fdt32(newlen);
		حافظہ_نقل(prop->ڈیٹا + oldlen, val, len);
	} ورنہ {
		err = fdt_جوڑو_property(fdt, nodeoffset, name, len, &prop);
		اگر (err)
			واپس err;
		حافظہ_نقل(prop->ڈیٹا, val, len);
	}
	واپس 0;
}

صحیح fdt_delprop(خالی *fdt, صحیح nodeoffset, ثابت حرف *name)
{
	ساخت fdt_property *prop;
	صحیح len, proplen;

	FDT_RW_PROBE(fdt);

	prop = fdt_get_property_w(fdt, nodeoffset, name, &len);
	اگر (!prop)
		واپس len;

	proplen = حجم(*prop) + FDT_TAGALIGN(len);
	واپس fdt_splice_struct_(fdt, prop, proplen, 0);
}

صحیح fdt_جوڑو_subnode_namelen(خالی *fdt, صحیح parentoffset,
			    ثابت حرف *name, صحیح namelen)
{
	ساخت fdt_گرہ_سرخی *nh;
	صحیح انحراف, nextoffset;
	صحیح nodelen;
	صحیح err;
	uint32_t لیبل;
	fdt32_t *endtag;

	FDT_RW_PROBE(fdt);

	انحراف = fdt_subnode_انحراف_namelen(fdt, parentoffset, name, namelen);
	اگر (انحراف >= 0)
		واپس -FDT_ERR_EXISTS;
	ورنہ اگر (انحراف != -FDT_ERR_NOTFOUND)
		واپس انحراف;

	/* Try to place the new node after the parent's properties */
	لیبل = fdt_اگلا_لیبل(fdt, parentoffset, &nextoffset);
	/* the fdt_subnode_offset_namelen() should ensure this never hits */
	اگر (!can_assume(LIBFDT_FLAWLESS) && (لیبل != FDT_BEGIN_گرہ))
		واپس -FDT_ERR_اندرونی;
	کرو {
		انحراف = nextoffset;
		لیبل = fdt_اگلا_لیبل(fdt, انحراف, &nextoffset);
	} جب ((لیبل == FDT_PROP) || (لیبل == FDT_NOP));

	nh = fdt_انحراف_ptr_w(fdt, انحراف);
	nodelen = حجم(*nh) + FDT_TAGALIGN(namelen+1) + FDT_TAGSIZE;

	err = fdt_splice_struct_(fdt, nh, 0, nodelen);
	اگر (err)
		واپس err;

	nh->لیبل = cpu_مقصد_fdt32(FDT_BEGIN_گرہ);
	memset(nh->name, 0, FDT_TAGALIGN(namelen+1));
	حافظہ_نقل(nh->name, name, namelen);
	endtag = (fdt32_t *)((حرف *)nh + nodelen - FDT_TAGSIZE);
	*endtag = cpu_مقصد_fdt32(FDT_انتہا_گرہ);

	واپس انحراف;
}

صحیح fdt_جوڑو_subnode(خالی *fdt, صحیح parentoffset, ثابت حرف *name)
{
	واپس fdt_جوڑو_subnode_namelen(fdt, parentoffset, name, لڑی_لمبائی(name));
}

صحیح fdt_del_گرہ(خالی *fdt, صحیح nodeoffset)
{
	صحیح endoffset;

	FDT_RW_PROBE(fdt);

	endoffset = fdt_گرہ_انتہا_انحراف(fdt, nodeoffset);
	اگر (endoffset < 0)
		واپس endoffset;

	واپس fdt_splice_struct_(fdt, fdt_انحراف_ptr_w(fdt, nodeoffset),
				  endoffset - nodeoffset, 0);
}

ساکن خالی fdt_packblocks_(ثابت حرف *old, حرف *new,
			    صحیح mem_rsv_سائز,
			    صحیح struct_سائز,
			    صحیح لڑیاں_سائز)
{
	صحیح mem_rsv_off, struct_off, لڑیاں_off;

	mem_rsv_off = FDT_ALIGN(حجم(ساخت fdt_سرخی), 8);
	struct_off = mem_rsv_off + mem_rsv_سائز;
	لڑیاں_off = struct_off + struct_سائز;

	memmove(new + mem_rsv_off, old + fdt_off_mem_rsvmap(old), mem_rsv_سائز);
	fdt_سیٹ_off_mem_rsvmap(new, mem_rsv_off);

	memmove(new + struct_off, old + fdt_off_dt_struct(old), struct_سائز);
	fdt_سیٹ_off_dt_struct(new, struct_off);
	fdt_سیٹ_سائز_dt_struct(new, struct_سائز);

	memmove(new + لڑیاں_off, old + fdt_off_dt_لڑیاں(old), لڑیاں_سائز);
	fdt_سیٹ_off_dt_لڑیاں(new, لڑیاں_off);
	fdt_سیٹ_سائز_dt_لڑیاں(new, fdt_سائز_dt_لڑیاں(old));
}

صحیح fdt_open_into(ثابت خالی *fdt, خالی *buf, صحیح bufsize)
{
	صحیح err;
	صحیح mem_rsv_سائز, struct_سائز;
	صحیح newsize;
	ثابت حرف *fdtstart = fdt;
	ثابت حرف *fdtend = fdtstart + fdt_totalsize(fdt);
	حرف *tmp;

	FDT_RO_PROBE(fdt);

	mem_rsv_سائز = (fdt_عدد_mem_rsv(fdt)+1)
		* حجم(ساخت fdt_reserve_اندراج);

	اگر (can_assume(LATEST) || fdt_نسخہ(fdt) >= 17) {
		struct_سائز = fdt_سائز_dt_struct(fdt);
	} ورنہ اگر (fdt_نسخہ(fdt) == 16) {
		struct_سائز = 0;
		جب (fdt_اگلا_لیبل(fdt, struct_سائز, &struct_سائز) != FDT_انتہا)
			;
		اگر (struct_سائز < 0)
			واپس struct_سائز;
	} ورنہ {
		واپس -FDT_ERR_BADVERSION;
	}

	اگر (can_assume(LIBFDT_ترتیب) ||
	    !fdt_بلاکیں_misordered(fdt, mem_rsv_سائز, struct_سائز)) {
		/* no further work necessary */
		err = fdt_move(fdt, buf, bufsize);
		اگر (err)
			واپس err;
		fdt_سیٹ_نسخہ(buf, 17);
		fdt_سیٹ_سائز_dt_struct(buf, struct_سائز);
		fdt_سیٹ_totalsize(buf, bufsize);
		واپس 0;
	}

	/* Need to reorder */
	newsize = FDT_ALIGN(حجم(ساخت fdt_سرخی), 8) + mem_rsv_سائز
		+ struct_سائز + fdt_سائز_dt_لڑیاں(fdt);

	اگر (bufsize < newsize)
		واپس -FDT_ERR_NOSPACE;

	/* First attempt to build converted tree at beginning of buffer */
	tmp = buf;
	/* But if that overlaps with the old tree... */
	اگر (((tmp + newsize) > fdtstart) && (tmp < fdtend)) {
		/* Try right after the old tree instead */
		tmp = (حرف *)(uintptr_t)fdtend;
		اگر ((tmp + newsize) > ((حرف *)buf + bufsize))
			واپس -FDT_ERR_NOSPACE;
	}

	fdt_packblocks_(fdt, tmp, mem_rsv_سائز, struct_سائز,
			fdt_سائز_dt_لڑیاں(fdt));
	memmove(buf, tmp, newsize);

	fdt_سیٹ_magic(buf, FDT_MAGIC);
	fdt_سیٹ_totalsize(buf, bufsize);
	fdt_سیٹ_نسخہ(buf, 17);
	fdt_سیٹ_آخری_comp_نسخہ(buf, 16);
	fdt_سیٹ_boot_cpuid_phys(buf, fdt_boot_cpuid_phys(fdt));

	واپس 0;
}

صحیح fdt_pack(خالی *fdt)
{
	صحیح mem_rsv_سائز;

	FDT_RW_PROBE(fdt);

	mem_rsv_سائز = (fdt_عدد_mem_rsv(fdt)+1)
		* حجم(ساخت fdt_reserve_اندراج);
	fdt_packblocks_(fdt, fdt, mem_rsv_سائز, fdt_سائز_dt_struct(fdt),
			fdt_سائز_dt_لڑیاں(fdt));
	fdt_سیٹ_totalsize(fdt, fdt_ڈیٹا_سائز(fdt));

	واپس 0;
}
