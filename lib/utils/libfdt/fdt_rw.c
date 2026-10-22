// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#ضمن "libfdt_env.h"

#ضمن <fdt.h>
#ضمن <libfdt.h>

#ضمن "libfdt_internal.h"

ساكن صحيح fdt_بلاڪون_misordered(ثابت فراغ *fdt,
				  صحيح mem_rsv_سائيز, صحيح struct_سائيز)
{
	أرجع (fdt_off_mem_rsvmap(fdt) < FDT_ALIGN(حجم(بنية fdt_سرو), 8))
		|| (fdt_off_dt_struct(fdt) <
		    (fdt_off_mem_rsvmap(fdt) + mem_rsv_سائيز))
		|| (fdt_off_dt_لڙيون(fdt) <
		    (fdt_off_dt_struct(fdt) + struct_سائيز))
		|| (fdt_totalsize(fdt) <
		    (fdt_off_dt_لڙيون(fdt) + fdt_سائيز_dt_لڙيون(fdt)));
}

ساكن صحيح fdt_rw_probe_(فراغ *fdt)
{
	إذا (can_assume(VALID_DTB))
		أرجع 0;
	FDT_RO_PROBE(fdt);

	إذا (!can_assume(LATEST) && fdt_نسخو(fdt) < 17)
		أرجع -FDT_ERR_BADVERSION;
	إذا (fdt_بلاڪون_misordered(fdt, حجم(بنية fdt_reserve_داخلا),
				   fdt_سائيز_dt_struct(fdt)))
		أرجع -FDT_ERR_BADLAYOUT;
	إذا (!can_assume(LATEST) && fdt_نسخو(fdt) > 17)
		fdt_سيٽ_نسخو(fdt, 17);

	أرجع 0;
}

#وصف_ڪريو FDT_RW_PROBE(fdt) \
	{ \
		صحيح err_; \
		إذا ((err_ = fdt_rw_probe_(fdt)) != 0) \
			أرجع err_; \
	}

ساكن مضمن بنانشان صحيح fdt_معلومات_سائيز(فراغ *fdt)
{
	أرجع fdt_off_dt_لڙيون(fdt) + fdt_سائيز_dt_لڙيون(fdt);
}

ساكن صحيح fdt_splice_(فراغ *fdt, فراغ *splicepoint, صحيح oldlen, صحيح newlen)
{
	حرف *p = splicepoint;
	بنانشان صحيح dsize = fdt_معلومات_سائيز(fdt);
	size_t soff = p - (حرف *)fdt;

	إذا ((oldlen < 0) || (soff + oldlen < soff) || (soff + oldlen > dsize))
		أرجع -FDT_ERR_BADOFFSET;
	إذا ((p < (حرف *)fdt) || (dsize + newlen < (بنانشان)oldlen))
		أرجع -FDT_ERR_BADOFFSET;
	إذا (dsize - oldlen + newlen > fdt_totalsize(fdt))
		أرجع -FDT_ERR_NOSPACE;
	memmove(p + newlen, p + oldlen, ((حرف *)fdt + dsize) - (p + oldlen));
	أرجع 0;
}

ساكن صحيح fdt_splice_mem_rsv_(فراغ *fdt, بنية fdt_reserve_داخلا *p,
			       صحيح oldn, صحيح newn)
{
	صحيح delta = (newn - oldn) * حجم(*p);
	صحيح err;
	err = fdt_splice_(fdt, p, oldn * حجم(*p), newn * حجم(*p));
	إذا (err)
		أرجع err;
	fdt_سيٽ_off_dt_struct(fdt, fdt_off_dt_struct(fdt) + delta);
	fdt_سيٽ_off_dt_لڙيون(fdt, fdt_off_dt_لڙيون(fdt) + delta);
	أرجع 0;
}

ساكن صحيح fdt_splice_struct_(فراغ *fdt, فراغ *p,
			      صحيح oldlen, صحيح newlen)
{
	صحيح delta = newlen - oldlen;
	صحيح err;

	إذا ((err = fdt_splice_(fdt, p, oldlen, newlen)))
		أرجع err;

	fdt_سيٽ_سائيز_dt_struct(fdt, fdt_سائيز_dt_struct(fdt) + delta);
	fdt_سيٽ_off_dt_لڙيون(fdt, fdt_off_dt_لڙيون(fdt) + delta);
	أرجع 0;
}

/* Must only be used to roll back in case of error */
ساكن فراغ fdt_del_آخري_string(فراغ *fdt, ثابت حرف *s)
{
	صحيح newlen = لڙي_ڊيگهه(s) + 1;

	fdt_سيٽ_سائيز_dt_لڙيون(fdt, fdt_سائيز_dt_لڙيون(fdt) - newlen);
}

ساكن صحيح fdt_splice_string_(فراغ *fdt, صحيح newlen)
{
	فراغ *p = (حرف *)fdt
		+ fdt_off_dt_لڙيون(fdt) + fdt_سائيز_dt_لڙيون(fdt);
	صحيح err;

	إذا ((err = fdt_splice_(fdt, p, 0, newlen)))
		أرجع err;

	fdt_سيٽ_سائيز_dt_لڙيون(fdt, fdt_سائيز_dt_لڙيون(fdt) + newlen);
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
ساكن صحيح fdt_ڳوليو_جوڙيو_string(فراغ *fdt, ثابت حرف *s, صحيح *allocated)
{
	حرف *strtab = (حرف *)fdt + fdt_off_dt_لڙيون(fdt);
	ثابت حرف *p;
	حرف *new;
	صحيح len = لڙي_ڊيگهه(s) + 1;
	صحيح err;

	إذا (!can_assume(NO_ROLLBACK))
		*allocated = 0;

	p = fdt_ڳوليو_string(strtab, fdt_سائيز_dt_لڙيون(fdt), s);
	إذا (p)
		/* found it */
		أرجع (p - strtab);

	new = strtab + fdt_سائيز_dt_لڙيون(fdt);
	err = fdt_splice_string_(fdt, len);
	إذا (err)
		أرجع err;

	إذا (!can_assume(NO_ROLLBACK))
		*allocated = 1;

	يادگيري_نقل(new, s, len);
	أرجع (new - strtab);
}

صحيح fdt_جوڙيو_mem_rsv(فراغ *fdt, uint64_t پتو, uint64_t سائيز)
{
	بنية fdt_reserve_داخلا *re;
	صحيح err;

	FDT_RW_PROBE(fdt);

	re = fdt_mem_rsv_w_(fdt, fdt_انگ_mem_rsv(fdt));
	err = fdt_splice_mem_rsv_(fdt, re, 0, 1);
	إذا (err)
		أرجع err;

	re->پتو = cpu_منزل_fdt64(پتو);
	re->سائيز = cpu_منزل_fdt64(سائيز);
	أرجع 0;
}

صحيح fdt_del_mem_rsv(فراغ *fdt, صحيح n)
{
	بنية fdt_reserve_داخلا *re = fdt_mem_rsv_w_(fdt, n);

	FDT_RW_PROBE(fdt);

	إذا (n >= fdt_انگ_mem_rsv(fdt))
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

	إذا ((err = fdt_splice_struct_(fdt, (*prop)->معلومات, FDT_TAGALIGN(oldlen),
				      FDT_TAGALIGN(len))))
		أرجع err;

	(*prop)->len = cpu_منزل_fdt32(len);
	أرجع 0;
}

ساكن صحيح fdt_جوڙيو_property(فراغ *fdt, صحيح nodeoffset, ثابت حرف *name,
			     صحيح len, بنية fdt_property **prop)
{
	صحيح proplen;
	صحيح nextoffset;
	صحيح namestroff;
	صحيح err;
	صحيح allocated;

	إذا ((nextoffset = fdt_check_ڳنڍ_وٿي(fdt, nodeoffset)) < 0)
		أرجع nextoffset;

	namestroff = fdt_ڳوليو_جوڙيو_string(fdt, name, &allocated);
	إذا (namestroff < 0)
		أرجع namestroff;

	*prop = fdt_وٿي_ptr_w(fdt, nextoffset);
	proplen = حجم(**prop) + FDT_TAGALIGN(len);

	err = fdt_splice_struct_(fdt, *prop, 0, proplen);
	إذا (err) {
		/* Delete the string if we failed to add it */
		إذا (!can_assume(NO_ROLLBACK) && allocated)
			fdt_del_آخري_string(fdt, name);
		أرجع err;
	}

	(*prop)->ٽيگ = cpu_منزل_fdt32(FDT_PROP);
	(*prop)->nameoff = cpu_منزل_fdt32(namestroff);
	(*prop)->len = cpu_منزل_fdt32(len);
	أرجع 0;
}

صحيح fdt_سيٽ_name(فراغ *fdt, صحيح nodeoffset, ثابت حرف *name)
{
	حرف *namep;
	صحيح oldlen, newlen;
	صحيح err;

	FDT_RW_PROBE(fdt);

	namep = (حرف *)(uintptr_t)fdt_get_name(fdt, nodeoffset, &oldlen);
	إذا (!namep)
		أرجع oldlen;

	newlen = لڙي_ڊيگهه(name);

	err = fdt_splice_struct_(fdt, namep, FDT_TAGALIGN(oldlen+1),
				 FDT_TAGALIGN(newlen+1));
	إذا (err)
		أرجع err;

	يادگيري_نقل(namep, name, newlen+1);
	أرجع 0;
}

صحيح fdt_setprop_placeholder(فراغ *fdt, صحيح nodeoffset, ثابت حرف *name,
			    صحيح len, فراغ **prop_معلومات)
{
	بنية fdt_property *prop;
	صحيح err;

	FDT_RW_PROBE(fdt);

	err = fdt_resize_property_(fdt, nodeoffset, name, len, &prop);
	إذا (err == -FDT_ERR_NOTFOUND)
		err = fdt_جوڙيو_property(fdt, nodeoffset, name, len, &prop);
	إذا (err)
		أرجع err;

	*prop_معلومات = prop->معلومات;
	أرجع 0;
}

صحيح fdt_setprop(فراغ *fdt, صحيح nodeoffset, ثابت حرف *name,
		ثابت فراغ *val, صحيح len)
{
	فراغ *prop_معلومات;
	صحيح err;

	err = fdt_setprop_placeholder(fdt, nodeoffset, name, len, &prop_معلومات);
	إذا (err)
		أرجع err;

	إذا (len)
		يادگيري_نقل(prop_معلومات, val, len);
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
		err = fdt_splice_struct_(fdt, prop->معلومات,
					 FDT_TAGALIGN(oldlen),
					 FDT_TAGALIGN(newlen));
		إذا (err)
			أرجع err;
		prop->len = cpu_منزل_fdt32(newlen);
		يادگيري_نقل(prop->معلومات + oldlen, val, len);
	} وإلا {
		err = fdt_جوڙيو_property(fdt, nodeoffset, name, len, &prop);
		إذا (err)
			أرجع err;
		يادگيري_نقل(prop->معلومات, val, len);
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

صحيح fdt_جوڙيو_subnode_namelen(فراغ *fdt, صحيح parentoffset,
			    ثابت حرف *name, صحيح namelen)
{
	بنية fdt_ڳنڍ_سرو *nh;
	صحيح وٿي, nextoffset;
	صحيح nodelen;
	صحيح err;
	uint32_t ٽيگ;
	fdt32_t *endtag;

	FDT_RW_PROBE(fdt);

	وٿي = fdt_subnode_وٿي_namelen(fdt, parentoffset, name, namelen);
	إذا (وٿي >= 0)
		أرجع -FDT_ERR_EXISTS;
	وإلا إذا (وٿي != -FDT_ERR_NOTFOUND)
		أرجع وٿي;

	/* Try to place the new node after the parent's properties */
	ٽيگ = fdt_ايندڙ_ٽيگ(fdt, parentoffset, &nextoffset);
	/* the fdt_subnode_offset_namelen() should ensure this never hits */
	إذا (!can_assume(LIBFDT_FLAWLESS) && (ٽيگ != FDT_BEGIN_ڳنڍ))
		أرجع -FDT_ERR_اندروني;
	افعل {
		وٿي = nextoffset;
		ٽيگ = fdt_ايندڙ_ٽيگ(fdt, وٿي, &nextoffset);
	} طالما ((ٽيگ == FDT_PROP) || (ٽيگ == FDT_NOP));

	nh = fdt_وٿي_ptr_w(fdt, وٿي);
	nodelen = حجم(*nh) + FDT_TAGALIGN(namelen+1) + FDT_TAGSIZE;

	err = fdt_splice_struct_(fdt, nh, 0, nodelen);
	إذا (err)
		أرجع err;

	nh->ٽيگ = cpu_منزل_fdt32(FDT_BEGIN_ڳنڍ);
	memset(nh->name, 0, FDT_TAGALIGN(namelen+1));
	يادگيري_نقل(nh->name, name, namelen);
	endtag = (fdt32_t *)((حرف *)nh + nodelen - FDT_TAGSIZE);
	*endtag = cpu_منزل_fdt32(FDT_پڇاڙي_ڳنڍ);

	أرجع وٿي;
}

صحيح fdt_جوڙيو_subnode(فراغ *fdt, صحيح parentoffset, ثابت حرف *name)
{
	أرجع fdt_جوڙيو_subnode_namelen(fdt, parentoffset, name, لڙي_ڊيگهه(name));
}

صحيح fdt_del_ڳنڍ(فراغ *fdt, صحيح nodeoffset)
{
	صحيح endoffset;

	FDT_RW_PROBE(fdt);

	endoffset = fdt_ڳنڍ_پڇاڙي_وٿي(fdt, nodeoffset);
	إذا (endoffset < 0)
		أرجع endoffset;

	أرجع fdt_splice_struct_(fdt, fdt_وٿي_ptr_w(fdt, nodeoffset),
				  endoffset - nodeoffset, 0);
}

ساكن فراغ fdt_packblocks_(ثابت حرف *old, حرف *new,
			    صحيح mem_rsv_سائيز,
			    صحيح struct_سائيز,
			    صحيح لڙيون_سائيز)
{
	صحيح mem_rsv_off, struct_off, لڙيون_off;

	mem_rsv_off = FDT_ALIGN(حجم(بنية fdt_سرو), 8);
	struct_off = mem_rsv_off + mem_rsv_سائيز;
	لڙيون_off = struct_off + struct_سائيز;

	memmove(new + mem_rsv_off, old + fdt_off_mem_rsvmap(old), mem_rsv_سائيز);
	fdt_سيٽ_off_mem_rsvmap(new, mem_rsv_off);

	memmove(new + struct_off, old + fdt_off_dt_struct(old), struct_سائيز);
	fdt_سيٽ_off_dt_struct(new, struct_off);
	fdt_سيٽ_سائيز_dt_struct(new, struct_سائيز);

	memmove(new + لڙيون_off, old + fdt_off_dt_لڙيون(old), لڙيون_سائيز);
	fdt_سيٽ_off_dt_لڙيون(new, لڙيون_off);
	fdt_سيٽ_سائيز_dt_لڙيون(new, fdt_سائيز_dt_لڙيون(old));
}

صحيح fdt_open_into(ثابت فراغ *fdt, فراغ *buf, صحيح bufsize)
{
	صحيح err;
	صحيح mem_rsv_سائيز, struct_سائيز;
	صحيح newsize;
	ثابت حرف *fdtstart = fdt;
	ثابت حرف *fdtend = fdtstart + fdt_totalsize(fdt);
	حرف *tmp;

	FDT_RO_PROBE(fdt);

	mem_rsv_سائيز = (fdt_انگ_mem_rsv(fdt)+1)
		* حجم(بنية fdt_reserve_داخلا);

	إذا (can_assume(LATEST) || fdt_نسخو(fdt) >= 17) {
		struct_سائيز = fdt_سائيز_dt_struct(fdt);
	} وإلا إذا (fdt_نسخو(fdt) == 16) {
		struct_سائيز = 0;
		طالما (fdt_ايندڙ_ٽيگ(fdt, struct_سائيز, &struct_سائيز) != FDT_پڇاڙي)
			;
		إذا (struct_سائيز < 0)
			أرجع struct_سائيز;
	} وإلا {
		أرجع -FDT_ERR_BADVERSION;
	}

	إذا (can_assume(LIBFDT_ترتيب) ||
	    !fdt_بلاڪون_misordered(fdt, mem_rsv_سائيز, struct_سائيز)) {
		/* no further work necessary */
		err = fdt_move(fdt, buf, bufsize);
		إذا (err)
			أرجع err;
		fdt_سيٽ_نسخو(buf, 17);
		fdt_سيٽ_سائيز_dt_struct(buf, struct_سائيز);
		fdt_سيٽ_totalsize(buf, bufsize);
		أرجع 0;
	}

	/* Need to reorder */
	newsize = FDT_ALIGN(حجم(بنية fdt_سرو), 8) + mem_rsv_سائيز
		+ struct_سائيز + fdt_سائيز_dt_لڙيون(fdt);

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

	fdt_packblocks_(fdt, tmp, mem_rsv_سائيز, struct_سائيز,
			fdt_سائيز_dt_لڙيون(fdt));
	memmove(buf, tmp, newsize);

	fdt_سيٽ_magic(buf, FDT_MAGIC);
	fdt_سيٽ_totalsize(buf, bufsize);
	fdt_سيٽ_نسخو(buf, 17);
	fdt_سيٽ_آخري_comp_نسخو(buf, 16);
	fdt_سيٽ_boot_cpuid_phys(buf, fdt_boot_cpuid_phys(fdt));

	أرجع 0;
}

صحيح fdt_pack(فراغ *fdt)
{
	صحيح mem_rsv_سائيز;

	FDT_RW_PROBE(fdt);

	mem_rsv_سائيز = (fdt_انگ_mem_rsv(fdt)+1)
		* حجم(بنية fdt_reserve_داخلا);
	fdt_packblocks_(fdt, fdt, mem_rsv_سائيز, fdt_سائيز_dt_struct(fdt),
			fdt_سائيز_dt_لڙيون(fdt));
	fdt_سيٽ_totalsize(fdt, fdt_معلومات_سائيز(fdt));

	أرجع 0;
}
