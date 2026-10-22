// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#شامل "libfdt_env.h"

#شامل <fdt.h>
#شامل <libfdt.h>

#شامل "libfdt_internal.h"

ساکن صحیح fdt_nodename_eq_(ثابت خالی *fdt, صحیح انحراف,
			    ثابت حرف *s, صحیح len)
{
	صحیح olen;
	ثابت حرف *p = fdt_get_name(fdt, انحراف, &olen);

	اگر (!p || olen < len)
		/* short match */
		واپس 0;

	اگر (memcmp(p, s, len) != 0)
		واپس 0;

	اگر (p[len] == '\0')
		واپس 1;
	ورنہ اگر (!memchr(s, '@', len) && (p[len] == '@'))
		واپس 1;
	ورنہ
		واپس 0;
}

ثابت حرف *fdt_get_string(ثابت خالی *fdt, صحیح stroffset, صحیح *lenp)
{
	int32_t totalsize;
	uint32_t absoffset;
	size_t len;
	صحیح err;
	ثابت حرف *s, *n;

	اگر (can_assume(VALID_داخلہ)) {
		s = (ثابت حرف *)fdt + fdt_off_dt_لڑیاں(fdt) + stroffset;

		اگر (lenp)
			*lenp = لڑی_لمبائی(s);
		واپس s;
	}
	totalsize = fdt_ro_probe_(fdt);
	err = totalsize;
	اگر (totalsize < 0)
		جاؤ fail;

	err = -FDT_ERR_BADOFFSET;
	absoffset = stroffset + fdt_off_dt_لڑیاں(fdt);
	اگر (absoffset >= (غیرعلامتی)totalsize)
		جاؤ fail;
	len = totalsize - absoffset;

	اگر (fdt_magic(fdt) == FDT_MAGIC) {
		اگر (stroffset < 0)
			جاؤ fail;
		اگر (can_assume(LATEST) || fdt_نسخہ(fdt) >= 17) {
			اگر ((غیرعلامتی)stroffset >= fdt_سائز_dt_لڑیاں(fdt))
				جاؤ fail;
			اگر ((fdt_سائز_dt_لڑیاں(fdt) - stroffset) < len)
				len = fdt_سائز_dt_لڑیاں(fdt) - stroffset;
		}
	} ورنہ اگر (fdt_magic(fdt) == FDT_SW_MAGIC) {
		غیرعلامتی صحیح sw_stroffset = -stroffset;

		اگر ((stroffset >= 0) ||
		    (sw_stroffset > fdt_سائز_dt_لڑیاں(fdt)))
			جاؤ fail;
		اگر (sw_stroffset < len)
			len = sw_stroffset;
	} ورنہ {
		err = -FDT_ERR_اندرونی;
		جاؤ fail;
	}

	s = (ثابت حرف *)fdt + absoffset;
	n = memchr(s, '\0', len);
	اگر (!n) {
		/* missing terminating NULL */
		err = -FDT_ERR_TRUNCATED;
		جاؤ fail;
	}

	اگر (lenp)
		*lenp = n - s;
	واپس s;

fail:
	اگر (lenp)
		*lenp = err;
	واپس تہی;
}

ثابت حرف *fdt_string(ثابت خالی *fdt, صحیح stroffset)
{
	واپس fdt_get_string(fdt, stroffset, تہی);
}

ساکن صحیح fdt_string_eq_(ثابت خالی *fdt, صحیح stroffset,
			  ثابت حرف *s, صحیح len)
{
	صحیح slen;
	ثابت حرف *p = fdt_get_string(fdt, stroffset, &slen);

	واپس p && (slen == len) && (memcmp(p, s, len) == 0);
}

صحیح fdt_ڈھونڈو_max_phandle(ثابت خالی *fdt, uint32_t *phandle)
{
	uint32_t max = 0;
	صحیح انحراف = -1;

	جب (true) {
		uint32_t قدر;

		انحراف = fdt_اگلا_گرہ(fdt, انحراف, تہی);
		اگر (انحراف < 0) {
			اگر (انحراف == -FDT_ERR_NOTFOUND)
				توڑو;

			واپس انحراف;
		}

		قدر = fdt_get_phandle(fdt, انحراف);

		اگر (قدر > max)
			max = قدر;
	}

	اگر (phandle)
		*phandle = max;

	واپس 0;
}

صحیح fdt_generate_phandle(ثابت خالی *fdt, uint32_t *phandle)
{
	uint32_t max;
	صحیح err;

	err = fdt_ڈھونڈو_max_phandle(fdt, &max);
	اگر (err < 0)
		واپس err;

	اگر (max == FDT_MAX_PHANDLE)
		واپس -FDT_ERR_NOPHANDLES;

	اگر (phandle)
		*phandle = max + 1;

	واپس 0;
}

ساکن ثابت ساخت fdt_reserve_اندراج *fdt_mem_rsv(ثابت خالی *fdt, صحیح n)
{
	غیرعلامتی صحیح انحراف = n * حجم(ساخت fdt_reserve_اندراج);
	غیرعلامتی صحیح absoffset = fdt_off_mem_rsvmap(fdt) + انحراف;

	اگر (!can_assume(VALID_داخلہ)) {
		اگر (absoffset < fdt_off_mem_rsvmap(fdt))
			واپس تہی;
		اگر (absoffset > fdt_totalsize(fdt) -
		    حجم(ساخت fdt_reserve_اندراج))
			واپس تہی;
	}
	واپس fdt_mem_rsv_(fdt, n);
}

صحیح fdt_get_mem_rsv(ثابت خالی *fdt, صحیح n, uint64_t *پتہ, uint64_t *سائز)
{
	ثابت ساخت fdt_reserve_اندراج *re;

	FDT_RO_PROBE(fdt);
	re = fdt_mem_rsv(fdt, n);
	اگر (!can_assume(VALID_داخلہ) && !re)
		واپس -FDT_ERR_BADOFFSET;

	*پتہ = fdt64_ld_(&re->پتہ);
	*سائز = fdt64_ld_(&re->سائز);
	واپس 0;
}

صحیح fdt_عدد_mem_rsv(ثابت خالی *fdt)
{
	صحیح i;
	ثابت ساخت fdt_reserve_اندراج *re;

	برائے (i = 0; (re = fdt_mem_rsv(fdt, i)) != تہی; i++) {
		اگر (fdt64_ld_(&re->سائز) == 0)
			واپس i;
	}
	واپس -FDT_ERR_TRUNCATED;
}

ساکن صحیح nextprop_(ثابت خالی *fdt, صحیح انحراف)
{
	uint32_t لیبل;
	صحیح nextoffset;

	کرو {
		لیبل = fdt_اگلا_لیبل(fdt, انحراف, &nextoffset);

		انتخاب (لیبل) {
		حالت FDT_انتہا:
			اگر (nextoffset >= 0)
				واپس -FDT_ERR_BADSTRUCTURE;
			ورنہ
				واپس nextoffset;

		حالت FDT_PROP:
			واپس انحراف;
		}
		انحراف = nextoffset;
	} جب (لیبل == FDT_NOP);

	واپس -FDT_ERR_NOTFOUND;
}

صحیح fdt_subnode_انحراف_namelen(ثابت خالی *fdt, صحیح انحراف,
			       ثابت حرف *name, صحیح namelen)
{
	صحیح گہرائی;

	FDT_RO_PROBE(fdt);

	برائے (گہرائی = 0;
	     (انحراف >= 0) && (گہرائی >= 0);
	     انحراف = fdt_اگلا_گرہ(fdt, انحراف, &گہرائی))
		اگر ((گہرائی == 1)
		    && fdt_nodename_eq_(fdt, انحراف, name, namelen))
			واپس انحراف;

	اگر (گہرائی < 0)
		واپس -FDT_ERR_NOTFOUND;
	واپس انحراف; /* error */
}

صحیح fdt_subnode_انحراف(ثابت خالی *fdt, صحیح parentoffset,
		       ثابت حرف *name)
{
	واپس fdt_subnode_انحراف_namelen(fdt, parentoffset, name, لڑی_لمبائی(name));
}

صحیح fdt_راستہ_انحراف_namelen(ثابت خالی *fdt, ثابت حرف *راستہ, صحیح namelen)
{
	ثابت حرف *انتہا = راستہ + namelen;
	ثابت حرف *p = راستہ;
	صحیح انحراف = 0;

	FDT_RO_PROBE(fdt);

	/* see if we have an alias */
	اگر (*راستہ != '/') {
		ثابت حرف *q = memchr(راستہ, '/', انتہا - p);

		اگر (!q)
			q = انتہا;

		p = fdt_get_alias_namelen(fdt, p, q - p);
		اگر (!p)
			واپس -FDT_ERR_BADPATH;
		انحراف = fdt_راستہ_انحراف(fdt, p);

		p = q;
	}

	جب (p < انتہا) {
		ثابت حرف *q;

		جب (*p == '/') {
			p++;
			اگر (p == انتہا)
				واپس انحراف;
		}
		q = memchr(p, '/', انتہا - p);
		اگر (! q)
			q = انتہا;

		انحراف = fdt_subnode_انحراف_namelen(fdt, انحراف, p, q-p);
		اگر (انحراف < 0)
			واپس انحراف;

		p = q;
	}

	واپس انحراف;
}

صحیح fdt_راستہ_انحراف(ثابت خالی *fdt, ثابت حرف *راستہ)
{
	واپس fdt_راستہ_انحراف_namelen(fdt, راستہ, لڑی_لمبائی(راستہ));
}

ثابت حرف *fdt_get_name(ثابت خالی *fdt, صحیح nodeoffset, صحیح *len)
{
	ثابت ساخت fdt_گرہ_سرخی *nh = fdt_انحراف_ptr_2(fdt, nodeoffset);
	ثابت حرف *nameptr;
	صحیح err;

	اگر (((err = fdt_ro_probe_(fdt)) < 0)
	    || ((err = fdt_check_گرہ_انحراف(fdt, nodeoffset)) < 0))
			جاؤ fail;

	nameptr = nh->name;

	اگر (!can_assume(LATEST) && fdt_نسخہ(fdt) < 0x10) {
		/*
		 * For old FDT versions, match the naming conventions of V16:
		 * give only the leaf name (after all /). The actual tree
		 * contents are loosely checked.
		 */
		ثابت حرف *برگ;
		برگ = strrchr(nameptr, '/');
		اگر (برگ == تہی) {
			err = -FDT_ERR_BADSTRUCTURE;
			جاؤ fail;
		}
		nameptr = برگ+1;
	}

	اگر (len)
		*len = لڑی_لمبائی(nameptr);

	واپس nameptr;

 fail:
	اگر (len)
		*len = err;
	واپس تہی;
}

صحیح fdt_پہلا_property_انحراف(ثابت خالی *fdt, صحیح nodeoffset)
{
	صحیح انحراف;

	اگر ((انحراف = fdt_check_گرہ_انحراف(fdt, nodeoffset)) < 0)
		واپس انحراف;

	واپس nextprop_(fdt, انحراف);
}

صحیح fdt_اگلا_property_انحراف(ثابت خالی *fdt, صحیح انحراف)
{
	اگر ((انحراف = fdt_check_prop_انحراف(fdt, انحراف)) < 0)
		واپس انحراف;

	واپس nextprop_(fdt, انحراف);
}

ساکن ثابت ساخت fdt_property *fdt_get_property_by_انحراف(ثابت خالی *fdt,
						              صحیح انحراف,
						              صحیح *lenp)
{
	صحیح err;
	ثابت ساخت fdt_property *prop;

	اگر (!can_assume(VALID_داخلہ) &&
	    (err = fdt_check_prop_انحراف(fdt, انحراف)) < 0) {
		اگر (lenp)
			*lenp = err;
		واپس تہی;
	}

	prop = fdt_انحراف_ptr_2(fdt, انحراف);

	اگر (lenp)
		*lenp = fdt32_ld_(&prop->len);

	واپس prop;
}

ثابت ساخت fdt_property *fdt_get_property_by_انحراف_2(ثابت خالی *fdt,
						      صحیح انحراف,
						      صحیح *lenp)
{
	/* Prior to version 16, properties may need realignment
	 * and this API does not work. fdt_getprop_*() will, however. */

	اگر (!can_assume(LATEST) && fdt_نسخہ(fdt) < 0x10) {
		اگر (lenp)
			*lenp = -FDT_ERR_BADVERSION;
		واپس تہی;
	}

	واپس fdt_get_property_by_انحراف(fdt, انحراف, lenp);
}

ساکن ثابت ساخت fdt_property *fdt_get_property_namelen_(ثابت خالی *fdt,
						            صحیح انحراف,
						            ثابت حرف *name,
						            صحیح namelen,
							    صحیح *lenp,
							    صحیح *poffset)
{
	برائے (انحراف = fdt_پہلا_property_انحراف(fdt, انحراف);
	     (انحراف >= 0);
	     (انحراف = fdt_اگلا_property_انحراف(fdt, انحراف))) {
		ثابت ساخت fdt_property *prop;

		prop = fdt_get_property_by_انحراف(fdt, انحراف, lenp);
		اگر (!can_assume(LIBFDT_FLAWLESS) && !prop) {
			انحراف = -FDT_ERR_اندرونی;
			توڑو;
		}
		اگر (fdt_string_eq_(fdt, fdt32_ld_(&prop->nameoff),
				   name, namelen)) {
			اگر (poffset)
				*poffset = انحراف;
			واپس prop;
		}
	}

	اگر (lenp)
		*lenp = انحراف;
	واپس تہی;
}


ثابت ساخت fdt_property *fdt_get_property_namelen(ثابت خالی *fdt,
						    صحیح انحراف,
						    ثابت حرف *name,
						    صحیح namelen, صحیح *lenp)
{
	/* Prior to version 16, properties may need realignment
	 * and this API does not work. fdt_getprop_*() will, however. */
	اگر (!can_assume(LATEST) && fdt_نسخہ(fdt) < 0x10) {
		اگر (lenp)
			*lenp = -FDT_ERR_BADVERSION;
		واپس تہی;
	}

	واپس fdt_get_property_namelen_(fdt, انحراف, name, namelen, lenp,
					 تہی);
}


ثابت ساخت fdt_property *fdt_get_property(ثابت خالی *fdt,
					    صحیح nodeoffset,
					    ثابت حرف *name, صحیح *lenp)
{
	واپس fdt_get_property_namelen(fdt, nodeoffset, name,
					لڑی_لمبائی(name), lenp);
}

ثابت خالی *fdt_getprop_namelen(ثابت خالی *fdt, صحیح nodeoffset,
				ثابت حرف *name, صحیح namelen, صحیح *lenp)
{
	صحیح poffset;
	ثابت ساخت fdt_property *prop;

	prop = fdt_get_property_namelen_(fdt, nodeoffset, name, namelen, lenp,
					 &poffset);
	اگر (!prop)
		واپس تہی;

	/* Handle realignment */
	اگر (!can_assume(LATEST) && fdt_نسخہ(fdt) < 0x10 &&
	    (poffset + حجم(*prop)) % 8 && fdt32_ld_(&prop->len) >= 8)
		واپس prop->ڈیٹا + 4;
	واپس prop->ڈیٹا;
}

ثابت خالی *fdt_getprop_by_انحراف(ثابت خالی *fdt, صحیح انحراف,
				  ثابت حرف **namep, صحیح *lenp)
{
	ثابت ساخت fdt_property *prop;

	prop = fdt_get_property_by_انحراف(fdt, انحراف, lenp);
	اگر (!prop)
		واپس تہی;
	اگر (namep) {
		ثابت حرف *name;
		صحیح namelen;

		اگر (!can_assume(VALID_داخلہ)) {
			name = fdt_get_string(fdt, fdt32_ld_(&prop->nameoff),
					      &namelen);
			اگر (!name) {
				اگر (lenp)
					*lenp = namelen;
				واپس تہی;
			}
			*namep = name;
		} ورنہ {
			*namep = fdt_string(fdt, fdt32_ld_(&prop->nameoff));
		}
	}

	/* Handle realignment */
	اگر (!can_assume(LATEST) && fdt_نسخہ(fdt) < 0x10 &&
	    (انحراف + حجم(*prop)) % 8 && fdt32_ld_(&prop->len) >= 8)
		واپس prop->ڈیٹا + 4;
	واپس prop->ڈیٹا;
}

ثابت خالی *fdt_getprop(ثابت خالی *fdt, صحیح nodeoffset,
			ثابت حرف *name, صحیح *lenp)
{
	واپس fdt_getprop_namelen(fdt, nodeoffset, name, لڑی_لمبائی(name), lenp);
}

uint32_t fdt_get_phandle(ثابت خالی *fdt, صحیح nodeoffset)
{
	ثابت fdt32_t *php;
	صحیح len;

	/* FIXME: This is a bit sub-optimal, since we potentially scan
	 * over all the properties twice. */
	php = fdt_getprop(fdt, nodeoffset, "phandle", &len);
	اگر (!php || (len != حجم(*php))) {
		php = fdt_getprop(fdt, nodeoffset, "linux,phandle", &len);
		اگر (!php || (len != حجم(*php)))
			واپس 0;
	}

	واپس fdt32_ld_(php);
}

ثابت حرف *fdt_get_alias_namelen(ثابت خالی *fdt,
				  ثابت حرف *name, صحیح namelen)
{
	صحیح aliasoffset;

	aliasoffset = fdt_راستہ_انحراف(fdt, "/aliases");
	اگر (aliasoffset < 0)
		واپس تہی;

	واپس fdt_getprop_namelen(fdt, aliasoffset, name, namelen, تہی);
}

ثابت حرف *fdt_get_alias(ثابت خالی *fdt, ثابت حرف *name)
{
	واپس fdt_get_alias_namelen(fdt, name, لڑی_لمبائی(name));
}

صحیح fdt_get_راستہ(ثابت خالی *fdt, صحیح nodeoffset, حرف *buf, صحیح buflen)
{
	صحیح pdepth = 0, p = 0;
	صحیح انحراف, گہرائی, namelen;
	ثابت حرف *name;

	FDT_RO_PROBE(fdt);

	اگر (buflen < 2)
		واپس -FDT_ERR_NOSPACE;

	برائے (انحراف = 0, گہرائی = 0;
	     (انحراف >= 0) && (انحراف <= nodeoffset);
	     انحراف = fdt_اگلا_گرہ(fdt, انحراف, &گہرائی)) {
		جب (pdepth > گہرائی) {
			کرو {
				p--;
			} جب (buf[p-1] != '/');
			pdepth--;
		}

		اگر (pdepth >= گہرائی) {
			name = fdt_get_name(fdt, انحراف, &namelen);
			اگر (!name)
				واپس namelen;
			اگر ((p + namelen + 1) <= buflen) {
				حافظہ_نقل(buf + p, name, namelen);
				p += namelen;
				buf[p++] = '/';
				pdepth++;
			}
		}

		اگر (انحراف == nodeoffset) {
			اگر (pdepth < (گہرائی + 1))
				واپس -FDT_ERR_NOSPACE;

			اگر (p > 1) /* special case so that root path is "/", not "" */
				p--;
			buf[p] = '\0';
			واپس 0;
		}
	}

	اگر ((انحراف == -FDT_ERR_NOTFOUND) || (انحراف >= 0))
		واپس -FDT_ERR_BADOFFSET;
	ورنہ اگر (انحراف == -FDT_ERR_BADOFFSET)
		واپس -FDT_ERR_BADSTRUCTURE;

	واپس انحراف; /* error from fdt_next_node() */
}

صحیح fdt_supernode_atdepth_انحراف(ثابت خالی *fdt, صحیح nodeoffset,
				 صحیح supernodedepth, صحیح *nodedepth)
{
	صحیح انحراف, گہرائی;
	صحیح supernodeoffset = -FDT_ERR_اندرونی;

	FDT_RO_PROBE(fdt);

	اگر (supernodedepth < 0)
		واپس -FDT_ERR_NOTFOUND;

	برائے (انحراف = 0, گہرائی = 0;
	     (انحراف >= 0) && (انحراف <= nodeoffset);
	     انحراف = fdt_اگلا_گرہ(fdt, انحراف, &گہرائی)) {
		اگر (گہرائی == supernodedepth)
			supernodeoffset = انحراف;

		اگر (انحراف == nodeoffset) {
			اگر (nodedepth)
				*nodedepth = گہرائی;

			اگر (supernodedepth > گہرائی)
				واپس -FDT_ERR_NOTFOUND;
			ورنہ
				واپس supernodeoffset;
		}
	}

	اگر (!can_assume(VALID_داخلہ)) {
		اگر ((انحراف == -FDT_ERR_NOTFOUND) || (انحراف >= 0))
			واپس -FDT_ERR_BADOFFSET;
		ورنہ اگر (انحراف == -FDT_ERR_BADOFFSET)
			واپس -FDT_ERR_BADSTRUCTURE;
	}

	واپس انحراف; /* error from fdt_next_node() */
}

صحیح fdt_گرہ_گہرائی(ثابت خالی *fdt, صحیح nodeoffset)
{
	صحیح nodedepth;
	صحیح err;

	err = fdt_supernode_atdepth_انحراف(fdt, nodeoffset, 0, &nodedepth);
	اگر (err)
		واپس (can_assume(LIBFDT_FLAWLESS) || err < 0) ? err :
			-FDT_ERR_اندرونی;
	واپس nodedepth;
}

صحیح fdt_والد_انحراف(ثابت خالی *fdt, صحیح nodeoffset)
{
	صحیح nodedepth = fdt_گرہ_گہرائی(fdt, nodeoffset);

	اگر (nodedepth < 0)
		واپس nodedepth;
	واپس fdt_supernode_atdepth_انحراف(fdt, nodeoffset,
					    nodedepth - 1, تہی);
}

صحیح fdt_گرہ_انحراف_by_prop_قدر(ثابت خالی *fdt, صحیح startoffset,
				  ثابت حرف *propname,
				  ثابت خالی *propval, صحیح proplen)
{
	صحیح انحراف;
	ثابت خالی *val;
	صحیح len;

	FDT_RO_PROBE(fdt);

	/* FIXME: The algorithm here is pretty horrible: we scan each
	 * property of a node in fdt_getprop(), then if that didn't
	 * find what we want, we scan over them again making our way
	 * to the next node.  Still it's the easiest to implement
	 * approach; performance can come later. */
	برائے (انحراف = fdt_اگلا_گرہ(fdt, startoffset, تہی);
	     انحراف >= 0;
	     انحراف = fdt_اگلا_گرہ(fdt, انحراف, تہی)) {
		val = fdt_getprop(fdt, انحراف, propname, &len);
		اگر (val && (len == proplen)
		    && (memcmp(val, propval, len) == 0))
			واپس انحراف;
	}

	واپس انحراف; /* error from fdt_next_node() */
}

صحیح fdt_گرہ_انحراف_by_phandle(ثابت خالی *fdt, uint32_t phandle)
{
	صحیح انحراف;

	اگر ((phandle == 0) || (phandle == ~0U))
		واپس -FDT_ERR_BADPHANDLE;

	FDT_RO_PROBE(fdt);

	/* FIXME: The algorithm here is pretty horrible: we
	 * potentially scan each property of a node in
	 * fdt_get_phandle(), then if that didn't find what
	 * we want, we scan over them again making our way to the next
	 * node.  Still it's the easiest to implement approach;
	 * performance can come later. */
	برائے (انحراف = fdt_اگلا_گرہ(fdt, -1, تہی);
	     انحراف >= 0;
	     انحراف = fdt_اگلا_گرہ(fdt, انحراف, تہی)) {
		اگر (fdt_get_phandle(fdt, انحراف) == phandle)
			واپس انحراف;
	}

	واپس انحراف; /* error from fdt_next_node() */
}

صحیح fdt_stringlist_شامل_ہے(ثابت حرف *strlist, صحیح listlen, ثابت حرف *str)
{
	صحیح len = لڑی_لمبائی(str);
	ثابت حرف *p;

	جب (listlen >= len) {
		اگر (memcmp(str, strlist, len+1) == 0)
			واپس 1;
		p = memchr(strlist, '\0', listlen);
		اگر (!p)
			واپس 0; /* malformed strlist.. */
		listlen -= (p-strlist) + 1;
		strlist = p + 1;
	}
	واپس 0;
}

صحیح fdt_stringlist_گنتی(ثابت خالی *fdt, صحیح nodeoffset, ثابت حرف *property)
{
	ثابت حرف *list, *انتہا;
	صحیح لمبائی, گنتی = 0;

	list = fdt_getprop(fdt, nodeoffset, property, &لمبائی);
	اگر (!list)
		واپس لمبائی;

	انتہا = list + لمبائی;

	جب (list < انتہا) {
		لمبائی = strnlen(list, انتہا - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		اگر (list + لمبائی > انتہا)
			واپس -FDT_ERR_BADVALUE;

		list += لمبائی;
		گنتی++;
	}

	واپس گنتی;
}

صحیح fdt_stringlist_تلاش(ثابت خالی *fdt, صحیح nodeoffset, ثابت حرف *property,
			  ثابت حرف *string)
{
	صحیح لمبائی, len, idx = 0;
	ثابت حرف *list, *انتہا;

	list = fdt_getprop(fdt, nodeoffset, property, &لمبائی);
	اگر (!list)
		واپس لمبائی;

	len = لڑی_لمبائی(string) + 1;
	انتہا = list + لمبائی;

	جب (list < انتہا) {
		لمبائی = strnlen(list, انتہا - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		اگر (list + لمبائی > انتہا)
			واپس -FDT_ERR_BADVALUE;

		اگر (لمبائی == len && memcmp(list, string, لمبائی) == 0)
			واپس idx;

		list += لمبائی;
		idx++;
	}

	واپس -FDT_ERR_NOTFOUND;
}

ثابت حرف *fdt_stringlist_get(ثابت خالی *fdt, صحیح nodeoffset,
			       ثابت حرف *property, صحیح idx,
			       صحیح *lenp)
{
	ثابت حرف *list, *انتہا;
	صحیح لمبائی;

	list = fdt_getprop(fdt, nodeoffset, property, &لمبائی);
	اگر (!list) {
		اگر (lenp)
			*lenp = لمبائی;

		واپس تہی;
	}

	انتہا = list + لمبائی;

	جب (list < انتہا) {
		لمبائی = strnlen(list, انتہا - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		اگر (list + لمبائی > انتہا) {
			اگر (lenp)
				*lenp = -FDT_ERR_BADVALUE;

			واپس تہی;
		}

		اگر (idx == 0) {
			اگر (lenp)
				*lenp = لمبائی - 1;

			واپس list;
		}

		list += لمبائی;
		idx--;
	}

	اگر (lenp)
		*lenp = -FDT_ERR_NOTFOUND;

	واپس تہی;
}

صحیح fdt_گرہ_check_compatible(ثابت خالی *fdt, صحیح nodeoffset,
			      ثابت حرف *compatible)
{
	ثابت خالی *prop;
	صحیح len;

	prop = fdt_getprop(fdt, nodeoffset, "compatible", &len);
	اگر (!prop)
		واپس len;

	واپس !fdt_stringlist_شامل_ہے(prop, len, compatible);
}

صحیح fdt_گرہ_انحراف_by_compatible(ثابت خالی *fdt, صحیح startoffset,
				  ثابت حرف *compatible)
{
	صحیح انحراف, err;

	FDT_RO_PROBE(fdt);

	/* FIXME: The algorithm here is pretty horrible: we scan each
	 * property of a node in fdt_node_check_compatible(), then if
	 * that didn't find what we want, we scan over them again
	 * making our way to the next node.  Still it's the easiest to
	 * implement approach; performance can come later. */
	برائے (انحراف = fdt_اگلا_گرہ(fdt, startoffset, تہی);
	     انحراف >= 0;
	     انحراف = fdt_اگلا_گرہ(fdt, انحراف, تہی)) {
		err = fdt_گرہ_check_compatible(fdt, انحراف, compatible);
		اگر ((err < 0) && (err != -FDT_ERR_NOTFOUND))
			واپس err;
		ورنہ اگر (err == 0)
			واپس انحراف;
	}

	واپس انحراف; /* error from fdt_next_node() */
}
