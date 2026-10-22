// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#ضمن "libfdt_env.h"

#ضمن <fdt.h>
#ضمن <libfdt.h>

#ضمن "libfdt_internal.h"

ساكن صحيح fdt_nodename_eq_(ثابت فراغ *fdt, صحيح وٿي,
			    ثابت حرف *s, صحيح len)
{
	صحيح olen;
	ثابت حرف *p = fdt_get_name(fdt, وٿي, &olen);

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

	إذا (can_assume(VALID_داخلا)) {
		s = (ثابت حرف *)fdt + fdt_off_dt_لڙيون(fdt) + stroffset;

		إذا (lenp)
			*lenp = لڙي_ڊيگهه(s);
		أرجع s;
	}
	totalsize = fdt_ro_probe_(fdt);
	err = totalsize;
	إذا (totalsize < 0)
		اذهب fail;

	err = -FDT_ERR_BADOFFSET;
	absoffset = stroffset + fdt_off_dt_لڙيون(fdt);
	إذا (absoffset >= (بنانشان)totalsize)
		اذهب fail;
	len = totalsize - absoffset;

	إذا (fdt_magic(fdt) == FDT_MAGIC) {
		إذا (stroffset < 0)
			اذهب fail;
		إذا (can_assume(LATEST) || fdt_نسخو(fdt) >= 17) {
			إذا ((بنانشان)stroffset >= fdt_سائيز_dt_لڙيون(fdt))
				اذهب fail;
			إذا ((fdt_سائيز_dt_لڙيون(fdt) - stroffset) < len)
				len = fdt_سائيز_dt_لڙيون(fdt) - stroffset;
		}
	} وإلا إذا (fdt_magic(fdt) == FDT_SW_MAGIC) {
		بنانشان صحيح sw_stroffset = -stroffset;

		إذا ((stroffset >= 0) ||
		    (sw_stroffset > fdt_سائيز_dt_لڙيون(fdt)))
			اذهب fail;
		إذا (sw_stroffset < len)
			len = sw_stroffset;
	} وإلا {
		err = -FDT_ERR_اندروني;
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

صحيح fdt_ڳوليو_max_phandle(ثابت فراغ *fdt, uint32_t *phandle)
{
	uint32_t max = 0;
	صحيح وٿي = -1;

	طالما (true) {
		uint32_t قدر;

		وٿي = fdt_ايندڙ_ڳنڍ(fdt, وٿي, عدم);
		إذا (وٿي < 0) {
			إذا (وٿي == -FDT_ERR_NOTFOUND)
				اكسر;

			أرجع وٿي;
		}

		قدر = fdt_get_phandle(fdt, وٿي);

		إذا (قدر > max)
			max = قدر;
	}

	إذا (phandle)
		*phandle = max;

	أرجع 0;
}

صحيح fdt_generate_phandle(ثابت فراغ *fdt, uint32_t *phandle)
{
	uint32_t max;
	صحيح err;

	err = fdt_ڳوليو_max_phandle(fdt, &max);
	إذا (err < 0)
		أرجع err;

	إذا (max == FDT_MAX_PHANDLE)
		أرجع -FDT_ERR_NOPHANDLES;

	إذا (phandle)
		*phandle = max + 1;

	أرجع 0;
}

ساكن ثابت بنية fdt_reserve_داخلا *fdt_mem_rsv(ثابت فراغ *fdt, صحيح n)
{
	بنانشان صحيح وٿي = n * حجم(بنية fdt_reserve_داخلا);
	بنانشان صحيح absoffset = fdt_off_mem_rsvmap(fdt) + وٿي;

	إذا (!can_assume(VALID_داخلا)) {
		إذا (absoffset < fdt_off_mem_rsvmap(fdt))
			أرجع عدم;
		إذا (absoffset > fdt_totalsize(fdt) -
		    حجم(بنية fdt_reserve_داخلا))
			أرجع عدم;
	}
	أرجع fdt_mem_rsv_(fdt, n);
}

صحيح fdt_get_mem_rsv(ثابت فراغ *fdt, صحيح n, uint64_t *پتو, uint64_t *سائيز)
{
	ثابت بنية fdt_reserve_داخلا *re;

	FDT_RO_PROBE(fdt);
	re = fdt_mem_rsv(fdt, n);
	إذا (!can_assume(VALID_داخلا) && !re)
		أرجع -FDT_ERR_BADOFFSET;

	*پتو = fdt64_ld_(&re->پتو);
	*سائيز = fdt64_ld_(&re->سائيز);
	أرجع 0;
}

صحيح fdt_انگ_mem_rsv(ثابت فراغ *fdt)
{
	صحيح i;
	ثابت بنية fdt_reserve_داخلا *re;

	لكل (i = 0; (re = fdt_mem_rsv(fdt, i)) != عدم; i++) {
		إذا (fdt64_ld_(&re->سائيز) == 0)
			أرجع i;
	}
	أرجع -FDT_ERR_TRUNCATED;
}

ساكن صحيح nextprop_(ثابت فراغ *fdt, صحيح وٿي)
{
	uint32_t ٽيگ;
	صحيح nextoffset;

	افعل {
		ٽيگ = fdt_ايندڙ_ٽيگ(fdt, وٿي, &nextoffset);

		بدل (ٽيگ) {
		حالة FDT_پڇاڙي:
			إذا (nextoffset >= 0)
				أرجع -FDT_ERR_BADSTRUCTURE;
			وإلا
				أرجع nextoffset;

		حالة FDT_PROP:
			أرجع وٿي;
		}
		وٿي = nextoffset;
	} طالما (ٽيگ == FDT_NOP);

	أرجع -FDT_ERR_NOTFOUND;
}

صحيح fdt_subnode_وٿي_namelen(ثابت فراغ *fdt, صحيح وٿي,
			       ثابت حرف *name, صحيح namelen)
{
	صحيح اونهائي;

	FDT_RO_PROBE(fdt);

	لكل (اونهائي = 0;
	     (وٿي >= 0) && (اونهائي >= 0);
	     وٿي = fdt_ايندڙ_ڳنڍ(fdt, وٿي, &اونهائي))
		إذا ((اونهائي == 1)
		    && fdt_nodename_eq_(fdt, وٿي, name, namelen))
			أرجع وٿي;

	إذا (اونهائي < 0)
		أرجع -FDT_ERR_NOTFOUND;
	أرجع وٿي; /* error */
}

صحيح fdt_subnode_وٿي(ثابت فراغ *fdt, صحيح parentoffset,
		       ثابت حرف *name)
{
	أرجع fdt_subnode_وٿي_namelen(fdt, parentoffset, name, لڙي_ڊيگهه(name));
}

صحيح fdt_رستو_وٿي_namelen(ثابت فراغ *fdt, ثابت حرف *رستو, صحيح namelen)
{
	ثابت حرف *پڇاڙي = رستو + namelen;
	ثابت حرف *p = رستو;
	صحيح وٿي = 0;

	FDT_RO_PROBE(fdt);

	/* see if we have an alias */
	إذا (*رستو != '/') {
		ثابت حرف *q = memchr(رستو, '/', پڇاڙي - p);

		إذا (!q)
			q = پڇاڙي;

		p = fdt_get_alias_namelen(fdt, p, q - p);
		إذا (!p)
			أرجع -FDT_ERR_BADPATH;
		وٿي = fdt_رستو_وٿي(fdt, p);

		p = q;
	}

	طالما (p < پڇاڙي) {
		ثابت حرف *q;

		طالما (*p == '/') {
			p++;
			إذا (p == پڇاڙي)
				أرجع وٿي;
		}
		q = memchr(p, '/', پڇاڙي - p);
		إذا (! q)
			q = پڇاڙي;

		وٿي = fdt_subnode_وٿي_namelen(fdt, وٿي, p, q-p);
		إذا (وٿي < 0)
			أرجع وٿي;

		p = q;
	}

	أرجع وٿي;
}

صحيح fdt_رستو_وٿي(ثابت فراغ *fdt, ثابت حرف *رستو)
{
	أرجع fdt_رستو_وٿي_namelen(fdt, رستو, لڙي_ڊيگهه(رستو));
}

ثابت حرف *fdt_get_name(ثابت فراغ *fdt, صحيح nodeoffset, صحيح *len)
{
	ثابت بنية fdt_ڳنڍ_سرو *nh = fdt_وٿي_ptr_2(fdt, nodeoffset);
	ثابت حرف *nameptr;
	صحيح err;

	إذا (((err = fdt_ro_probe_(fdt)) < 0)
	    || ((err = fdt_check_ڳنڍ_وٿي(fdt, nodeoffset)) < 0))
			اذهب fail;

	nameptr = nh->name;

	إذا (!can_assume(LATEST) && fdt_نسخو(fdt) < 0x10) {
		/*
		 * For old FDT versions, match the naming conventions of V16:
		 * give only the leaf name (after all /). The actual tree
		 * contents are loosely checked.
		 */
		ثابت حرف *پن;
		پن = strrchr(nameptr, '/');
		إذا (پن == عدم) {
			err = -FDT_ERR_BADSTRUCTURE;
			اذهب fail;
		}
		nameptr = پن+1;
	}

	إذا (len)
		*len = لڙي_ڊيگهه(nameptr);

	أرجع nameptr;

 fail:
	إذا (len)
		*len = err;
	أرجع عدم;
}

صحيح fdt_پهريون_property_وٿي(ثابت فراغ *fdt, صحيح nodeoffset)
{
	صحيح وٿي;

	إذا ((وٿي = fdt_check_ڳنڍ_وٿي(fdt, nodeoffset)) < 0)
		أرجع وٿي;

	أرجع nextprop_(fdt, وٿي);
}

صحيح fdt_ايندڙ_property_وٿي(ثابت فراغ *fdt, صحيح وٿي)
{
	إذا ((وٿي = fdt_check_prop_وٿي(fdt, وٿي)) < 0)
		أرجع وٿي;

	أرجع nextprop_(fdt, وٿي);
}

ساكن ثابت بنية fdt_property *fdt_get_property_by_وٿي(ثابت فراغ *fdt,
						              صحيح وٿي,
						              صحيح *lenp)
{
	صحيح err;
	ثابت بنية fdt_property *prop;

	إذا (!can_assume(VALID_داخلا) &&
	    (err = fdt_check_prop_وٿي(fdt, وٿي)) < 0) {
		إذا (lenp)
			*lenp = err;
		أرجع عدم;
	}

	prop = fdt_وٿي_ptr_2(fdt, وٿي);

	إذا (lenp)
		*lenp = fdt32_ld_(&prop->len);

	أرجع prop;
}

ثابت بنية fdt_property *fdt_get_property_by_وٿي_2(ثابت فراغ *fdt,
						      صحيح وٿي,
						      صحيح *lenp)
{
	/* Prior to version 16, properties may need realignment
	 * and this API does not work. fdt_getprop_*() will, however. */

	إذا (!can_assume(LATEST) && fdt_نسخو(fdt) < 0x10) {
		إذا (lenp)
			*lenp = -FDT_ERR_BADVERSION;
		أرجع عدم;
	}

	أرجع fdt_get_property_by_وٿي(fdt, وٿي, lenp);
}

ساكن ثابت بنية fdt_property *fdt_get_property_namelen_(ثابت فراغ *fdt,
						            صحيح وٿي,
						            ثابت حرف *name,
						            صحيح namelen,
							    صحيح *lenp,
							    صحيح *poffset)
{
	لكل (وٿي = fdt_پهريون_property_وٿي(fdt, وٿي);
	     (وٿي >= 0);
	     (وٿي = fdt_ايندڙ_property_وٿي(fdt, وٿي))) {
		ثابت بنية fdt_property *prop;

		prop = fdt_get_property_by_وٿي(fdt, وٿي, lenp);
		إذا (!can_assume(LIBFDT_FLAWLESS) && !prop) {
			وٿي = -FDT_ERR_اندروني;
			اكسر;
		}
		إذا (fdt_string_eq_(fdt, fdt32_ld_(&prop->nameoff),
				   name, namelen)) {
			إذا (poffset)
				*poffset = وٿي;
			أرجع prop;
		}
	}

	إذا (lenp)
		*lenp = وٿي;
	أرجع عدم;
}


ثابت بنية fdt_property *fdt_get_property_namelen(ثابت فراغ *fdt,
						    صحيح وٿي,
						    ثابت حرف *name,
						    صحيح namelen, صحيح *lenp)
{
	/* Prior to version 16, properties may need realignment
	 * and this API does not work. fdt_getprop_*() will, however. */
	إذا (!can_assume(LATEST) && fdt_نسخو(fdt) < 0x10) {
		إذا (lenp)
			*lenp = -FDT_ERR_BADVERSION;
		أرجع عدم;
	}

	أرجع fdt_get_property_namelen_(fdt, وٿي, name, namelen, lenp,
					 عدم);
}


ثابت بنية fdt_property *fdt_get_property(ثابت فراغ *fdt,
					    صحيح nodeoffset,
					    ثابت حرف *name, صحيح *lenp)
{
	أرجع fdt_get_property_namelen(fdt, nodeoffset, name,
					لڙي_ڊيگهه(name), lenp);
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
	إذا (!can_assume(LATEST) && fdt_نسخو(fdt) < 0x10 &&
	    (poffset + حجم(*prop)) % 8 && fdt32_ld_(&prop->len) >= 8)
		أرجع prop->معلومات + 4;
	أرجع prop->معلومات;
}

ثابت فراغ *fdt_getprop_by_وٿي(ثابت فراغ *fdt, صحيح وٿي,
				  ثابت حرف **namep, صحيح *lenp)
{
	ثابت بنية fdt_property *prop;

	prop = fdt_get_property_by_وٿي(fdt, وٿي, lenp);
	إذا (!prop)
		أرجع عدم;
	إذا (namep) {
		ثابت حرف *name;
		صحيح namelen;

		إذا (!can_assume(VALID_داخلا)) {
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
	إذا (!can_assume(LATEST) && fdt_نسخو(fdt) < 0x10 &&
	    (وٿي + حجم(*prop)) % 8 && fdt32_ld_(&prop->len) >= 8)
		أرجع prop->معلومات + 4;
	أرجع prop->معلومات;
}

ثابت فراغ *fdt_getprop(ثابت فراغ *fdt, صحيح nodeoffset,
			ثابت حرف *name, صحيح *lenp)
{
	أرجع fdt_getprop_namelen(fdt, nodeoffset, name, لڙي_ڊيگهه(name), lenp);
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

	aliasoffset = fdt_رستو_وٿي(fdt, "/aliases");
	إذا (aliasoffset < 0)
		أرجع عدم;

	أرجع fdt_getprop_namelen(fdt, aliasoffset, name, namelen, عدم);
}

ثابت حرف *fdt_get_alias(ثابت فراغ *fdt, ثابت حرف *name)
{
	أرجع fdt_get_alias_namelen(fdt, name, لڙي_ڊيگهه(name));
}

صحيح fdt_get_رستو(ثابت فراغ *fdt, صحيح nodeoffset, حرف *buf, صحيح buflen)
{
	صحيح pdepth = 0, p = 0;
	صحيح وٿي, اونهائي, namelen;
	ثابت حرف *name;

	FDT_RO_PROBE(fdt);

	إذا (buflen < 2)
		أرجع -FDT_ERR_NOSPACE;

	لكل (وٿي = 0, اونهائي = 0;
	     (وٿي >= 0) && (وٿي <= nodeoffset);
	     وٿي = fdt_ايندڙ_ڳنڍ(fdt, وٿي, &اونهائي)) {
		طالما (pdepth > اونهائي) {
			افعل {
				p--;
			} طالما (buf[p-1] != '/');
			pdepth--;
		}

		إذا (pdepth >= اونهائي) {
			name = fdt_get_name(fdt, وٿي, &namelen);
			إذا (!name)
				أرجع namelen;
			إذا ((p + namelen + 1) <= buflen) {
				يادگيري_نقل(buf + p, name, namelen);
				p += namelen;
				buf[p++] = '/';
				pdepth++;
			}
		}

		إذا (وٿي == nodeoffset) {
			إذا (pdepth < (اونهائي + 1))
				أرجع -FDT_ERR_NOSPACE;

			إذا (p > 1) /* special case so that root path is "/", not "" */
				p--;
			buf[p] = '\0';
			أرجع 0;
		}
	}

	إذا ((وٿي == -FDT_ERR_NOTFOUND) || (وٿي >= 0))
		أرجع -FDT_ERR_BADOFFSET;
	وإلا إذا (وٿي == -FDT_ERR_BADOFFSET)
		أرجع -FDT_ERR_BADSTRUCTURE;

	أرجع وٿي; /* error from fdt_next_node() */
}

صحيح fdt_supernode_atdepth_وٿي(ثابت فراغ *fdt, صحيح nodeoffset,
				 صحيح supernodedepth, صحيح *nodedepth)
{
	صحيح وٿي, اونهائي;
	صحيح supernodeoffset = -FDT_ERR_اندروني;

	FDT_RO_PROBE(fdt);

	إذا (supernodedepth < 0)
		أرجع -FDT_ERR_NOTFOUND;

	لكل (وٿي = 0, اونهائي = 0;
	     (وٿي >= 0) && (وٿي <= nodeoffset);
	     وٿي = fdt_ايندڙ_ڳنڍ(fdt, وٿي, &اونهائي)) {
		إذا (اونهائي == supernodedepth)
			supernodeoffset = وٿي;

		إذا (وٿي == nodeoffset) {
			إذا (nodedepth)
				*nodedepth = اونهائي;

			إذا (supernodedepth > اونهائي)
				أرجع -FDT_ERR_NOTFOUND;
			وإلا
				أرجع supernodeoffset;
		}
	}

	إذا (!can_assume(VALID_داخلا)) {
		إذا ((وٿي == -FDT_ERR_NOTFOUND) || (وٿي >= 0))
			أرجع -FDT_ERR_BADOFFSET;
		وإلا إذا (وٿي == -FDT_ERR_BADOFFSET)
			أرجع -FDT_ERR_BADSTRUCTURE;
	}

	أرجع وٿي; /* error from fdt_next_node() */
}

صحيح fdt_ڳنڍ_اونهائي(ثابت فراغ *fdt, صحيح nodeoffset)
{
	صحيح nodedepth;
	صحيح err;

	err = fdt_supernode_atdepth_وٿي(fdt, nodeoffset, 0, &nodedepth);
	إذا (err)
		أرجع (can_assume(LIBFDT_FLAWLESS) || err < 0) ? err :
			-FDT_ERR_اندروني;
	أرجع nodedepth;
}

صحيح fdt_والد_وٿي(ثابت فراغ *fdt, صحيح nodeoffset)
{
	صحيح nodedepth = fdt_ڳنڍ_اونهائي(fdt, nodeoffset);

	إذا (nodedepth < 0)
		أرجع nodedepth;
	أرجع fdt_supernode_atdepth_وٿي(fdt, nodeoffset,
					    nodedepth - 1, عدم);
}

صحيح fdt_ڳنڍ_وٿي_by_prop_قدر(ثابت فراغ *fdt, صحيح startoffset,
				  ثابت حرف *propname,
				  ثابت فراغ *propval, صحيح proplen)
{
	صحيح وٿي;
	ثابت فراغ *val;
	صحيح len;

	FDT_RO_PROBE(fdt);

	/* FIXME: The algorithm here is pretty horrible: we scan each
	 * property of a node in fdt_getprop(), then if that didn't
	 * find what we want, we scan over them again making our way
	 * to the next node.  Still it's the easiest to implement
	 * approach; performance can come later. */
	لكل (وٿي = fdt_ايندڙ_ڳنڍ(fdt, startoffset, عدم);
	     وٿي >= 0;
	     وٿي = fdt_ايندڙ_ڳنڍ(fdt, وٿي, عدم)) {
		val = fdt_getprop(fdt, وٿي, propname, &len);
		إذا (val && (len == proplen)
		    && (memcmp(val, propval, len) == 0))
			أرجع وٿي;
	}

	أرجع وٿي; /* error from fdt_next_node() */
}

صحيح fdt_ڳنڍ_وٿي_by_phandle(ثابت فراغ *fdt, uint32_t phandle)
{
	صحيح وٿي;

	إذا ((phandle == 0) || (phandle == ~0U))
		أرجع -FDT_ERR_BADPHANDLE;

	FDT_RO_PROBE(fdt);

	/* FIXME: The algorithm here is pretty horrible: we
	 * potentially scan each property of a node in
	 * fdt_get_phandle(), then if that didn't find what
	 * we want, we scan over them again making our way to the next
	 * node.  Still it's the easiest to implement approach;
	 * performance can come later. */
	لكل (وٿي = fdt_ايندڙ_ڳنڍ(fdt, -1, عدم);
	     وٿي >= 0;
	     وٿي = fdt_ايندڙ_ڳنڍ(fdt, وٿي, عدم)) {
		إذا (fdt_get_phandle(fdt, وٿي) == phandle)
			أرجع وٿي;
	}

	أرجع وٿي; /* error from fdt_next_node() */
}

صحيح fdt_stringlist_شامل_آهي(ثابت حرف *strlist, صحيح listlen, ثابت حرف *str)
{
	صحيح len = لڙي_ڊيگهه(str);
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

صحيح fdt_stringlist_ڳڻپ(ثابت فراغ *fdt, صحيح nodeoffset, ثابت حرف *property)
{
	ثابت حرف *list, *پڇاڙي;
	صحيح ڊيگهائي, ڳڻپ = 0;

	list = fdt_getprop(fdt, nodeoffset, property, &ڊيگهائي);
	إذا (!list)
		أرجع ڊيگهائي;

	پڇاڙي = list + ڊيگهائي;

	طالما (list < پڇاڙي) {
		ڊيگهائي = strnlen(list, پڇاڙي - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		إذا (list + ڊيگهائي > پڇاڙي)
			أرجع -FDT_ERR_BADVALUE;

		list += ڊيگهائي;
		ڳڻپ++;
	}

	أرجع ڳڻپ;
}

صحيح fdt_stringlist_ڳولا(ثابت فراغ *fdt, صحيح nodeoffset, ثابت حرف *property,
			  ثابت حرف *string)
{
	صحيح ڊيگهائي, len, idx = 0;
	ثابت حرف *list, *پڇاڙي;

	list = fdt_getprop(fdt, nodeoffset, property, &ڊيگهائي);
	إذا (!list)
		أرجع ڊيگهائي;

	len = لڙي_ڊيگهه(string) + 1;
	پڇاڙي = list + ڊيگهائي;

	طالما (list < پڇاڙي) {
		ڊيگهائي = strnlen(list, پڇاڙي - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		إذا (list + ڊيگهائي > پڇاڙي)
			أرجع -FDT_ERR_BADVALUE;

		إذا (ڊيگهائي == len && memcmp(list, string, ڊيگهائي) == 0)
			أرجع idx;

		list += ڊيگهائي;
		idx++;
	}

	أرجع -FDT_ERR_NOTFOUND;
}

ثابت حرف *fdt_stringlist_get(ثابت فراغ *fdt, صحيح nodeoffset,
			       ثابت حرف *property, صحيح idx,
			       صحيح *lenp)
{
	ثابت حرف *list, *پڇاڙي;
	صحيح ڊيگهائي;

	list = fdt_getprop(fdt, nodeoffset, property, &ڊيگهائي);
	إذا (!list) {
		إذا (lenp)
			*lenp = ڊيگهائي;

		أرجع عدم;
	}

	پڇاڙي = list + ڊيگهائي;

	طالما (list < پڇاڙي) {
		ڊيگهائي = strnlen(list, پڇاڙي - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		إذا (list + ڊيگهائي > پڇاڙي) {
			إذا (lenp)
				*lenp = -FDT_ERR_BADVALUE;

			أرجع عدم;
		}

		إذا (idx == 0) {
			إذا (lenp)
				*lenp = ڊيگهائي - 1;

			أرجع list;
		}

		list += ڊيگهائي;
		idx--;
	}

	إذا (lenp)
		*lenp = -FDT_ERR_NOTFOUND;

	أرجع عدم;
}

صحيح fdt_ڳنڍ_check_compatible(ثابت فراغ *fdt, صحيح nodeoffset,
			      ثابت حرف *compatible)
{
	ثابت فراغ *prop;
	صحيح len;

	prop = fdt_getprop(fdt, nodeoffset, "compatible", &len);
	إذا (!prop)
		أرجع len;

	أرجع !fdt_stringlist_شامل_آهي(prop, len, compatible);
}

صحيح fdt_ڳنڍ_وٿي_by_compatible(ثابت فراغ *fdt, صحيح startoffset,
				  ثابت حرف *compatible)
{
	صحيح وٿي, err;

	FDT_RO_PROBE(fdt);

	/* FIXME: The algorithm here is pretty horrible: we scan each
	 * property of a node in fdt_node_check_compatible(), then if
	 * that didn't find what we want, we scan over them again
	 * making our way to the next node.  Still it's the easiest to
	 * implement approach; performance can come later. */
	لكل (وٿي = fdt_ايندڙ_ڳنڍ(fdt, startoffset, عدم);
	     وٿي >= 0;
	     وٿي = fdt_ايندڙ_ڳنڍ(fdt, وٿي, عدم)) {
		err = fdt_ڳنڍ_check_compatible(fdt, وٿي, compatible);
		إذا ((err < 0) && (err != -FDT_ERR_NOTFOUND))
			أرجع err;
		وإلا إذا (err == 0)
			أرجع وٿي;
	}

	أرجع وٿي; /* error from fdt_next_node() */
}
