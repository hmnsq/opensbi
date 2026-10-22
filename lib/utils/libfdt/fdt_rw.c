// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#ضمن "libfdt_env.h"

#ضمن <fdt.h>
#ضمن <libfdt.h>

#ضمن "libfdt_internal.h"

ساكن صحيح fdt_بلاکاں_misordered(ثابت فراغ *fdt,
				  صحيح mem_rsv_ناپ, صحيح struct_ناپ)
{
	أرجع (fdt_off_mem_rsvmap(fdt) < FDT_ALIGN(حجم(بنية fdt_سرخی), 8))
		|| (fdt_off_dt_struct(fdt) <
		    (fdt_off_mem_rsvmap(fdt) + mem_rsv_ناپ))
		|| (fdt_off_dt_لڑیاں(fdt) <
		    (fdt_off_dt_struct(fdt) + struct_ناپ))
		|| (fdt_totalsize(fdt) <
		    (fdt_off_dt_لڑیاں(fdt) + fdt_ناپ_dt_لڑیاں(fdt)));
}

ساكن صحيح fdt_rw_probe_(فراغ *fdt)
{
	إذا (can_assume(VALID_DTB))
		أرجع 0;
	FDT_RO_PROBE(fdt);

	إذا (!can_assume(LATEST) && fdt_نسخہ(fdt) < 17)
		أرجع -FDT_ERR_BADVERSION;
	إذا (fdt_بلاکاں_misordered(fdt, حجم(بنية fdt_reserve_اندراج),
				   fdt_ناپ_dt_struct(fdt)))
		أرجع -FDT_ERR_BADLAYOUT;
	إذا (!can_assume(LATEST) && fdt_نسخہ(fdt) > 17)
		fdt_سیٹ_نسخہ(fdt, 17);

	أرجع 0;
}

#تعریف_کرو FDT_RW_PROBE(fdt) \
	{ \
		صحيح err_; \
		إذا ((err_ = fdt_rw_probe_(fdt)) != 0) \
			أرجع err_; \
	}

ساكن مضمن بنانشانی صحيح fdt_انکڑے_ناپ(فراغ *fdt)
{
	أرجع fdt_off_dt_لڑیاں(fdt) + fdt_ناپ_dt_لڑیاں(fdt);
}

ساكن صحيح fdt_splice_(فراغ *fdt, فراغ *splicepoint, صحيح oldlen, صحيح newlen)
{
	حرف *p = splicepoint;
	بنانشانی صحيح dsize = fdt_انکڑے_ناپ(fdt);
	size_t soff = p - (حرف *)fdt;

	إذا ((oldlen < 0) || (soff + oldlen < soff) || (soff + oldlen > dsize))
		أرجع -FDT_ERR_BADOFFSET;
	إذا ((p < (حرف *)fdt) || (dsize + newlen < (بنانشانی)oldlen))
		أرجع -FDT_ERR_BADOFFSET;
	إذا (dsize - oldlen + newlen > fdt_totalsize(fdt))
		أرجع -FDT_ERR_NOSPACE;
	memmove(p + newlen, p + oldlen, ((حرف *)fdt + dsize) - (p + oldlen));
	أرجع 0;
}

ساكن صحيح fdt_splice_mem_rsv_(فراغ *fdt, بنية fdt_reserve_اندراج *p,
			       صحيح oldn, صحيح newn)
{
	صحيح delta = (newn - oldn) * حجم(*p);
	صحيح err;
	err = fdt_splice_(fdt, p, oldn * حجم(*p), newn * حجم(*p));
	إذا (err)
		أرجع err;
	fdt_سیٹ_off_dt_struct(fdt, fdt_off_dt_struct(fdt) + delta);
	fdt_سیٹ_off_dt_لڑیاں(fdt, fdt_off_dt_لڑیاں(fdt) + delta);
	أرجع 0;
}

ساكن صحيح fdt_splice_struct_(فراغ *fdt, فراغ *p,
			      صحيح oldlen, صحيح newlen)
{
	صحيح delta = newlen - oldlen;
	صحيح err;

	إذا ((err = fdt_splice_(fdt, p, oldlen, newlen)))
		أرجع err;

	fdt_سیٹ_ناپ_dt_struct(fdt, fdt_ناپ_dt_struct(fdt) + delta);
	fdt_سیٹ_off_dt_لڑیاں(fdt, fdt_off_dt_لڑیاں(fdt) + delta);
	أرجع 0;
}

/* Must only be used to roll back in case of error */
ساكن فراغ fdt_del_آخری_string(فراغ *fdt, ثابت حرف *s)
{
	صحيح newlen = لڑی_لمبائی(s) + 1;

	fdt_سیٹ_ناپ_dt_لڑیاں(fdt, fdt_ناپ_dt_لڑیاں(fdt) - newlen);
}

ساكن صحيح fdt_splice_string_(فراغ *fdt, صحيح newlen)
{
	فراغ *p = (حرف *)fdt
		+ fdt_off_dt_لڑیاں(fdt) + fdt_ناپ_dt_لڑیاں(fdt);
	صحيح err;

	إذا ((err = fdt_splice_(fdt, p, 0, newlen)))
		أرجع err;

	fdt_سیٹ_ناپ_dt_لڑیاں(fdt, fdt_ناپ_dt_لڑیاں(fdt) + newlen);
	أرجع 0;
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
ساكن صحيح fdt_لبھو_جوڑو_string(فراغ *fdt, ثابت حرف *s, صحيح *allocated)
{
	حرف *strtab = (حرف *)fdt + fdt_off_dt_لڑیاں(fdt);
	ثابت حرف *p;
	حرف *new;
	صحيح len = لڑی_لمبائی(s) + 1;
	صحيح err;

	إذا (!can_assume(NO_ROLLBACK))
		*allocated = 0;

	p = fdt_لبھو_string(strtab, fdt_ناپ_dt_لڑیاں(fdt), s);
	إذا (p)
		/* found it */
		أرجع (p - strtab);

	new = strtab + fdt_ناپ_dt_لڑیاں(fdt);
	err = fdt_splice_string_(fdt, len);
	إذا (err)
		أرجع err;

	إذا (!can_assume(NO_ROLLBACK))
		*allocated = 1;

	حافظہ_نقل(new, s, len);
	أرجع (new - strtab);
}

صحيح fdt_جوڑو_mem_rsv(فراغ *fdt, uint64_t پتہ, uint64_t ناپ)
{
	بنية fdt_reserve_اندراج *re;
	صحيح err;

	FDT_RW_PROBE(fdt);

	re = fdt_mem_rsv_w_(fdt, fdt_عدد_mem_rsv(fdt));
	err = fdt_splice_mem_rsv_(fdt, re, 0, 1);
	إذا (err)
		أرجع err;

	re->پتہ = cpu_منزل_fdt64(پتہ);
	re->ناپ = cpu_منزل_fdt64(ناپ);
	أرجع 0;
}

صحيح fdt_del_mem_rsv(فراغ *fdt, صحيح n)
{
	بنية fdt_reserve_اندراج *re = fdt_mem_rsv_w_(fdt, n);

	FDT_RW_PROBE(fdt);

	إذا (n >= fdt_عدد_mem_rsv(fdt))
		أرجع -FDT_ERR_NOTFOUND;

	أرجع fdt_splice_mem_rsv_(fdt, re, 1, 0);
}

ساكن صحيح fdt_resize_property_(فراغ *fdt, صحيح nodeoffset, ثابت حرف *name,
				صحيح len, بنية fdt_property **prop)
{
	صحيح oldlen;
	صحيح err;

	*prop = fdt_get_property_w(fdt, nodeoffset, name, &oldlen);
	إذا (!*prop)
		أرجع oldlen;

	إذا ((err = fdt_splice_struct_(fdt, (*prop)->انکڑے, FDT_TAGALIGN(oldlen),
				      FDT_TAGALIGN(len))))
		أرجع err;

	(*prop)->len = cpu_منزل_fdt32(len);
	أرجع 0;
}

ساكن صحيح fdt_جوڑو_property(فراغ *fdt, صحيح nodeoffset, ثابت حرف *name,
			     صحيح len, بنية fdt_property **prop)
{
	صحيح proplen;
	صحيح nextoffset;
	صحيح namestroff;
	صحيح err;
	صحيح allocated;

	إذا ((nextoffset = fdt_check_گرہ_وتھ(fdt, nodeoffset)) < 0)
		أرجع nextoffset;

	namestroff = fdt_لبھو_جوڑو_string(fdt, name, &allocated);
	إذا (namestroff < 0)
		أرجع namestroff;

	*prop = fdt_وتھ_ptr_w(fdt, nextoffset);
	proplen = حجم(**prop) + FDT_TAGALIGN(len);

	err = fdt_splice_struct_(fdt, *prop, 0, proplen);
	إذا (err) {
		/* Delete the string if we failed to add it */
		إذا (!can_assume(NO_ROLLBACK) && allocated)
			fdt_del_آخری_string(fdt, name);
		أرجع err;
	}

	(*prop)->ٹیگ = cpu_منزل_fdt32(FDT_PROP);
	(*prop)->nameoff = cpu_منزل_fdt32(namestroff);
	(*prop)->len = cpu_منزل_fdt32(len);
	أرجع 0;
}

صحيح fdt_سیٹ_name(فراغ *fdt, صحيح nodeoffset, ثابت حرف *name)
{
	حرف *namep;
	صحيح oldlen, newlen;
	صحيح err;

	FDT_RW_PROBE(fdt);

	namep = (حرف *)(uintptr_t)fdt_get_name(fdt, nodeoffset, &oldlen);
	إذا (!namep)
		أرجع oldlen;

	newlen = لڑی_لمبائی(name);

	err = fdt_splice_struct_(fdt, namep, FDT_TAGALIGN(oldlen+1),
				 FDT_TAGALIGN(newlen+1));
	إذا (err)
		أرجع err;

	حافظہ_نقل(namep, name, newlen+1);
	أرجع 0;
}

صحيح fdt_setprop_placeholder(فراغ *fdt, صحيح nodeoffset, ثابت حرف *name,
			    صحيح len, فراغ **prop_انکڑے)
{
	بنية fdt_property *prop;
	صحيح err;

	FDT_RW_PROBE(fdt);

	err = fdt_resize_property_(fdt, nodeoffset, name, len, &prop);
	إذا (err == -FDT_ERR_NOTFOUND)
		err = fdt_جوڑو_property(fdt, nodeoffset, name, len, &prop);
	إذا (err)
		أرجع err;

	*prop_انکڑے = prop->انکڑے;
	أرجع 0;
}

صحيح fdt_setprop(فراغ *fdt, صحيح nodeoffset, ثابت حرف *name,
		ثابت فراغ *val, صحيح len)
{
	فراغ *prop_انکڑے;
	صحيح err;

	err = fdt_setprop_placeholder(fdt, nodeoffset, name, len, &prop_انکڑے);
	إذا (err)
		أرجع err;

	إذا (len)
		حافظہ_نقل(prop_انکڑے, val, len);
	أرجع 0;
}

صحيح fdt_appendprop(فراغ *fdt, صحيح nodeoffset, ثابت حرف *name,
		   ثابت فراغ *val, صحيح len)
{
	بنية fdt_property *prop;
	صحيح err, oldlen, newlen;

	FDT_RW_PROBE(fdt);

	prop = fdt_get_property_w(fdt, nodeoffset, name, &oldlen);
	إذا (prop) {
		newlen = len + oldlen;
		err = fdt_splice_struct_(fdt, prop->انکڑے,
					 FDT_TAGALIGN(oldlen),
					 FDT_TAGALIGN(newlen));
		إذا (err)
			أرجع err;
		prop->len = cpu_منزل_fdt32(newlen);
		حافظہ_نقل(prop->انکڑے + oldlen, val, len);
	} وإلا {
		err = fdt_جوڑو_property(fdt, nodeoffset, name, len, &prop);
		إذا (err)
			أرجع err;
		حافظہ_نقل(prop->انکڑے, val, len);
	}
	أرجع 0;
}

صحيح fdt_delprop(فراغ *fdt, صحيح nodeoffset, ثابت حرف *name)
{
	بنية fdt_property *prop;
	صحيح len, proplen;

	FDT_RW_PROBE(fdt);

	prop = fdt_get_property_w(fdt, nodeoffset, name, &len);
	إذا (!prop)
		أرجع len;

	proplen = حجم(*prop) + FDT_TAGALIGN(len);
	أرجع fdt_splice_struct_(fdt, prop, proplen, 0);
}

صحيح fdt_جوڑو_subnode_namelen(فراغ *fdt, صحيح parentoffset,
			    ثابت حرف *name, صحيح namelen)
{
	بنية fdt_گرہ_سرخی *nh;
	صحيح وتھ, nextoffset;
	صحيح nodelen;
	صحيح err;
	uint32_t ٹیگ;
	fdt32_t *endtag;

	FDT_RW_PROBE(fdt);

	وتھ = fdt_subnode_وتھ_namelen(fdt, parentoffset, name, namelen);
	إذا (وتھ >= 0)
		أرجع -FDT_ERR_EXISTS;
	وإلا إذا (وتھ != -FDT_ERR_NOTFOUND)
		أرجع وتھ;

	/* Try to place the new node after the parent's properties */
	ٹیگ = fdt_اگلا_ٹیگ(fdt, parentoffset, &nextoffset);
	/* the fdt_subnode_offset_namelen() should ensure this never hits */
	إذا (!can_assume(LIBFDT_FLAWLESS) && (ٹیگ != FDT_BEGIN_گرہ))
		أرجع -FDT_ERR_اندرونی;
	افعل {
		وتھ = nextoffset;
		ٹیگ = fdt_اگلا_ٹیگ(fdt, وتھ, &nextoffset);
	} طالما ((ٹیگ == FDT_PROP) || (ٹیگ == FDT_NOP));

	nh = fdt_وتھ_ptr_w(fdt, وتھ);
	nodelen = حجم(*nh) + FDT_TAGALIGN(namelen+1) + FDT_TAGSIZE;

	err = fdt_splice_struct_(fdt, nh, 0, nodelen);
	إذا (err)
		أرجع err;

	nh->ٹیگ = cpu_منزل_fdt32(FDT_BEGIN_گرہ);
	memset(nh->name, 0, FDT_TAGALIGN(namelen+1));
	حافظہ_نقل(nh->name, name, namelen);
	endtag = (fdt32_t *)((حرف *)nh + nodelen - FDT_TAGSIZE);
	*endtag = cpu_منزل_fdt32(FDT_سرا_گرہ);

	أرجع وتھ;
}

صحيح fdt_جوڑو_subnode(فراغ *fdt, صحيح parentoffset, ثابت حرف *name)
{
	أرجع fdt_جوڑو_subnode_namelen(fdt, parentoffset, name, لڑی_لمبائی(name));
}

صحيح fdt_del_گرہ(فراغ *fdt, صحيح nodeoffset)
{
	صحيح endoffset;

	FDT_RW_PROBE(fdt);

	endoffset = fdt_گرہ_سرا_وتھ(fdt, nodeoffset);
	إذا (endoffset < 0)
		أرجع endoffset;

	أرجع fdt_splice_struct_(fdt, fdt_وتھ_ptr_w(fdt, nodeoffset),
				  endoffset - nodeoffset, 0);
}

ساكن فراغ fdt_packblocks_(ثابت حرف *old, حرف *new,
			    صحيح mem_rsv_ناپ,
			    صحيح struct_ناپ,
			    صحيح لڑیاں_ناپ)
{
	صحيح mem_rsv_off, struct_off, لڑیاں_off;

	mem_rsv_off = FDT_ALIGN(حجم(بنية fdt_سرخی), 8);
	struct_off = mem_rsv_off + mem_rsv_ناپ;
	لڑیاں_off = struct_off + struct_ناپ;

	memmove(new + mem_rsv_off, old + fdt_off_mem_rsvmap(old), mem_rsv_ناپ);
	fdt_سیٹ_off_mem_rsvmap(new, mem_rsv_off);

	memmove(new + struct_off, old + fdt_off_dt_struct(old), struct_ناپ);
	fdt_سیٹ_off_dt_struct(new, struct_off);
	fdt_سیٹ_ناپ_dt_struct(new, struct_ناپ);

	memmove(new + لڑیاں_off, old + fdt_off_dt_لڑیاں(old), لڑیاں_ناپ);
	fdt_سیٹ_off_dt_لڑیاں(new, لڑیاں_off);
	fdt_سیٹ_ناپ_dt_لڑیاں(new, fdt_ناپ_dt_لڑیاں(old));
}

صحيح fdt_open_into(ثابت فراغ *fdt, فراغ *buf, صحيح bufsize)
{
	صحيح err;
	صحيح mem_rsv_ناپ, struct_ناپ;
	صحيح newsize;
	ثابت حرف *fdtstart = fdt;
	ثابت حرف *fdtend = fdtstart + fdt_totalsize(fdt);
	حرف *tmp;

	FDT_RO_PROBE(fdt);

	mem_rsv_ناپ = (fdt_عدد_mem_rsv(fdt)+1)
		* حجم(بنية fdt_reserve_اندراج);

	إذا (can_assume(LATEST) || fdt_نسخہ(fdt) >= 17) {
		struct_ناپ = fdt_ناپ_dt_struct(fdt);
	} وإلا إذا (fdt_نسخہ(fdt) == 16) {
		struct_ناپ = 0;
		طالما (fdt_اگلا_ٹیگ(fdt, struct_ناپ, &struct_ناپ) != FDT_سرا)
			;
		إذا (struct_ناپ < 0)
			أرجع struct_ناپ;
	} وإلا {
		أرجع -FDT_ERR_BADVERSION;
	}

	إذا (can_assume(LIBFDT_ترتیب) ||
	    !fdt_بلاکاں_misordered(fdt, mem_rsv_ناپ, struct_ناپ)) {
		/* no further work necessary */
		err = fdt_move(fdt, buf, bufsize);
		إذا (err)
			أرجع err;
		fdt_سیٹ_نسخہ(buf, 17);
		fdt_سیٹ_ناپ_dt_struct(buf, struct_ناپ);
		fdt_سیٹ_totalsize(buf, bufsize);
		أرجع 0;
	}

	/* Need to reorder */
	newsize = FDT_ALIGN(حجم(بنية fdt_سرخی), 8) + mem_rsv_ناپ
		+ struct_ناپ + fdt_ناپ_dt_لڑیاں(fdt);

	إذا (bufsize < newsize)
		أرجع -FDT_ERR_NOSPACE;

	/* First attempt to build converted tree at beginning of buffer */
	tmp = buf;
	/* But if that overlaps with the old tree... */
	إذا (((tmp + newsize) > fdtstart) && (tmp < fdtend)) {
		/* Try right after the old tree instead */
		tmp = (حرف *)(uintptr_t)fdtend;
		إذا ((tmp + newsize) > ((حرف *)buf + bufsize))
			أرجع -FDT_ERR_NOSPACE;
	}

	fdt_packblocks_(fdt, tmp, mem_rsv_ناپ, struct_ناپ,
			fdt_ناپ_dt_لڑیاں(fdt));
	memmove(buf, tmp, newsize);

	fdt_سیٹ_magic(buf, FDT_MAGIC);
	fdt_سیٹ_totalsize(buf, bufsize);
	fdt_سیٹ_نسخہ(buf, 17);
	fdt_سیٹ_آخری_comp_نسخہ(buf, 16);
	fdt_سیٹ_boot_cpuid_phys(buf, fdt_boot_cpuid_phys(fdt));

	أرجع 0;
}

صحيح fdt_pack(فراغ *fdt)
{
	صحيح mem_rsv_ناپ;

	FDT_RW_PROBE(fdt);

	mem_rsv_ناپ = (fdt_عدد_mem_rsv(fdt)+1)
		* حجم(بنية fdt_reserve_اندراج);
	fdt_packblocks_(fdt, fdt, mem_rsv_ناپ, fdt_ناپ_dt_struct(fdt),
			fdt_ناپ_dt_لڑیاں(fdt));
	fdt_سیٹ_totalsize(fdt, fdt_انکڑے_ناپ(fdt));

	أرجع 0;
}
