// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#ضمن "libfdt_env.h"

#ضمن <fdt.h>
#ضمن <libfdt.h>

#ضمن "libfdt_internal.h"

ساكن صحيح fdt_nodename_eq_(ثابت فراغ *fdt, صحيح جابه_جایی,
			    ثابت حرف *s, صحيح len)
{
	صحيح olen;
	ثابت حرف *p = fdt_get_name(fdt, جابه_جایی, &olen);

	إذا (!p || olen < len)
		/* short match */
		أرجع 0;

	إذا (memcmp(p, s, len) != 0)
		أرجع 0;

	إذا (p[len] == '\0')
		أرجع 1;
	وإلا إذا (!memchr(s, '@', len) && (p[len] == '@'))
		أرجع 1;
	وإلا
		أرجع 0;
}

ثابت حرف *fdt_get_string(ثابت فراغ *fdt, صحيح stroffset, صحيح *lenp)
{
	int32_t totalsize;
	uint32_t absoffset;
	size_t len;
	صحيح err;
	ثابت حرف *s, *n;

	إذا (can_assume(VALID_درون_داد)) {
		s = (ثابت حرف *)fdt + fdt_off_dt_رشتهها(fdt) + stroffset;

		إذا (lenp)
			*lenp = طول_رشته(s);
		أرجع s;
	}
	totalsize = fdt_ro_probe_(fdt);
	err = totalsize;
	إذا (totalsize < 0)
		اذهب fail;

	err = -FDT_ERR_BADOFFSET;
	absoffset = stroffset + fdt_off_dt_رشتهها(fdt);
	إذا (absoffset >= (بیعلامت)totalsize)
		اذهب fail;
	len = totalsize - absoffset;

	إذا (fdt_magic(fdt) == FDT_MAGIC) {
		إذا (stroffset < 0)
			اذهب fail;
		إذا (can_assume(LATEST) || fdt_نسخه(fdt) >= 17) {
			إذا ((بیعلامت)stroffset >= fdt_اندازه_dt_رشتهها(fdt))
				اذهب fail;
			إذا ((fdt_اندازه_dt_رشتهها(fdt) - stroffset) < len)
				len = fdt_اندازه_dt_رشتهها(fdt) - stroffset;
		}
	} وإلا إذا (fdt_magic(fdt) == FDT_SW_MAGIC) {
		بیعلامت صحيح sw_stroffset = -stroffset;

		إذا ((stroffset >= 0) ||
		    (sw_stroffset > fdt_اندازه_dt_رشتهها(fdt)))
			اذهب fail;
		إذا (sw_stroffset < len)
			len = sw_stroffset;
	} وإلا {
		err = -FDT_ERR_داخلی;
		اذهب fail;
	}

	s = (ثابت حرف *)fdt + absoffset;
	n = memchr(s, '\0', len);
	إذا (!n) {
		/* missing terminating NULL */
		err = -FDT_ERR_TRUNCATED;
		اذهب fail;
	}

	إذا (lenp)
		*lenp = n - s;
	أرجع s;

fail:
	إذا (lenp)
		*lenp = err;
	أرجع عدم;
}

ثابت حرف *fdt_string(ثابت فراغ *fdt, صحيح stroffset)
{
	أرجع fdt_get_string(fdt, stroffset, عدم);
}

ساكن صحيح fdt_string_eq_(ثابت فراغ *fdt, صحيح stroffset,
			  ثابت حرف *s, صحيح len)
{
	صحيح slen;
	ثابت حرف *p = fdt_get_string(fdt, stroffset, &slen);

	أرجع p && (slen == len) && (memcmp(p, s, len) == 0);
}

صحيح fdt_پیدا_کن_max_phandle(ثابت فراغ *fdt, uint32_t *phandle)
{
	uint32_t max = 0;
	صحيح جابه_جایی = -1;

	طالما (true) {
		uint32_t مقدار;

		جابه_جایی = fdt_بعدی_گره(fdt, جابه_جایی, عدم);
		إذا (جابه_جایی < 0) {
			إذا (جابه_جایی == -FDT_ERR_NOTFOUND)
				اكسر;

			أرجع جابه_جایی;
		}

		مقدار = fdt_get_phandle(fdt, جابه_جایی);

		إذا (مقدار > max)
			max = مقدار;
	}

	إذا (phandle)
		*phandle = max;

	أرجع 0;
}

صحيح fdt_generate_phandle(ثابت فراغ *fdt, uint32_t *phandle)
{
	uint32_t max;
	صحيح err;

	err = fdt_پیدا_کن_max_phandle(fdt, &max);
	إذا (err < 0)
		أرجع err;

	إذا (max == FDT_MAX_PHANDLE)
		أرجع -FDT_ERR_NOPHANDLES;

	إذا (phandle)
		*phandle = max + 1;

	أرجع 0;
}

ساكن ثابت بنية fdt_reserve_مدخل *fdt_mem_rsv(ثابت فراغ *fdt, صحيح n)
{
	بیعلامت صحيح جابه_جایی = n * حجم(بنية fdt_reserve_مدخل);
	بیعلامت صحيح absoffset = fdt_off_mem_rsvmap(fdt) + جابه_جایی;

	إذا (!can_assume(VALID_درون_داد)) {
		إذا (absoffset < fdt_off_mem_rsvmap(fdt))
			أرجع عدم;
		إذا (absoffset > fdt_totalsize(fdt) -
		    حجم(بنية fdt_reserve_مدخل))
			أرجع عدم;
	}
	أرجع fdt_mem_rsv_(fdt, n);
}

صحيح fdt_get_mem_rsv(ثابت فراغ *fdt, صحيح n, uint64_t *آدرس, uint64_t *اندازه)
{
	ثابت بنية fdt_reserve_مدخل *re;

	FDT_RO_PROBE(fdt);
	re = fdt_mem_rsv(fdt, n);
	إذا (!can_assume(VALID_درون_داد) && !re)
		أرجع -FDT_ERR_BADOFFSET;

	*آدرس = fdt64_ld_(&re->آدرس);
	*اندازه = fdt64_ld_(&re->اندازه);
	أرجع 0;
}

صحيح fdt_رقم_mem_rsv(ثابت فراغ *fdt)
{
	صحيح i;
	ثابت بنية fdt_reserve_مدخل *re;

	لكل (i = 0; (re = fdt_mem_rsv(fdt, i)) != عدم; i++) {
		إذا (fdt64_ld_(&re->اندازه) == 0)
			أرجع i;
	}
	أرجع -FDT_ERR_TRUNCATED;
}

ساكن صحيح nextprop_(ثابت فراغ *fdt, صحيح جابه_جایی)
{
	uint32_t برچسب;
	صحيح nextoffset;

	افعل {
		برچسب = fdt_بعدی_برچسب(fdt, جابه_جایی, &nextoffset);

		بدل (برچسب) {
		حالة FDT_انتها:
			إذا (nextoffset >= 0)
				أرجع -FDT_ERR_BADSTRUCTURE;
			وإلا
				أرجع nextoffset;

		حالة FDT_PROP:
			أرجع جابه_جایی;
		}
		جابه_جایی = nextoffset;
	} طالما (برچسب == FDT_NOP);

	أرجع -FDT_ERR_NOTFOUND;
}

صحيح fdt_subnode_جابه_جایی_namelen(ثابت فراغ *fdt, صحيح جابه_جایی,
			       ثابت حرف *name, صحيح namelen)
{
	صحيح عمق;

	FDT_RO_PROBE(fdt);

	لكل (عمق = 0;
	     (جابه_جایی >= 0) && (عمق >= 0);
	     جابه_جایی = fdt_بعدی_گره(fdt, جابه_جایی, &عمق))
		إذا ((عمق == 1)
		    && fdt_nodename_eq_(fdt, جابه_جایی, name, namelen))
			أرجع جابه_جایی;

	إذا (عمق < 0)
		أرجع -FDT_ERR_NOTFOUND;
	أرجع جابه_جایی; /* error */
}

صحيح fdt_subnode_جابه_جایی(ثابت فراغ *fdt, صحيح parentoffset,
		       ثابت حرف *name)
{
	أرجع fdt_subnode_جابه_جایی_namelen(fdt, parentoffset, name, طول_رشته(name));
}

صحيح fdt_مسیر_جابه_جایی_namelen(ثابت فراغ *fdt, ثابت حرف *مسیر, صحيح namelen)
{
	ثابت حرف *انتها = مسیر + namelen;
	ثابت حرف *p = مسیر;
	صحيح جابه_جایی = 0;

	FDT_RO_PROBE(fdt);

	/* see if we have an alias */
	إذا (*مسیر != '/') {
		ثابت حرف *q = memchr(مسیر, '/', انتها - p);

		إذا (!q)
			q = انتها;

		p = fdt_get_alias_namelen(fdt, p, q - p);
		إذا (!p)
			أرجع -FDT_ERR_BADPATH;
		جابه_جایی = fdt_مسیر_جابه_جایی(fdt, p);

		p = q;
	}

	طالما (p < انتها) {
		ثابت حرف *q;

		طالما (*p == '/') {
			p++;
			إذا (p == انتها)
				أرجع جابه_جایی;
		}
		q = memchr(p, '/', انتها - p);
		إذا (! q)
			q = انتها;

		جابه_جایی = fdt_subnode_جابه_جایی_namelen(fdt, جابه_جایی, p, q-p);
		إذا (جابه_جایی < 0)
			أرجع جابه_جایی;

		p = q;
	}

	أرجع جابه_جایی;
}

صحيح fdt_مسیر_جابه_جایی(ثابت فراغ *fdt, ثابت حرف *مسیر)
{
	أرجع fdt_مسیر_جابه_جایی_namelen(fdt, مسیر, طول_رشته(مسیر));
}

ثابت حرف *fdt_get_name(ثابت فراغ *fdt, صحيح nodeoffset, صحيح *len)
{
	ثابت بنية fdt_گره_سرآیند *nh = fdt_جابه_جایی_ptr_2(fdt, nodeoffset);
	ثابت حرف *nameptr;
	صحيح err;

	إذا (((err = fdt_ro_probe_(fdt)) < 0)
	    || ((err = fdt_check_گره_جابه_جایی(fdt, nodeoffset)) < 0))
			اذهب fail;

	nameptr = nh->name;

	إذا (!can_assume(LATEST) && fdt_نسخه(fdt) < 0x10) {
		/*
		 * For old FDT versions, match the naming conventions of V16:
		 * give only the leaf name (after all /). The actual tree
		 * contents are loosely checked.
		 */
		ثابت حرف *برگ;
		برگ = strrchr(nameptr, '/');
		إذا (برگ == عدم) {
			err = -FDT_ERR_BADSTRUCTURE;
			اذهب fail;
		}
		nameptr = برگ+1;
	}

	إذا (len)
		*len = طول_رشته(nameptr);

	أرجع nameptr;

 fail:
	إذا (len)
		*len = err;
	أرجع عدم;
}

صحيح fdt_اولی_property_جابه_جایی(ثابت فراغ *fdt, صحيح nodeoffset)
{
	صحيح جابه_جایی;

	إذا ((جابه_جایی = fdt_check_گره_جابه_جایی(fdt, nodeoffset)) < 0)
		أرجع جابه_جایی;

	أرجع nextprop_(fdt, جابه_جایی);
}

صحيح fdt_بعدی_property_جابه_جایی(ثابت فراغ *fdt, صحيح جابه_جایی)
{
	إذا ((جابه_جایی = fdt_check_prop_جابه_جایی(fdt, جابه_جایی)) < 0)
		أرجع جابه_جایی;

	أرجع nextprop_(fdt, جابه_جایی);
}

ساكن ثابت بنية fdt_property *fdt_get_property_by_جابه_جایی(ثابت فراغ *fdt,
						              صحيح جابه_جایی,
						              صحيح *lenp)
{
	صحيح err;
	ثابت بنية fdt_property *prop;

	إذا (!can_assume(VALID_درون_داد) &&
	    (err = fdt_check_prop_جابه_جایی(fdt, جابه_جایی)) < 0) {
		إذا (lenp)
			*lenp = err;
		أرجع عدم;
	}

	prop = fdt_جابه_جایی_ptr_2(fdt, جابه_جایی);

	إذا (lenp)
		*lenp = fdt32_ld_(&prop->len);

	أرجع prop;
}

ثابت بنية fdt_property *fdt_get_property_by_جابه_جایی_2(ثابت فراغ *fdt,
						      صحيح جابه_جایی,
						      صحيح *lenp)
{
	/* Prior to version 16, properties may need realignment
	 * and this API does not work. fdt_getprop_*() will, however. */

	إذا (!can_assume(LATEST) && fdt_نسخه(fdt) < 0x10) {
		إذا (lenp)
			*lenp = -FDT_ERR_BADVERSION;
		أرجع عدم;
	}

	أرجع fdt_get_property_by_جابه_جایی(fdt, جابه_جایی, lenp);
}

ساكن ثابت بنية fdt_property *fdt_get_property_namelen_(ثابت فراغ *fdt,
						            صحيح جابه_جایی,
						            ثابت حرف *name,
						            صحيح namelen,
							    صحيح *lenp,
							    صحيح *poffset)
{
	لكل (جابه_جایی = fdt_اولی_property_جابه_جایی(fdt, جابه_جایی);
	     (جابه_جایی >= 0);
	     (جابه_جایی = fdt_بعدی_property_جابه_جایی(fdt, جابه_جایی))) {
		ثابت بنية fdt_property *prop;

		prop = fdt_get_property_by_جابه_جایی(fdt, جابه_جایی, lenp);
		إذا (!can_assume(LIBFDT_FLAWLESS) && !prop) {
			جابه_جایی = -FDT_ERR_داخلی;
			اكسر;
		}
		إذا (fdt_string_eq_(fdt, fdt32_ld_(&prop->nameoff),
				   name, namelen)) {
			إذا (poffset)
				*poffset = جابه_جایی;
			أرجع prop;
		}
	}

	إذا (lenp)
		*lenp = جابه_جایی;
	أرجع عدم;
}


ثابت بنية fdt_property *fdt_get_property_namelen(ثابت فراغ *fdt,
						    صحيح جابه_جایی,
						    ثابت حرف *name,
						    صحيح namelen, صحيح *lenp)
{
	/* Prior to version 16, properties may need realignment
	 * and this API does not work. fdt_getprop_*() will, however. */
	إذا (!can_assume(LATEST) && fdt_نسخه(fdt) < 0x10) {
		إذا (lenp)
			*lenp = -FDT_ERR_BADVERSION;
		أرجع عدم;
	}

	أرجع fdt_get_property_namelen_(fdt, جابه_جایی, name, namelen, lenp,
					 عدم);
}


ثابت بنية fdt_property *fdt_get_property(ثابت فراغ *fdt,
					    صحيح nodeoffset,
					    ثابت حرف *name, صحيح *lenp)
{
	أرجع fdt_get_property_namelen(fdt, nodeoffset, name,
					طول_رشته(name), lenp);
}

ثابت فراغ *fdt_getprop_namelen(ثابت فراغ *fdt, صحيح nodeoffset,
				ثابت حرف *name, صحيح namelen, صحيح *lenp)
{
	صحيح poffset;
	ثابت بنية fdt_property *prop;

	prop = fdt_get_property_namelen_(fdt, nodeoffset, name, namelen, lenp,
					 &poffset);
	إذا (!prop)
		أرجع عدم;

	/* Handle realignment */
	إذا (!can_assume(LATEST) && fdt_نسخه(fdt) < 0x10 &&
	    (poffset + حجم(*prop)) % 8 && fdt32_ld_(&prop->len) >= 8)
		أرجع prop->داده + 4;
	أرجع prop->داده;
}

ثابت فراغ *fdt_getprop_by_جابه_جایی(ثابت فراغ *fdt, صحيح جابه_جایی,
				  ثابت حرف **namep, صحيح *lenp)
{
	ثابت بنية fdt_property *prop;

	prop = fdt_get_property_by_جابه_جایی(fdt, جابه_جایی, lenp);
	إذا (!prop)
		أرجع عدم;
	إذا (namep) {
		ثابت حرف *name;
		صحيح namelen;

		إذا (!can_assume(VALID_درون_داد)) {
			name = fdt_get_string(fdt, fdt32_ld_(&prop->nameoff),
					      &namelen);
			إذا (!name) {
				إذا (lenp)
					*lenp = namelen;
				أرجع عدم;
			}
			*namep = name;
		} وإلا {
			*namep = fdt_string(fdt, fdt32_ld_(&prop->nameoff));
		}
	}

	/* Handle realignment */
	إذا (!can_assume(LATEST) && fdt_نسخه(fdt) < 0x10 &&
	    (جابه_جایی + حجم(*prop)) % 8 && fdt32_ld_(&prop->len) >= 8)
		أرجع prop->داده + 4;
	أرجع prop->داده;
}

ثابت فراغ *fdt_getprop(ثابت فراغ *fdt, صحيح nodeoffset,
			ثابت حرف *name, صحيح *lenp)
{
	أرجع fdt_getprop_namelen(fdt, nodeoffset, name, طول_رشته(name), lenp);
}

uint32_t fdt_get_phandle(ثابت فراغ *fdt, صحيح nodeoffset)
{
	ثابت fdt32_t *php;
	صحيح len;

	/* FIXME: This is a bit sub-optimal, since we potentially scan
	 * over all the properties twice. */
	php = fdt_getprop(fdt, nodeoffset, "phandle", &len);
	إذا (!php || (len != حجم(*php))) {
		php = fdt_getprop(fdt, nodeoffset, "linux,phandle", &len);
		إذا (!php || (len != حجم(*php)))
			أرجع 0;
	}

	أرجع fdt32_ld_(php);
}

ثابت حرف *fdt_get_alias_namelen(ثابت فراغ *fdt,
				  ثابت حرف *name, صحيح namelen)
{
	صحيح aliasoffset;

	aliasoffset = fdt_مسیر_جابه_جایی(fdt, "/aliases");
	إذا (aliasoffset < 0)
		أرجع عدم;

	أرجع fdt_getprop_namelen(fdt, aliasoffset, name, namelen, عدم);
}

ثابت حرف *fdt_get_alias(ثابت فراغ *fdt, ثابت حرف *name)
{
	أرجع fdt_get_alias_namelen(fdt, name, طول_رشته(name));
}

صحيح fdt_get_مسیر(ثابت فراغ *fdt, صحيح nodeoffset, حرف *buf, صحيح buflen)
{
	صحيح pdepth = 0, p = 0;
	صحيح جابه_جایی, عمق, namelen;
	ثابت حرف *name;

	FDT_RO_PROBE(fdt);

	إذا (buflen < 2)
		أرجع -FDT_ERR_NOSPACE;

	لكل (جابه_جایی = 0, عمق = 0;
	     (جابه_جایی >= 0) && (جابه_جایی <= nodeoffset);
	     جابه_جایی = fdt_بعدی_گره(fdt, جابه_جایی, &عمق)) {
		طالما (pdepth > عمق) {
			افعل {
				p--;
			} طالما (buf[p-1] != '/');
			pdepth--;
		}

		إذا (pdepth >= عمق) {
			name = fdt_get_name(fdt, جابه_جایی, &namelen);
			إذا (!name)
				أرجع namelen;
			إذا ((p + namelen + 1) <= buflen) {
				کپی_حافظه(buf + p, name, namelen);
				p += namelen;
				buf[p++] = '/';
				pdepth++;
			}
		}

		إذا (جابه_جایی == nodeoffset) {
			إذا (pdepth < (عمق + 1))
				أرجع -FDT_ERR_NOSPACE;

			إذا (p > 1) /* special case so that root path is "/", not "" */
				p--;
			buf[p] = '\0';
			أرجع 0;
		}
	}

	إذا ((جابه_جایی == -FDT_ERR_NOTFOUND) || (جابه_جایی >= 0))
		أرجع -FDT_ERR_BADOFFSET;
	وإلا إذا (جابه_جایی == -FDT_ERR_BADOFFSET)
		أرجع -FDT_ERR_BADSTRUCTURE;

	أرجع جابه_جایی; /* error from fdt_next_node() */
}

صحيح fdt_supernode_atdepth_جابه_جایی(ثابت فراغ *fdt, صحيح nodeoffset,
				 صحيح supernodedepth, صحيح *nodedepth)
{
	صحيح جابه_جایی, عمق;
	صحيح supernodeoffset = -FDT_ERR_داخلی;

	FDT_RO_PROBE(fdt);

	إذا (supernodedepth < 0)
		أرجع -FDT_ERR_NOTFOUND;

	لكل (جابه_جایی = 0, عمق = 0;
	     (جابه_جایی >= 0) && (جابه_جایی <= nodeoffset);
	     جابه_جایی = fdt_بعدی_گره(fdt, جابه_جایی, &عمق)) {
		إذا (عمق == supernodedepth)
			supernodeoffset = جابه_جایی;

		إذا (جابه_جایی == nodeoffset) {
			إذا (nodedepth)
				*nodedepth = عمق;

			إذا (supernodedepth > عمق)
				أرجع -FDT_ERR_NOTFOUND;
			وإلا
				أرجع supernodeoffset;
		}
	}

	إذا (!can_assume(VALID_درون_داد)) {
		إذا ((جابه_جایی == -FDT_ERR_NOTFOUND) || (جابه_جایی >= 0))
			أرجع -FDT_ERR_BADOFFSET;
		وإلا إذا (جابه_جایی == -FDT_ERR_BADOFFSET)
			أرجع -FDT_ERR_BADSTRUCTURE;
	}

	أرجع جابه_جایی; /* error from fdt_next_node() */
}

صحيح fdt_گره_عمق(ثابت فراغ *fdt, صحيح nodeoffset)
{
	صحيح nodedepth;
	صحيح err;

	err = fdt_supernode_atdepth_جابه_جایی(fdt, nodeoffset, 0, &nodedepth);
	إذا (err)
		أرجع (can_assume(LIBFDT_FLAWLESS) || err < 0) ? err :
			-FDT_ERR_داخلی;
	أرجع nodedepth;
}

صحيح fdt_والد_جابه_جایی(ثابت فراغ *fdt, صحيح nodeoffset)
{
	صحيح nodedepth = fdt_گره_عمق(fdt, nodeoffset);

	إذا (nodedepth < 0)
		أرجع nodedepth;
	أرجع fdt_supernode_atdepth_جابه_جایی(fdt, nodeoffset,
					    nodedepth - 1, عدم);
}

صحيح fdt_گره_جابه_جایی_by_prop_مقدار(ثابت فراغ *fdt, صحيح startoffset,
				  ثابت حرف *propname,
				  ثابت فراغ *propval, صحيح proplen)
{
	صحيح جابه_جایی;
	ثابت فراغ *val;
	صحيح len;

	FDT_RO_PROBE(fdt);

	/* FIXME: The algorithm here is pretty horrible: we scan each
	 * property of a node in fdt_getprop(), then if that didn't
	 * find what we want, we scan over them again making our way
	 * to the next node.  Still it's the easiest to implement
	 * approach; performance can come later. */
	لكل (جابه_جایی = fdt_بعدی_گره(fdt, startoffset, عدم);
	     جابه_جایی >= 0;
	     جابه_جایی = fdt_بعدی_گره(fdt, جابه_جایی, عدم)) {
		val = fdt_getprop(fdt, جابه_جایی, propname, &len);
		إذا (val && (len == proplen)
		    && (memcmp(val, propval, len) == 0))
			أرجع جابه_جایی;
	}

	أرجع جابه_جایی; /* error from fdt_next_node() */
}

صحيح fdt_گره_جابه_جایی_by_phandle(ثابت فراغ *fdt, uint32_t phandle)
{
	صحيح جابه_جایی;

	إذا ((phandle == 0) || (phandle == ~0U))
		أرجع -FDT_ERR_BADPHANDLE;

	FDT_RO_PROBE(fdt);

	/* FIXME: The algorithm here is pretty horrible: we
	 * potentially scan each property of a node in
	 * fdt_get_phandle(), then if that didn't find what
	 * we want, we scan over them again making our way to the next
	 * node.  Still it's the easiest to implement approach;
	 * performance can come later. */
	لكل (جابه_جایی = fdt_بعدی_گره(fdt, -1, عدم);
	     جابه_جایی >= 0;
	     جابه_جایی = fdt_بعدی_گره(fdt, جابه_جایی, عدم)) {
		إذا (fdt_get_phandle(fdt, جابه_جایی) == phandle)
			أرجع جابه_جایی;
	}

	أرجع جابه_جایی; /* error from fdt_next_node() */
}

صحيح fdt_stringlist_شامل_است(ثابت حرف *strlist, صحيح listlen, ثابت حرف *str)
{
	صحيح len = طول_رشته(str);
	ثابت حرف *p;

	طالما (listlen >= len) {
		إذا (memcmp(str, strlist, len+1) == 0)
			أرجع 1;
		p = memchr(strlist, '\0', listlen);
		إذا (!p)
			أرجع 0; /* malformed strlist.. */
		listlen -= (p-strlist) + 1;
		strlist = p + 1;
	}
	أرجع 0;
}

صحيح fdt_stringlist_شمار(ثابت فراغ *fdt, صحيح nodeoffset, ثابت حرف *property)
{
	ثابت حرف *list, *انتها;
	صحيح درازی, شمار = 0;

	list = fdt_getprop(fdt, nodeoffset, property, &درازی);
	إذا (!list)
		أرجع درازی;

	انتها = list + درازی;

	طالما (list < انتها) {
		درازی = strnlen(list, انتها - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		إذا (list + درازی > انتها)
			أرجع -FDT_ERR_BADVALUE;

		list += درازی;
		شمار++;
	}

	أرجع شمار;
}

صحيح fdt_stringlist_جستجو(ثابت فراغ *fdt, صحيح nodeoffset, ثابت حرف *property,
			  ثابت حرف *string)
{
	صحيح درازی, len, idx = 0;
	ثابت حرف *list, *انتها;

	list = fdt_getprop(fdt, nodeoffset, property, &درازی);
	إذا (!list)
		أرجع درازی;

	len = طول_رشته(string) + 1;
	انتها = list + درازی;

	طالما (list < انتها) {
		درازی = strnlen(list, انتها - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		إذا (list + درازی > انتها)
			أرجع -FDT_ERR_BADVALUE;

		إذا (درازی == len && memcmp(list, string, درازی) == 0)
			أرجع idx;

		list += درازی;
		idx++;
	}

	أرجع -FDT_ERR_NOTFOUND;
}

ثابت حرف *fdt_stringlist_get(ثابت فراغ *fdt, صحيح nodeoffset,
			       ثابت حرف *property, صحيح idx,
			       صحيح *lenp)
{
	ثابت حرف *list, *انتها;
	صحيح درازی;

	list = fdt_getprop(fdt, nodeoffset, property, &درازی);
	إذا (!list) {
		إذا (lenp)
			*lenp = درازی;

		أرجع عدم;
	}

	انتها = list + درازی;

	طالما (list < انتها) {
		درازی = strnlen(list, انتها - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		إذا (list + درازی > انتها) {
			إذا (lenp)
				*lenp = -FDT_ERR_BADVALUE;

			أرجع عدم;
		}

		إذا (idx == 0) {
			إذا (lenp)
				*lenp = درازی - 1;

			أرجع list;
		}

		list += درازی;
		idx--;
	}

	إذا (lenp)
		*lenp = -FDT_ERR_NOTFOUND;

	أرجع عدم;
}

صحيح fdt_گره_check_compatible(ثابت فراغ *fdt, صحيح nodeoffset,
			      ثابت حرف *compatible)
{
	ثابت فراغ *prop;
	صحيح len;

	prop = fdt_getprop(fdt, nodeoffset, "compatible", &len);
	إذا (!prop)
		أرجع len;

	أرجع !fdt_stringlist_شامل_است(prop, len, compatible);
}

صحيح fdt_گره_جابه_جایی_by_compatible(ثابت فراغ *fdt, صحيح startoffset,
				  ثابت حرف *compatible)
{
	صحيح جابه_جایی, err;

	FDT_RO_PROBE(fdt);

	/* FIXME: The algorithm here is pretty horrible: we scan each
	 * property of a node in fdt_node_check_compatible(), then if
	 * that didn't find what we want, we scan over them again
	 * making our way to the next node.  Still it's the easiest to
	 * implement approach; performance can come later. */
	لكل (جابه_جایی = fdt_بعدی_گره(fdt, startoffset, عدم);
	     جابه_جایی >= 0;
	     جابه_جایی = fdt_بعدی_گره(fdt, جابه_جایی, عدم)) {
		err = fdt_گره_check_compatible(fdt, جابه_جایی, compatible);
		إذا ((err < 0) && (err != -FDT_ERR_NOTFOUND))
			أرجع err;
		وإلا إذا (err == 0)
			أرجع جابه_جایی;
	}

	أرجع جابه_جایی; /* error from fdt_next_node() */
}
