// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#ضمن "libfdt_env.h"

#ضمن <fdt.h>
#ضمن <libfdt.h>

#ضمن "libfdt_internal.h"

ساكن صحيح fdt_بلوکها_misordered(ثابت فراغ *fdt,
				  صحيح mem_rsv_اندازه, صحيح struct_اندازه)
{
	أرجع (fdt_off_mem_rsvmap(fdt) < FDT_ALIGN(حجم(بنية fdt_سرآیند), 8))
		|| (fdt_off_dt_struct(fdt) <
		    (fdt_off_mem_rsvmap(fdt) + mem_rsv_اندازه))
		|| (fdt_off_dt_رشتهها(fdt) <
		    (fdt_off_dt_struct(fdt) + struct_اندازه))
		|| (fdt_totalsize(fdt) <
		    (fdt_off_dt_رشتهها(fdt) + fdt_اندازه_dt_رشتهها(fdt)));
}

ساكن صحيح fdt_rw_probe_(فراغ *fdt)
{
	إذا (can_assume(VALID_DTB))
		أرجع 0;
	FDT_RO_PROBE(fdt);

	إذا (!can_assume(LATEST) && fdt_نسخه(fdt) < 17)
		أرجع -FDT_ERR_BADVERSION;
	إذا (fdt_بلوکها_misordered(fdt, حجم(بنية fdt_reserve_مدخل),
				   fdt_اندازه_dt_struct(fdt)))
		أرجع -FDT_ERR_BADLAYOUT;
	إذا (!can_assume(LATEST) && fdt_نسخه(fdt) > 17)
		fdt_مجموعه_نسخه(fdt, 17);

	أرجع 0;
}

#تعریف_کن FDT_RW_PROBE(fdt) \
	{ \
		صحيح err_; \
		إذا ((err_ = fdt_rw_probe_(fdt)) != 0) \
			أرجع err_; \
	}

ساكن مضمن بیعلامت صحيح fdt_داده_اندازه(فراغ *fdt)
{
	أرجع fdt_off_dt_رشتهها(fdt) + fdt_اندازه_dt_رشتهها(fdt);
}

ساكن صحيح fdt_splice_(فراغ *fdt, فراغ *splicepoint, صحيح oldlen, صحيح newlen)
{
	حرف *p = splicepoint;
	بیعلامت صحيح dsize = fdt_داده_اندازه(fdt);
	size_t soff = p - (حرف *)fdt;

	إذا ((oldlen < 0) || (soff + oldlen < soff) || (soff + oldlen > dsize))
		أرجع -FDT_ERR_BADOFFSET;
	إذا ((p < (حرف *)fdt) || (dsize + newlen < (بیعلامت)oldlen))
		أرجع -FDT_ERR_BADOFFSET;
	إذا (dsize - oldlen + newlen > fdt_totalsize(fdt))
		أرجع -FDT_ERR_NOSPACE;
	memmove(p + newlen, p + oldlen, ((حرف *)fdt + dsize) - (p + oldlen));
	أرجع 0;
}

ساكن صحيح fdt_splice_mem_rsv_(فراغ *fdt, بنية fdt_reserve_مدخل *p,
			       صحيح oldn, صحيح newn)
{
	صحيح delta = (newn - oldn) * حجم(*p);
	صحيح err;
	err = fdt_splice_(fdt, p, oldn * حجم(*p), newn * حجم(*p));
	إذا (err)
		أرجع err;
	fdt_مجموعه_off_dt_struct(fdt, fdt_off_dt_struct(fdt) + delta);
	fdt_مجموعه_off_dt_رشتهها(fdt, fdt_off_dt_رشتهها(fdt) + delta);
	أرجع 0;
}

ساكن صحيح fdt_splice_struct_(فراغ *fdt, فراغ *p,
			      صحيح oldlen, صحيح newlen)
{
	صحيح delta = newlen - oldlen;
	صحيح err;

	إذا ((err = fdt_splice_(fdt, p, oldlen, newlen)))
		أرجع err;

	fdt_مجموعه_اندازه_dt_struct(fdt, fdt_اندازه_dt_struct(fdt) + delta);
	fdt_مجموعه_off_dt_رشتهها(fdt, fdt_off_dt_رشتهها(fdt) + delta);
	أرجع 0;
}

/* Must only be used to roll back in case of error */
ساكن فراغ fdt_del_آخرین_string(فراغ *fdt, ثابت حرف *s)
{
	صحيح newlen = طول_رشته(s) + 1;

	fdt_مجموعه_اندازه_dt_رشتهها(fdt, fdt_اندازه_dt_رشتهها(fdt) - newlen);
}

ساكن صحيح fdt_splice_string_(فراغ *fdt, صحيح newlen)
{
	فراغ *p = (حرف *)fdt
		+ fdt_off_dt_رشتهها(fdt) + fdt_اندازه_dt_رشتهها(fdt);
	صحيح err;

	إذا ((err = fdt_splice_(fdt, p, 0, newlen)))
		أرجع err;

	fdt_مجموعه_اندازه_dt_رشتهها(fdt, fdt_اندازه_dt_رشتهها(fdt) + newlen);
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
ساكن صحيح fdt_پیدا_کن_جمع_کن_string(فراغ *fdt, ثابت حرف *s, صحيح *allocated)
{
	حرف *strtab = (حرف *)fdt + fdt_off_dt_رشتهها(fdt);
	ثابت حرف *p;
	حرف *new;
	صحيح len = طول_رشته(s) + 1;
	صحيح err;

	إذا (!can_assume(NO_ROLLBACK))
		*allocated = 0;

	p = fdt_پیدا_کن_string(strtab, fdt_اندازه_dt_رشتهها(fdt), s);
	إذا (p)
		/* found it */
		أرجع (p - strtab);

	new = strtab + fdt_اندازه_dt_رشتهها(fdt);
	err = fdt_splice_string_(fdt, len);
	إذا (err)
		أرجع err;

	إذا (!can_assume(NO_ROLLBACK))
		*allocated = 1;

	کپی_حافظه(new, s, len);
	أرجع (new - strtab);
}

صحيح fdt_جمع_کن_mem_rsv(فراغ *fdt, uint64_t آدرس, uint64_t اندازه)
{
	بنية fdt_reserve_مدخل *re;
	صحيح err;

	FDT_RW_PROBE(fdt);

	re = fdt_mem_rsv_w_(fdt, fdt_رقم_mem_rsv(fdt));
	err = fdt_splice_mem_rsv_(fdt, re, 0, 1);
	إذا (err)
		أرجع err;

	re->آدرس = cpu_مقصد_fdt64(آدرس);
	re->اندازه = cpu_مقصد_fdt64(اندازه);
	أرجع 0;
}

صحيح fdt_del_mem_rsv(فراغ *fdt, صحيح n)
{
	بنية fdt_reserve_مدخل *re = fdt_mem_rsv_w_(fdt, n);

	FDT_RW_PROBE(fdt);

	إذا (n >= fdt_رقم_mem_rsv(fdt))
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

	إذا ((err = fdt_splice_struct_(fdt, (*prop)->داده, FDT_TAGALIGN(oldlen),
				      FDT_TAGALIGN(len))))
		أرجع err;

	(*prop)->len = cpu_مقصد_fdt32(len);
	أرجع 0;
}

ساكن صحيح fdt_جمع_کن_property(فراغ *fdt, صحيح nodeoffset, ثابت حرف *name,
			     صحيح len, بنية fdt_property **prop)
{
	صحيح proplen;
	صحيح nextoffset;
	صحيح namestroff;
	صحيح err;
	صحيح allocated;

	إذا ((nextoffset = fdt_check_گره_جابه_جایی(fdt, nodeoffset)) < 0)
		أرجع nextoffset;

	namestroff = fdt_پیدا_کن_جمع_کن_string(fdt, name, &allocated);
	إذا (namestroff < 0)
		أرجع namestroff;

	*prop = fdt_جابه_جایی_ptr_w(fdt, nextoffset);
	proplen = حجم(**prop) + FDT_TAGALIGN(len);

	err = fdt_splice_struct_(fdt, *prop, 0, proplen);
	إذا (err) {
		/* Delete the string if we failed to add it */
		إذا (!can_assume(NO_ROLLBACK) && allocated)
			fdt_del_آخرین_string(fdt, name);
		أرجع err;
	}

	(*prop)->برچسب = cpu_مقصد_fdt32(FDT_PROP);
	(*prop)->nameoff = cpu_مقصد_fdt32(namestroff);
	(*prop)->len = cpu_مقصد_fdt32(len);
	أرجع 0;
}

صحيح fdt_مجموعه_name(فراغ *fdt, صحيح nodeoffset, ثابت حرف *name)
{
	حرف *namep;
	صحيح oldlen, newlen;
	صحيح err;

	FDT_RW_PROBE(fdt);

	namep = (حرف *)(uintptr_t)fdt_get_name(fdt, nodeoffset, &oldlen);
	إذا (!namep)
		أرجع oldlen;

	newlen = طول_رشته(name);

	err = fdt_splice_struct_(fdt, namep, FDT_TAGALIGN(oldlen+1),
				 FDT_TAGALIGN(newlen+1));
	إذا (err)
		أرجع err;

	کپی_حافظه(namep, name, newlen+1);
	أرجع 0;
}

صحيح fdt_setprop_placeholder(فراغ *fdt, صحيح nodeoffset, ثابت حرف *name,
			    صحيح len, فراغ **prop_داده)
{
	بنية fdt_property *prop;
	صحيح err;

	FDT_RW_PROBE(fdt);

	err = fdt_resize_property_(fdt, nodeoffset, name, len, &prop);
	إذا (err == -FDT_ERR_NOTFOUND)
		err = fdt_جمع_کن_property(fdt, nodeoffset, name, len, &prop);
	إذا (err)
		أرجع err;

	*prop_داده = prop->داده;
	أرجع 0;
}

صحيح fdt_setprop(فراغ *fdt, صحيح nodeoffset, ثابت حرف *name,
		ثابت فراغ *val, صحيح len)
{
	فراغ *prop_داده;
	صحيح err;

	err = fdt_setprop_placeholder(fdt, nodeoffset, name, len, &prop_داده);
	إذا (err)
		أرجع err;

	إذا (len)
		کپی_حافظه(prop_داده, val, len);
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
		err = fdt_splice_struct_(fdt, prop->داده,
					 FDT_TAGALIGN(oldlen),
					 FDT_TAGALIGN(newlen));
		إذا (err)
			أرجع err;
		prop->len = cpu_مقصد_fdt32(newlen);
		کپی_حافظه(prop->داده + oldlen, val, len);
	} وإلا {
		err = fdt_جمع_کن_property(fdt, nodeoffset, name, len, &prop);
		إذا (err)
			أرجع err;
		کپی_حافظه(prop->داده, val, len);
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

صحيح fdt_جمع_کن_subnode_namelen(فراغ *fdt, صحيح parentoffset,
			    ثابت حرف *name, صحيح namelen)
{
	بنية fdt_گره_سرآیند *nh;
	صحيح جابه_جایی, nextoffset;
	صحيح nodelen;
	صحيح err;
	uint32_t برچسب;
	fdt32_t *endtag;

	FDT_RW_PROBE(fdt);

	جابه_جایی = fdt_subnode_جابه_جایی_namelen(fdt, parentoffset, name, namelen);
	إذا (جابه_جایی >= 0)
		أرجع -FDT_ERR_EXISTS;
	وإلا إذا (جابه_جایی != -FDT_ERR_NOTFOUND)
		أرجع جابه_جایی;

	/* Try to place the new node after the parent's properties */
	برچسب = fdt_بعدی_برچسب(fdt, parentoffset, &nextoffset);
	/* the fdt_subnode_offset_namelen() should ensure this never hits */
	إذا (!can_assume(LIBFDT_FLAWLESS) && (برچسب != FDT_BEGIN_گره))
		أرجع -FDT_ERR_داخلی;
	افعل {
		جابه_جایی = nextoffset;
		برچسب = fdt_بعدی_برچسب(fdt, جابه_جایی, &nextoffset);
	} طالما ((برچسب == FDT_PROP) || (برچسب == FDT_NOP));

	nh = fdt_جابه_جایی_ptr_w(fdt, جابه_جایی);
	nodelen = حجم(*nh) + FDT_TAGALIGN(namelen+1) + FDT_TAGSIZE;

	err = fdt_splice_struct_(fdt, nh, 0, nodelen);
	إذا (err)
		أرجع err;

	nh->برچسب = cpu_مقصد_fdt32(FDT_BEGIN_گره);
	memset(nh->name, 0, FDT_TAGALIGN(namelen+1));
	کپی_حافظه(nh->name, name, namelen);
	endtag = (fdt32_t *)((حرف *)nh + nodelen - FDT_TAGSIZE);
	*endtag = cpu_مقصد_fdt32(FDT_انتها_گره);

	أرجع جابه_جایی;
}

صحيح fdt_جمع_کن_subnode(فراغ *fdt, صحيح parentoffset, ثابت حرف *name)
{
	أرجع fdt_جمع_کن_subnode_namelen(fdt, parentoffset, name, طول_رشته(name));
}

صحيح fdt_del_گره(فراغ *fdt, صحيح nodeoffset)
{
	صحيح endoffset;

	FDT_RW_PROBE(fdt);

	endoffset = fdt_گره_انتها_جابه_جایی(fdt, nodeoffset);
	إذا (endoffset < 0)
		أرجع endoffset;

	أرجع fdt_splice_struct_(fdt, fdt_جابه_جایی_ptr_w(fdt, nodeoffset),
				  endoffset - nodeoffset, 0);
}

ساكن فراغ fdt_packblocks_(ثابت حرف *old, حرف *new,
			    صحيح mem_rsv_اندازه,
			    صحيح struct_اندازه,
			    صحيح رشتهها_اندازه)
{
	صحيح mem_rsv_off, struct_off, رشتهها_off;

	mem_rsv_off = FDT_ALIGN(حجم(بنية fdt_سرآیند), 8);
	struct_off = mem_rsv_off + mem_rsv_اندازه;
	رشتهها_off = struct_off + struct_اندازه;

	memmove(new + mem_rsv_off, old + fdt_off_mem_rsvmap(old), mem_rsv_اندازه);
	fdt_مجموعه_off_mem_rsvmap(new, mem_rsv_off);

	memmove(new + struct_off, old + fdt_off_dt_struct(old), struct_اندازه);
	fdt_مجموعه_off_dt_struct(new, struct_off);
	fdt_مجموعه_اندازه_dt_struct(new, struct_اندازه);

	memmove(new + رشتهها_off, old + fdt_off_dt_رشتهها(old), رشتهها_اندازه);
	fdt_مجموعه_off_dt_رشتهها(new, رشتهها_off);
	fdt_مجموعه_اندازه_dt_رشتهها(new, fdt_اندازه_dt_رشتهها(old));
}

صحيح fdt_open_into(ثابت فراغ *fdt, فراغ *buf, صحيح bufsize)
{
	صحيح err;
	صحيح mem_rsv_اندازه, struct_اندازه;
	صحيح newsize;
	ثابت حرف *fdtstart = fdt;
	ثابت حرف *fdtend = fdtstart + fdt_totalsize(fdt);
	حرف *tmp;

	FDT_RO_PROBE(fdt);

	mem_rsv_اندازه = (fdt_رقم_mem_rsv(fdt)+1)
		* حجم(بنية fdt_reserve_مدخل);

	إذا (can_assume(LATEST) || fdt_نسخه(fdt) >= 17) {
		struct_اندازه = fdt_اندازه_dt_struct(fdt);
	} وإلا إذا (fdt_نسخه(fdt) == 16) {
		struct_اندازه = 0;
		طالما (fdt_بعدی_برچسب(fdt, struct_اندازه, &struct_اندازه) != FDT_انتها)
			;
		إذا (struct_اندازه < 0)
			أرجع struct_اندازه;
	} وإلا {
		أرجع -FDT_ERR_BADVERSION;
	}

	إذا (can_assume(LIBFDT_ترتیب) ||
	    !fdt_بلوکها_misordered(fdt, mem_rsv_اندازه, struct_اندازه)) {
		/* no further work necessary */
		err = fdt_move(fdt, buf, bufsize);
		إذا (err)
			أرجع err;
		fdt_مجموعه_نسخه(buf, 17);
		fdt_مجموعه_اندازه_dt_struct(buf, struct_اندازه);
		fdt_مجموعه_totalsize(buf, bufsize);
		أرجع 0;
	}

	/* Need to reorder */
	newsize = FDT_ALIGN(حجم(بنية fdt_سرآیند), 8) + mem_rsv_اندازه
		+ struct_اندازه + fdt_اندازه_dt_رشتهها(fdt);

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

	fdt_packblocks_(fdt, tmp, mem_rsv_اندازه, struct_اندازه,
			fdt_اندازه_dt_رشتهها(fdt));
	memmove(buf, tmp, newsize);

	fdt_مجموعه_magic(buf, FDT_MAGIC);
	fdt_مجموعه_totalsize(buf, bufsize);
	fdt_مجموعه_نسخه(buf, 17);
	fdt_مجموعه_آخرین_comp_نسخه(buf, 16);
	fdt_مجموعه_boot_cpuid_phys(buf, fdt_boot_cpuid_phys(fdt));

	أرجع 0;
}

صحيح fdt_pack(فراغ *fdt)
{
	صحيح mem_rsv_اندازه;

	FDT_RW_PROBE(fdt);

	mem_rsv_اندازه = (fdt_رقم_mem_rsv(fdt)+1)
		* حجم(بنية fdt_reserve_مدخل);
	fdt_packblocks_(fdt, fdt, mem_rsv_اندازه, fdt_اندازه_dt_struct(fdt),
			fdt_اندازه_dt_رشتهها(fdt));
	fdt_مجموعه_totalsize(fdt, fdt_داده_اندازه(fdt));

	أرجع 0;
}
