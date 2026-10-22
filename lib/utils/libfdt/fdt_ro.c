// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#ضمن "libfdt_env.h"

#ضمن <fdt.h>
#ضمن <libfdt.h>

#ضمن "libfdt_internal.h"

ساكن صحيح fdt_nodename_eq_(ثابت فراغ *fdt, صحيح وتھ,
			    ثابت حرف *s, صحيح len)
{
	صحيح olen;
	ثابت حرف *p = fdt_get_name(fdt, وتھ, &olen);

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

	إذا (can_assume(VALID_داخلہ)) {
		s = (ثابت حرف *)fdt + fdt_off_dt_لڑیاں(fdt) + stroffset;

		إذا (lenp)
			*lenp = لڑی_لمبائی(s);
		أرجع s;
	}
	totalsize = fdt_ro_probe_(fdt);
	err = totalsize;
	إذا (totalsize < 0)
		اذهب fail;

	err = -FDT_ERR_BADOFFSET;
	absoffset = stroffset + fdt_off_dt_لڑیاں(fdt);
	إذا (absoffset >= (بنانشانی)totalsize)
		اذهب fail;
	len = totalsize - absoffset;

	إذا (fdt_magic(fdt) == FDT_MAGIC) {
		إذا (stroffset < 0)
			اذهب fail;
		إذا (can_assume(LATEST) || fdt_نسخہ(fdt) >= 17) {
			إذا ((بنانشانی)stroffset >= fdt_ناپ_dt_لڑیاں(fdt))
				اذهب fail;
			إذا ((fdt_ناپ_dt_لڑیاں(fdt) - stroffset) < len)
				len = fdt_ناپ_dt_لڑیاں(fdt) - stroffset;
		}
	} وإلا إذا (fdt_magic(fdt) == FDT_SW_MAGIC) {
		بنانشانی صحيح sw_stroffset = -stroffset;

		إذا ((stroffset >= 0) ||
		    (sw_stroffset > fdt_ناپ_dt_لڑیاں(fdt)))
			اذهب fail;
		إذا (sw_stroffset < len)
			len = sw_stroffset;
	} وإلا {
		err = -FDT_ERR_اندرونی;
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

صحيح fdt_لبھو_max_phandle(ثابت فراغ *fdt, uint32_t *phandle)
{
	uint32_t max = 0;
	صحيح وتھ = -1;

	طالما (true) {
		uint32_t قدر;

		وتھ = fdt_اگلا_گرہ(fdt, وتھ, عدم);
		إذا (وتھ < 0) {
			إذا (وتھ == -FDT_ERR_NOTFOUND)
				اكسر;

			أرجع وتھ;
		}

		قدر = fdt_get_phandle(fdt, وتھ);

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

	err = fdt_لبھو_max_phandle(fdt, &max);
	إذا (err < 0)
		أرجع err;

	إذا (max == FDT_MAX_PHANDLE)
		أرجع -FDT_ERR_NOPHANDLES;

	إذا (phandle)
		*phandle = max + 1;

	أرجع 0;
}

ساكن ثابت بنية fdt_reserve_اندراج *fdt_mem_rsv(ثابت فراغ *fdt, صحيح n)
{
	بنانشانی صحيح وتھ = n * حجم(بنية fdt_reserve_اندراج);
	بنانشانی صحيح absoffset = fdt_off_mem_rsvmap(fdt) + وتھ;

	إذا (!can_assume(VALID_داخلہ)) {
		إذا (absoffset < fdt_off_mem_rsvmap(fdt))
			أرجع عدم;
		إذا (absoffset > fdt_totalsize(fdt) -
		    حجم(بنية fdt_reserve_اندراج))
			أرجع عدم;
	}
	أرجع fdt_mem_rsv_(fdt, n);
}

صحيح fdt_get_mem_rsv(ثابت فراغ *fdt, صحيح n, uint64_t *پتہ, uint64_t *ناپ)
{
	ثابت بنية fdt_reserve_اندراج *re;

	FDT_RO_PROBE(fdt);
	re = fdt_mem_rsv(fdt, n);
	إذا (!can_assume(VALID_داخلہ) && !re)
		أرجع -FDT_ERR_BADOFFSET;

	*پتہ = fdt64_ld_(&re->پتہ);
	*ناپ = fdt64_ld_(&re->ناپ);
	أرجع 0;
}

صحيح fdt_عدد_mem_rsv(ثابت فراغ *fdt)
{
	صحيح i;
	ثابت بنية fdt_reserve_اندراج *re;

	لكل (i = 0; (re = fdt_mem_rsv(fdt, i)) != عدم; i++) {
		إذا (fdt64_ld_(&re->ناپ) == 0)
			أرجع i;
	}
	أرجع -FDT_ERR_TRUNCATED;
}

ساكن صحيح nextprop_(ثابت فراغ *fdt, صحيح وتھ)
{
	uint32_t ٹیگ;
	صحيح nextoffset;

	افعل {
		ٹیگ = fdt_اگلا_ٹیگ(fdt, وتھ, &nextoffset);

		بدل (ٹیگ) {
		حالة FDT_سرا:
			إذا (nextoffset >= 0)
				أرجع -FDT_ERR_BADSTRUCTURE;
			وإلا
				أرجع nextoffset;

		حالة FDT_PROP:
			أرجع وتھ;
		}
		وتھ = nextoffset;
	} طالما (ٹیگ == FDT_NOP);

	أرجع -FDT_ERR_NOTFOUND;
}

صحيح fdt_subnode_وتھ_namelen(ثابت فراغ *fdt, صحيح وتھ,
			       ثابت حرف *name, صحيح namelen)
{
	صحيح ڈونگھائی;

	FDT_RO_PROBE(fdt);

	لكل (ڈونگھائی = 0;
	     (وتھ >= 0) && (ڈونگھائی >= 0);
	     وتھ = fdt_اگلا_گرہ(fdt, وتھ, &ڈونگھائی))
		إذا ((ڈونگھائی == 1)
		    && fdt_nodename_eq_(fdt, وتھ, name, namelen))
			أرجع وتھ;

	إذا (ڈونگھائی < 0)
		أرجع -FDT_ERR_NOTFOUND;
	أرجع وتھ; /* error */
}

صحيح fdt_subnode_وتھ(ثابت فراغ *fdt, صحيح parentoffset,
		       ثابت حرف *name)
{
	أرجع fdt_subnode_وتھ_namelen(fdt, parentoffset, name, لڑی_لمبائی(name));
}

صحيح fdt_رستہ_وتھ_namelen(ثابت فراغ *fdt, ثابت حرف *رستہ, صحيح namelen)
{
	ثابت حرف *سرا = رستہ + namelen;
	ثابت حرف *p = رستہ;
	صحيح وتھ = 0;

	FDT_RO_PROBE(fdt);

	/* see if we have an alias */
	إذا (*رستہ != '/') {
		ثابت حرف *q = memchr(رستہ, '/', سرا - p);

		إذا (!q)
			q = سرا;

		p = fdt_get_alias_namelen(fdt, p, q - p);
		إذا (!p)
			أرجع -FDT_ERR_BADPATH;
		وتھ = fdt_رستہ_وتھ(fdt, p);

		p = q;
	}

	طالما (p < سرا) {
		ثابت حرف *q;

		طالما (*p == '/') {
			p++;
			إذا (p == سرا)
				أرجع وتھ;
		}
		q = memchr(p, '/', سرا - p);
		إذا (! q)
			q = سرا;

		وتھ = fdt_subnode_وتھ_namelen(fdt, وتھ, p, q-p);
		إذا (وتھ < 0)
			أرجع وتھ;

		p = q;
	}

	أرجع وتھ;
}

صحيح fdt_رستہ_وتھ(ثابت فراغ *fdt, ثابت حرف *رستہ)
{
	أرجع fdt_رستہ_وتھ_namelen(fdt, رستہ, لڑی_لمبائی(رستہ));
}

ثابت حرف *fdt_get_name(ثابت فراغ *fdt, صحيح nodeoffset, صحيح *len)
{
	ثابت بنية fdt_گرہ_سرخی *nh = fdt_وتھ_ptr_2(fdt, nodeoffset);
	ثابت حرف *nameptr;
	صحيح err;

	إذا (((err = fdt_ro_probe_(fdt)) < 0)
	    || ((err = fdt_check_گرہ_وتھ(fdt, nodeoffset)) < 0))
			اذهب fail;

	nameptr = nh->name;

	إذا (!can_assume(LATEST) && fdt_نسخہ(fdt) < 0x10) {
		/*
		 * For old FDT versions, match the naming conventions of V16:
		 * give only the leaf name (after all /). The actual tree
		 * contents are loosely checked.
		 */
		ثابت حرف *پتی;
		پتی = strrchr(nameptr, '/');
		إذا (پتی == عدم) {
			err = -FDT_ERR_BADSTRUCTURE;
			اذهب fail;
		}
		nameptr = پتی+1;
	}

	إذا (len)
		*len = لڑی_لمبائی(nameptr);

	أرجع nameptr;

 fail:
	إذا (len)
		*len = err;
	أرجع عدم;
}

صحيح fdt_پہلا_property_وتھ(ثابت فراغ *fdt, صحيح nodeoffset)
{
	صحيح وتھ;

	إذا ((وتھ = fdt_check_گرہ_وتھ(fdt, nodeoffset)) < 0)
		أرجع وتھ;

	أرجع nextprop_(fdt, وتھ);
}

صحيح fdt_اگلا_property_وتھ(ثابت فراغ *fdt, صحيح وتھ)
{
	إذا ((وتھ = fdt_check_prop_وتھ(fdt, وتھ)) < 0)
		أرجع وتھ;

	أرجع nextprop_(fdt, وتھ);
}

ساكن ثابت بنية fdt_property *fdt_get_property_by_وتھ(ثابت فراغ *fdt,
						              صحيح وتھ,
						              صحيح *lenp)
{
	صحيح err;
	ثابت بنية fdt_property *prop;

	إذا (!can_assume(VALID_داخلہ) &&
	    (err = fdt_check_prop_وتھ(fdt, وتھ)) < 0) {
		إذا (lenp)
			*lenp = err;
		أرجع عدم;
	}

	prop = fdt_وتھ_ptr_2(fdt, وتھ);

	إذا (lenp)
		*lenp = fdt32_ld_(&prop->len);

	أرجع prop;
}

ثابت بنية fdt_property *fdt_get_property_by_وتھ_2(ثابت فراغ *fdt,
						      صحيح وتھ,
						      صحيح *lenp)
{
	/* Prior to version 16, properties may need realignment
	 * and this API does not work. fdt_getprop_*() will, however. */

	إذا (!can_assume(LATEST) && fdt_نسخہ(fdt) < 0x10) {
		إذا (lenp)
			*lenp = -FDT_ERR_BADVERSION;
		أرجع عدم;
	}

	أرجع fdt_get_property_by_وتھ(fdt, وتھ, lenp);
}

ساكن ثابت بنية fdt_property *fdt_get_property_namelen_(ثابت فراغ *fdt,
						            صحيح وتھ,
						            ثابت حرف *name,
						            صحيح namelen,
							    صحيح *lenp,
							    صحيح *poffset)
{
	لكل (وتھ = fdt_پہلا_property_وتھ(fdt, وتھ);
	     (وتھ >= 0);
	     (وتھ = fdt_اگلا_property_وتھ(fdt, وتھ))) {
		ثابت بنية fdt_property *prop;

		prop = fdt_get_property_by_وتھ(fdt, وتھ, lenp);
		إذا (!can_assume(LIBFDT_FLAWLESS) && !prop) {
			وتھ = -FDT_ERR_اندرونی;
			اكسر;
		}
		إذا (fdt_string_eq_(fdt, fdt32_ld_(&prop->nameoff),
				   name, namelen)) {
			إذا (poffset)
				*poffset = وتھ;
			أرجع prop;
		}
	}

	إذا (lenp)
		*lenp = وتھ;
	أرجع عدم;
}


ثابت بنية fdt_property *fdt_get_property_namelen(ثابت فراغ *fdt,
						    صحيح وتھ,
						    ثابت حرف *name,
						    صحيح namelen, صحيح *lenp)
{
	/* Prior to version 16, properties may need realignment
	 * and this API does not work. fdt_getprop_*() will, however. */
	إذا (!can_assume(LATEST) && fdt_نسخہ(fdt) < 0x10) {
		إذا (lenp)
			*lenp = -FDT_ERR_BADVERSION;
		أرجع عدم;
	}

	أرجع fdt_get_property_namelen_(fdt, وتھ, name, namelen, lenp,
					 عدم);
}


ثابت بنية fdt_property *fdt_get_property(ثابت فراغ *fdt,
					    صحيح nodeoffset,
					    ثابت حرف *name, صحيح *lenp)
{
	أرجع fdt_get_property_namelen(fdt, nodeoffset, name,
					لڑی_لمبائی(name), lenp);
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
	إذا (!can_assume(LATEST) && fdt_نسخہ(fdt) < 0x10 &&
	    (poffset + حجم(*prop)) % 8 && fdt32_ld_(&prop->len) >= 8)
		أرجع prop->انکڑے + 4;
	أرجع prop->انکڑے;
}

ثابت فراغ *fdt_getprop_by_وتھ(ثابت فراغ *fdt, صحيح وتھ,
				  ثابت حرف **namep, صحيح *lenp)
{
	ثابت بنية fdt_property *prop;

	prop = fdt_get_property_by_وتھ(fdt, وتھ, lenp);
	إذا (!prop)
		أرجع عدم;
	إذا (namep) {
		ثابت حرف *name;
		صحيح namelen;

		إذا (!can_assume(VALID_داخلہ)) {
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
	إذا (!can_assume(LATEST) && fdt_نسخہ(fdt) < 0x10 &&
	    (وتھ + حجم(*prop)) % 8 && fdt32_ld_(&prop->len) >= 8)
		أرجع prop->انکڑے + 4;
	أرجع prop->انکڑے;
}

ثابت فراغ *fdt_getprop(ثابت فراغ *fdt, صحيح nodeoffset,
			ثابت حرف *name, صحيح *lenp)
{
	أرجع fdt_getprop_namelen(fdt, nodeoffset, name, لڑی_لمبائی(name), lenp);
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

	aliasoffset = fdt_رستہ_وتھ(fdt, "/aliases");
	إذا (aliasoffset < 0)
		أرجع عدم;

	أرجع fdt_getprop_namelen(fdt, aliasoffset, name, namelen, عدم);
}

ثابت حرف *fdt_get_alias(ثابت فراغ *fdt, ثابت حرف *name)
{
	أرجع fdt_get_alias_namelen(fdt, name, لڑی_لمبائی(name));
}

صحيح fdt_get_رستہ(ثابت فراغ *fdt, صحيح nodeoffset, حرف *buf, صحيح buflen)
{
	صحيح pdepth = 0, p = 0;
	صحيح وتھ, ڈونگھائی, namelen;
	ثابت حرف *name;

	FDT_RO_PROBE(fdt);

	إذا (buflen < 2)
		أرجع -FDT_ERR_NOSPACE;

	لكل (وتھ = 0, ڈونگھائی = 0;
	     (وتھ >= 0) && (وتھ <= nodeoffset);
	     وتھ = fdt_اگلا_گرہ(fdt, وتھ, &ڈونگھائی)) {
		طالما (pdepth > ڈونگھائی) {
			افعل {
				p--;
			} طالما (buf[p-1] != '/');
			pdepth--;
		}

		إذا (pdepth >= ڈونگھائی) {
			name = fdt_get_name(fdt, وتھ, &namelen);
			إذا (!name)
				أرجع namelen;
			إذا ((p + namelen + 1) <= buflen) {
				حافظہ_نقل(buf + p, name, namelen);
				p += namelen;
				buf[p++] = '/';
				pdepth++;
			}
		}

		إذا (وتھ == nodeoffset) {
			إذا (pdepth < (ڈونگھائی + 1))
				أرجع -FDT_ERR_NOSPACE;

			إذا (p > 1) /* special case so that root path is "/", not "" */
				p--;
			buf[p] = '\0';
			أرجع 0;
		}
	}

	إذا ((وتھ == -FDT_ERR_NOTFOUND) || (وتھ >= 0))
		أرجع -FDT_ERR_BADOFFSET;
	وإلا إذا (وتھ == -FDT_ERR_BADOFFSET)
		أرجع -FDT_ERR_BADSTRUCTURE;

	أرجع وتھ; /* error from fdt_next_node() */
}

صحيح fdt_supernode_atdepth_وتھ(ثابت فراغ *fdt, صحيح nodeoffset,
				 صحيح supernodedepth, صحيح *nodedepth)
{
	صحيح وتھ, ڈونگھائی;
	صحيح supernodeoffset = -FDT_ERR_اندرونی;

	FDT_RO_PROBE(fdt);

	إذا (supernodedepth < 0)
		أرجع -FDT_ERR_NOTFOUND;

	لكل (وتھ = 0, ڈونگھائی = 0;
	     (وتھ >= 0) && (وتھ <= nodeoffset);
	     وتھ = fdt_اگلا_گرہ(fdt, وتھ, &ڈونگھائی)) {
		إذا (ڈونگھائی == supernodedepth)
			supernodeoffset = وتھ;

		إذا (وتھ == nodeoffset) {
			إذا (nodedepth)
				*nodedepth = ڈونگھائی;

			إذا (supernodedepth > ڈونگھائی)
				أرجع -FDT_ERR_NOTFOUND;
			وإلا
				أرجع supernodeoffset;
		}
	}

	إذا (!can_assume(VALID_داخلہ)) {
		إذا ((وتھ == -FDT_ERR_NOTFOUND) || (وتھ >= 0))
			أرجع -FDT_ERR_BADOFFSET;
		وإلا إذا (وتھ == -FDT_ERR_BADOFFSET)
			أرجع -FDT_ERR_BADSTRUCTURE;
	}

	أرجع وتھ; /* error from fdt_next_node() */
}

صحيح fdt_گرہ_ڈونگھائی(ثابت فراغ *fdt, صحيح nodeoffset)
{
	صحيح nodedepth;
	صحيح err;

	err = fdt_supernode_atdepth_وتھ(fdt, nodeoffset, 0, &nodedepth);
	إذا (err)
		أرجع (can_assume(LIBFDT_FLAWLESS) || err < 0) ? err :
			-FDT_ERR_اندرونی;
	أرجع nodedepth;
}

صحيح fdt_ماپا_وتھ(ثابت فراغ *fdt, صحيح nodeoffset)
{
	صحيح nodedepth = fdt_گرہ_ڈونگھائی(fdt, nodeoffset);

	إذا (nodedepth < 0)
		أرجع nodedepth;
	أرجع fdt_supernode_atdepth_وتھ(fdt, nodeoffset,
					    nodedepth - 1, عدم);
}

صحيح fdt_گرہ_وتھ_by_prop_قدر(ثابت فراغ *fdt, صحيح startoffset,
				  ثابت حرف *propname,
				  ثابت فراغ *propval, صحيح proplen)
{
	صحيح وتھ;
	ثابت فراغ *val;
	صحيح len;

	FDT_RO_PROBE(fdt);

	/* FIXME: The algorithm here is pretty horrible: we scan each
	 * property of a node in fdt_getprop(), then if that didn't
	 * find what we want, we scan over them again making our way
	 * to the next node.  Still it's the easiest to implement
	 * approach; performance can come later. */
	لكل (وتھ = fdt_اگلا_گرہ(fdt, startoffset, عدم);
	     وتھ >= 0;
	     وتھ = fdt_اگلا_گرہ(fdt, وتھ, عدم)) {
		val = fdt_getprop(fdt, وتھ, propname, &len);
		إذا (val && (len == proplen)
		    && (memcmp(val, propval, len) == 0))
			أرجع وتھ;
	}

	أرجع وتھ; /* error from fdt_next_node() */
}

صحيح fdt_گرہ_وتھ_by_phandle(ثابت فراغ *fdt, uint32_t phandle)
{
	صحيح وتھ;

	إذا ((phandle == 0) || (phandle == ~0U))
		أرجع -FDT_ERR_BADPHANDLE;

	FDT_RO_PROBE(fdt);

	/* FIXME: The algorithm here is pretty horrible: we
	 * potentially scan each property of a node in
	 * fdt_get_phandle(), then if that didn't find what
	 * we want, we scan over them again making our way to the next
	 * node.  Still it's the easiest to implement approach;
	 * performance can come later. */
	لكل (وتھ = fdt_اگلا_گرہ(fdt, -1, عدم);
	     وتھ >= 0;
	     وتھ = fdt_اگلا_گرہ(fdt, وتھ, عدم)) {
		إذا (fdt_get_phandle(fdt, وتھ) == phandle)
			أرجع وتھ;
	}

	أرجع وتھ; /* error from fdt_next_node() */
}

صحيح fdt_stringlist_شامل_اے(ثابت حرف *strlist, صحيح listlen, ثابت حرف *str)
{
	صحيح len = لڑی_لمبائی(str);
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

صحيح fdt_stringlist_گنتی(ثابت فراغ *fdt, صحيح nodeoffset, ثابت حرف *property)
{
	ثابت حرف *list, *سرا;
	صحيح لمائی, گنتی = 0;

	list = fdt_getprop(fdt, nodeoffset, property, &لمائی);
	إذا (!list)
		أرجع لمائی;

	سرا = list + لمائی;

	طالما (list < سرا) {
		لمائی = strnlen(list, سرا - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		إذا (list + لمائی > سرا)
			أرجع -FDT_ERR_BADVALUE;

		list += لمائی;
		گنتی++;
	}

	أرجع گنتی;
}

صحيح fdt_stringlist_تلاش(ثابت فراغ *fdt, صحيح nodeoffset, ثابت حرف *property,
			  ثابت حرف *string)
{
	صحيح لمائی, len, idx = 0;
	ثابت حرف *list, *سرا;

	list = fdt_getprop(fdt, nodeoffset, property, &لمائی);
	إذا (!list)
		أرجع لمائی;

	len = لڑی_لمبائی(string) + 1;
	سرا = list + لمائی;

	طالما (list < سرا) {
		لمائی = strnlen(list, سرا - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		إذا (list + لمائی > سرا)
			أرجع -FDT_ERR_BADVALUE;

		إذا (لمائی == len && memcmp(list, string, لمائی) == 0)
			أرجع idx;

		list += لمائی;
		idx++;
	}

	أرجع -FDT_ERR_NOTFOUND;
}

ثابت حرف *fdt_stringlist_get(ثابت فراغ *fdt, صحيح nodeoffset,
			       ثابت حرف *property, صحيح idx,
			       صحيح *lenp)
{
	ثابت حرف *list, *سرا;
	صحيح لمائی;

	list = fdt_getprop(fdt, nodeoffset, property, &لمائی);
	إذا (!list) {
		إذا (lenp)
			*lenp = لمائی;

		أرجع عدم;
	}

	سرا = list + لمائی;

	طالما (list < سرا) {
		لمائی = strnlen(list, سرا - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		إذا (list + لمائی > سرا) {
			إذا (lenp)
				*lenp = -FDT_ERR_BADVALUE;

			أرجع عدم;
		}

		إذا (idx == 0) {
			إذا (lenp)
				*lenp = لمائی - 1;

			أرجع list;
		}

		list += لمائی;
		idx--;
	}

	إذا (lenp)
		*lenp = -FDT_ERR_NOTFOUND;

	أرجع عدم;
}

صحيح fdt_گرہ_check_compatible(ثابت فراغ *fdt, صحيح nodeoffset,
			      ثابت حرف *compatible)
{
	ثابت فراغ *prop;
	صحيح len;

	prop = fdt_getprop(fdt, nodeoffset, "compatible", &len);
	إذا (!prop)
		أرجع len;

	أرجع !fdt_stringlist_شامل_اے(prop, len, compatible);
}

صحيح fdt_گرہ_وتھ_by_compatible(ثابت فراغ *fdt, صحيح startoffset,
				  ثابت حرف *compatible)
{
	صحيح وتھ, err;

	FDT_RO_PROBE(fdt);

	/* FIXME: The algorithm here is pretty horrible: we scan each
	 * property of a node in fdt_node_check_compatible(), then if
	 * that didn't find what we want, we scan over them again
	 * making our way to the next node.  Still it's the easiest to
	 * implement approach; performance can come later. */
	لكل (وتھ = fdt_اگلا_گرہ(fdt, startoffset, عدم);
	     وتھ >= 0;
	     وتھ = fdt_اگلا_گرہ(fdt, وتھ, عدم)) {
		err = fdt_گرہ_check_compatible(fdt, وتھ, compatible);
		إذا ((err < 0) && (err != -FDT_ERR_NOTFOUND))
			أرجع err;
		وإلا إذا (err == 0)
			أرجع وتھ;
	}

	أرجع وتھ; /* error from fdt_next_node() */
}
