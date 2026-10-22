// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#ضمن "libfdt_env.h"

#ضمن <fdt.h>
#ضمن <libfdt.h>

#ضمن "libfdt_internal.h"

ساكن صحيح fdt_nodename_eq_(ثابت فراغ *fdt, صحيح دننه_واټن,
			    ثابت حرف *s, صحيح len)
{
	صحيح olen;
	ثابت حرف *p = fdt_get_name(fdt, دننه_واټن, &olen);

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

	إذا (can_assume(VALID_ننوت)) {
		s = (ثابت حرف *)fdt + fdt_off_dt_مزي_ګانې(fdt) + stroffset;

		إذا (lenp)
			*lenp = مزي_اوږدوالی(s);
		أرجع s;
	}
	totalsize = fdt_ro_probe_(fdt);
	err = totalsize;
	إذا (totalsize < 0)
		اذهب fail;

	err = -FDT_ERR_BADOFFSET;
	absoffset = stroffset + fdt_off_dt_مزي_ګانې(fdt);
	إذا (absoffset >= (بینښه)totalsize)
		اذهب fail;
	len = totalsize - absoffset;

	إذا (fdt_magic(fdt) == FDT_MAGIC) {
		إذا (stroffset < 0)
			اذهب fail;
		إذا (can_assume(LATEST) || fdt_نسخه(fdt) >= 17) {
			إذا ((بینښه)stroffset >= fdt_اندازه_dt_مزي_ګانې(fdt))
				اذهب fail;
			إذا ((fdt_اندازه_dt_مزي_ګانې(fdt) - stroffset) < len)
				len = fdt_اندازه_dt_مزي_ګانې(fdt) - stroffset;
		}
	} وإلا إذا (fdt_magic(fdt) == FDT_SW_MAGIC) {
		بینښه صحيح sw_stroffset = -stroffset;

		إذا ((stroffset >= 0) ||
		    (sw_stroffset > fdt_اندازه_dt_مزي_ګانې(fdt)))
			اذهب fail;
		إذا (sw_stroffset < len)
			len = sw_stroffset;
	} وإلا {
		err = -FDT_ERR_دننی;
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

صحيح fdt_پیدا_کړه_max_phandle(ثابت فراغ *fdt, uint32_t *phandle)
{
	uint32_t max = 0;
	صحيح دننه_واټن = -1;

	طالما (true) {
		uint32_t ارزښت;

		دننه_واټن = fdt_بل_غوټه(fdt, دننه_واټن, عدم);
		إذا (دننه_واټن < 0) {
			إذا (دننه_واټن == -FDT_ERR_NOTFOUND)
				اكسر;

			أرجع دننه_واټن;
		}

		ارزښت = fdt_get_phandle(fdt, دننه_واټن);

		إذا (ارزښت > max)
			max = ارزښت;
	}

	إذا (phandle)
		*phandle = max;

	أرجع 0;
}

صحيح fdt_generate_phandle(ثابت فراغ *fdt, uint32_t *phandle)
{
	uint32_t max;
	صحيح err;

	err = fdt_پیدا_کړه_max_phandle(fdt, &max);
	إذا (err < 0)
		أرجع err;

	إذا (max == FDT_MAX_PHANDLE)
		أرجع -FDT_ERR_NOPHANDLES;

	إذا (phandle)
		*phandle = max + 1;

	أرجع 0;
}

ساكن ثابت بنية fdt_reserve_ثبت *fdt_mem_rsv(ثابت فراغ *fdt, صحيح n)
{
	بینښه صحيح دننه_واټن = n * حجم(بنية fdt_reserve_ثبت);
	بینښه صحيح absoffset = fdt_off_mem_rsvmap(fdt) + دننه_واټن;

	إذا (!can_assume(VALID_ننوت)) {
		إذا (absoffset < fdt_off_mem_rsvmap(fdt))
			أرجع عدم;
		إذا (absoffset > fdt_totalsize(fdt) -
		    حجم(بنية fdt_reserve_ثبت))
			أرجع عدم;
	}
	أرجع fdt_mem_rsv_(fdt, n);
}

صحيح fdt_get_mem_rsv(ثابت فراغ *fdt, صحيح n, uint64_t *پته, uint64_t *اندازه)
{
	ثابت بنية fdt_reserve_ثبت *re;

	FDT_RO_PROBE(fdt);
	re = fdt_mem_rsv(fdt, n);
	إذا (!can_assume(VALID_ننوت) && !re)
		أرجع -FDT_ERR_BADOFFSET;

	*پته = fdt64_ld_(&re->پته);
	*اندازه = fdt64_ld_(&re->اندازه);
	أرجع 0;
}

صحيح fdt_عدد_mem_rsv(ثابت فراغ *fdt)
{
	صحيح i;
	ثابت بنية fdt_reserve_ثبت *re;

	لكل (i = 0; (re = fdt_mem_rsv(fdt, i)) != عدم; i++) {
		إذا (fdt64_ld_(&re->اندازه) == 0)
			أرجع i;
	}
	أرجع -FDT_ERR_TRUNCATED;
}

ساكن صحيح nextprop_(ثابت فراغ *fdt, صحيح دننه_واټن)
{
	uint32_t نښان;
	صحيح nextoffset;

	افعل {
		نښان = fdt_بل_نښان(fdt, دننه_واټن, &nextoffset);

		بدل (نښان) {
		حالة FDT_پای_ټکی:
			إذا (nextoffset >= 0)
				أرجع -FDT_ERR_BADSTRUCTURE;
			وإلا
				أرجع nextoffset;

		حالة FDT_PROP:
			أرجع دننه_واټن;
		}
		دننه_واټن = nextoffset;
	} طالما (نښان == FDT_NOP);

	أرجع -FDT_ERR_NOTFOUND;
}

صحيح fdt_subnode_دننه_واټن_namelen(ثابت فراغ *fdt, صحيح دننه_واټن,
			       ثابت حرف *name, صحيح namelen)
{
	صحيح ژوروالی;

	FDT_RO_PROBE(fdt);

	لكل (ژوروالی = 0;
	     (دننه_واټن >= 0) && (ژوروالی >= 0);
	     دننه_واټن = fdt_بل_غوټه(fdt, دننه_واټن, &ژوروالی))
		إذا ((ژوروالی == 1)
		    && fdt_nodename_eq_(fdt, دننه_واټن, name, namelen))
			أرجع دننه_واټن;

	إذا (ژوروالی < 0)
		أرجع -FDT_ERR_NOTFOUND;
	أرجع دننه_واټن; /* error */
}

صحيح fdt_subnode_دننه_واټن(ثابت فراغ *fdt, صحيح parentoffset,
		       ثابت حرف *name)
{
	أرجع fdt_subnode_دننه_واټن_namelen(fdt, parentoffset, name, مزي_اوږدوالی(name));
}

صحيح fdt_لار_دننه_واټن_namelen(ثابت فراغ *fdt, ثابت حرف *لار, صحيح namelen)
{
	ثابت حرف *پای_ټکی = لار + namelen;
	ثابت حرف *p = لار;
	صحيح دننه_واټن = 0;

	FDT_RO_PROBE(fdt);

	/* see if we have an alias */
	إذا (*لار != '/') {
		ثابت حرف *q = memchr(لار, '/', پای_ټکی - p);

		إذا (!q)
			q = پای_ټکی;

		p = fdt_get_alias_namelen(fdt, p, q - p);
		إذا (!p)
			أرجع -FDT_ERR_BADPATH;
		دننه_واټن = fdt_لار_دننه_واټن(fdt, p);

		p = q;
	}

	طالما (p < پای_ټکی) {
		ثابت حرف *q;

		طالما (*p == '/') {
			p++;
			إذا (p == پای_ټکی)
				أرجع دننه_واټن;
		}
		q = memchr(p, '/', پای_ټکی - p);
		إذا (! q)
			q = پای_ټکی;

		دننه_واټن = fdt_subnode_دننه_واټن_namelen(fdt, دننه_واټن, p, q-p);
		إذا (دننه_واټن < 0)
			أرجع دننه_واټن;

		p = q;
	}

	أرجع دننه_واټن;
}

صحيح fdt_لار_دننه_واټن(ثابت فراغ *fdt, ثابت حرف *لار)
{
	أرجع fdt_لار_دننه_واټن_namelen(fdt, لار, مزي_اوږدوالی(لار));
}

ثابت حرف *fdt_get_name(ثابت فراغ *fdt, صحيح nodeoffset, صحيح *len)
{
	ثابت بنية fdt_غوټه_سرلیک *nh = fdt_دننه_واټن_ptr_2(fdt, nodeoffset);
	ثابت حرف *nameptr;
	صحيح err;

	إذا (((err = fdt_ro_probe_(fdt)) < 0)
	    || ((err = fdt_check_غوټه_دننه_واټن(fdt, nodeoffset)) < 0))
			اذهب fail;

	nameptr = nh->name;

	إذا (!can_assume(LATEST) && fdt_نسخه(fdt) < 0x10) {
		/*
		 * For old FDT versions, match the naming conventions of V16:
		 * give only the leaf name (after all /). The actual tree
		 * contents are loosely checked.
		 */
		ثابت حرف *ونې_پاڼه;
		ونې_پاڼه = strrchr(nameptr, '/');
		إذا (ونې_پاڼه == عدم) {
			err = -FDT_ERR_BADSTRUCTURE;
			اذهب fail;
		}
		nameptr = ونې_پاڼه+1;
	}

	إذا (len)
		*len = مزي_اوږدوالی(nameptr);

	أرجع nameptr;

 fail:
	إذا (len)
		*len = err;
	أرجع عدم;
}

صحيح fdt_لومړی_property_دننه_واټن(ثابت فراغ *fdt, صحيح nodeoffset)
{
	صحيح دننه_واټن;

	إذا ((دننه_واټن = fdt_check_غوټه_دننه_واټن(fdt, nodeoffset)) < 0)
		أرجع دننه_واټن;

	أرجع nextprop_(fdt, دننه_واټن);
}

صحيح fdt_بل_property_دننه_واټن(ثابت فراغ *fdt, صحيح دننه_واټن)
{
	إذا ((دننه_واټن = fdt_check_prop_دننه_واټن(fdt, دننه_واټن)) < 0)
		أرجع دننه_واټن;

	أرجع nextprop_(fdt, دننه_واټن);
}

ساكن ثابت بنية fdt_property *fdt_get_property_by_دننه_واټن(ثابت فراغ *fdt,
						              صحيح دننه_واټن,
						              صحيح *lenp)
{
	صحيح err;
	ثابت بنية fdt_property *prop;

	إذا (!can_assume(VALID_ننوت) &&
	    (err = fdt_check_prop_دننه_واټن(fdt, دننه_واټن)) < 0) {
		إذا (lenp)
			*lenp = err;
		أرجع عدم;
	}

	prop = fdt_دننه_واټن_ptr_2(fdt, دننه_واټن);

	إذا (lenp)
		*lenp = fdt32_ld_(&prop->len);

	أرجع prop;
}

ثابت بنية fdt_property *fdt_get_property_by_دننه_واټن_2(ثابت فراغ *fdt,
						      صحيح دننه_واټن,
						      صحيح *lenp)
{
	/* Prior to version 16, properties may need realignment
	 * and this API does not work. fdt_getprop_*() will, however. */

	إذا (!can_assume(LATEST) && fdt_نسخه(fdt) < 0x10) {
		إذا (lenp)
			*lenp = -FDT_ERR_BADVERSION;
		أرجع عدم;
	}

	أرجع fdt_get_property_by_دننه_واټن(fdt, دننه_واټن, lenp);
}

ساكن ثابت بنية fdt_property *fdt_get_property_namelen_(ثابت فراغ *fdt,
						            صحيح دننه_واټن,
						            ثابت حرف *name,
						            صحيح namelen,
							    صحيح *lenp,
							    صحيح *poffset)
{
	لكل (دننه_واټن = fdt_لومړی_property_دننه_واټن(fdt, دننه_واټن);
	     (دننه_واټن >= 0);
	     (دننه_واټن = fdt_بل_property_دننه_واټن(fdt, دننه_واټن))) {
		ثابت بنية fdt_property *prop;

		prop = fdt_get_property_by_دننه_واټن(fdt, دننه_واټن, lenp);
		إذا (!can_assume(LIBFDT_FLAWLESS) && !prop) {
			دننه_واټن = -FDT_ERR_دننی;
			اكسر;
		}
		إذا (fdt_string_eq_(fdt, fdt32_ld_(&prop->nameoff),
				   name, namelen)) {
			إذا (poffset)
				*poffset = دننه_واټن;
			أرجع prop;
		}
	}

	إذا (lenp)
		*lenp = دننه_واټن;
	أرجع عدم;
}


ثابت بنية fdt_property *fdt_get_property_namelen(ثابت فراغ *fdt,
						    صحيح دننه_واټن,
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

	أرجع fdt_get_property_namelen_(fdt, دننه_واټن, name, namelen, lenp,
					 عدم);
}


ثابت بنية fdt_property *fdt_get_property(ثابت فراغ *fdt,
					    صحيح nodeoffset,
					    ثابت حرف *name, صحيح *lenp)
{
	أرجع fdt_get_property_namelen(fdt, nodeoffset, name,
					مزي_اوږدوالی(name), lenp);
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
		أرجع prop->معلومات + 4;
	أرجع prop->معلومات;
}

ثابت فراغ *fdt_getprop_by_دننه_واټن(ثابت فراغ *fdt, صحيح دننه_واټن,
				  ثابت حرف **namep, صحيح *lenp)
{
	ثابت بنية fdt_property *prop;

	prop = fdt_get_property_by_دننه_واټن(fdt, دننه_واټن, lenp);
	إذا (!prop)
		أرجع عدم;
	إذا (namep) {
		ثابت حرف *name;
		صحيح namelen;

		إذا (!can_assume(VALID_ننوت)) {
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
	    (دننه_واټن + حجم(*prop)) % 8 && fdt32_ld_(&prop->len) >= 8)
		أرجع prop->معلومات + 4;
	أرجع prop->معلومات;
}

ثابت فراغ *fdt_getprop(ثابت فراغ *fdt, صحيح nodeoffset,
			ثابت حرف *name, صحيح *lenp)
{
	أرجع fdt_getprop_namelen(fdt, nodeoffset, name, مزي_اوږدوالی(name), lenp);
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

	aliasoffset = fdt_لار_دننه_واټن(fdt, "/aliases");
	إذا (aliasoffset < 0)
		أرجع عدم;

	أرجع fdt_getprop_namelen(fdt, aliasoffset, name, namelen, عدم);
}

ثابت حرف *fdt_get_alias(ثابت فراغ *fdt, ثابت حرف *name)
{
	أرجع fdt_get_alias_namelen(fdt, name, مزي_اوږدوالی(name));
}

صحيح fdt_get_لار(ثابت فراغ *fdt, صحيح nodeoffset, حرف *buf, صحيح buflen)
{
	صحيح pdepth = 0, p = 0;
	صحيح دننه_واټن, ژوروالی, namelen;
	ثابت حرف *name;

	FDT_RO_PROBE(fdt);

	إذا (buflen < 2)
		أرجع -FDT_ERR_NOSPACE;

	لكل (دننه_واټن = 0, ژوروالی = 0;
	     (دننه_واټن >= 0) && (دننه_واټن <= nodeoffset);
	     دننه_واټن = fdt_بل_غوټه(fdt, دننه_واټن, &ژوروالی)) {
		طالما (pdepth > ژوروالی) {
			افعل {
				p--;
			} طالما (buf[p-1] != '/');
			pdepth--;
		}

		إذا (pdepth >= ژوروالی) {
			name = fdt_get_name(fdt, دننه_واټن, &namelen);
			إذا (!name)
				أرجع namelen;
			إذا ((p + namelen + 1) <= buflen) {
				حافظه_نقل(buf + p, name, namelen);
				p += namelen;
				buf[p++] = '/';
				pdepth++;
			}
		}

		إذا (دننه_واټن == nodeoffset) {
			إذا (pdepth < (ژوروالی + 1))
				أرجع -FDT_ERR_NOSPACE;

			إذا (p > 1) /* special case so that root path is "/", not "" */
				p--;
			buf[p] = '\0';
			أرجع 0;
		}
	}

	إذا ((دننه_واټن == -FDT_ERR_NOTFOUND) || (دننه_واټن >= 0))
		أرجع -FDT_ERR_BADOFFSET;
	وإلا إذا (دننه_واټن == -FDT_ERR_BADOFFSET)
		أرجع -FDT_ERR_BADSTRUCTURE;

	أرجع دننه_واټن; /* error from fdt_next_node() */
}

صحيح fdt_supernode_atdepth_دننه_واټن(ثابت فراغ *fdt, صحيح nodeoffset,
				 صحيح supernodedepth, صحيح *nodedepth)
{
	صحيح دننه_واټن, ژوروالی;
	صحيح supernodeoffset = -FDT_ERR_دننی;

	FDT_RO_PROBE(fdt);

	إذا (supernodedepth < 0)
		أرجع -FDT_ERR_NOTFOUND;

	لكل (دننه_واټن = 0, ژوروالی = 0;
	     (دننه_واټن >= 0) && (دننه_واټن <= nodeoffset);
	     دننه_واټن = fdt_بل_غوټه(fdt, دننه_واټن, &ژوروالی)) {
		إذا (ژوروالی == supernodedepth)
			supernodeoffset = دننه_واټن;

		إذا (دننه_واټن == nodeoffset) {
			إذا (nodedepth)
				*nodedepth = ژوروالی;

			إذا (supernodedepth > ژوروالی)
				أرجع -FDT_ERR_NOTFOUND;
			وإلا
				أرجع supernodeoffset;
		}
	}

	إذا (!can_assume(VALID_ننوت)) {
		إذا ((دننه_واټن == -FDT_ERR_NOTFOUND) || (دننه_واټن >= 0))
			أرجع -FDT_ERR_BADOFFSET;
		وإلا إذا (دننه_واټن == -FDT_ERR_BADOFFSET)
			أرجع -FDT_ERR_BADSTRUCTURE;
	}

	أرجع دننه_واټن; /* error from fdt_next_node() */
}

صحيح fdt_غوټه_ژوروالی(ثابت فراغ *fdt, صحيح nodeoffset)
{
	صحيح nodedepth;
	صحيح err;

	err = fdt_supernode_atdepth_دننه_واټن(fdt, nodeoffset, 0, &nodedepth);
	إذا (err)
		أرجع (can_assume(LIBFDT_FLAWLESS) || err < 0) ? err :
			-FDT_ERR_دننی;
	أرجع nodedepth;
}

صحيح fdt_پلار_دننه_واټن(ثابت فراغ *fdt, صحيح nodeoffset)
{
	صحيح nodedepth = fdt_غوټه_ژوروالی(fdt, nodeoffset);

	إذا (nodedepth < 0)
		أرجع nodedepth;
	أرجع fdt_supernode_atdepth_دننه_واټن(fdt, nodeoffset,
					    nodedepth - 1, عدم);
}

صحيح fdt_غوټه_دننه_واټن_by_prop_ارزښت(ثابت فراغ *fdt, صحيح startoffset,
				  ثابت حرف *propname,
				  ثابت فراغ *propval, صحيح proplen)
{
	صحيح دننه_واټن;
	ثابت فراغ *val;
	صحيح len;

	FDT_RO_PROBE(fdt);

	/* FIXME: The algorithm here is pretty horrible: we scan each
	 * property of a node in fdt_getprop(), then if that didn't
	 * find what we want, we scan over them again making our way
	 * to the next node.  Still it's the easiest to implement
	 * approach; performance can come later. */
	لكل (دننه_واټن = fdt_بل_غوټه(fdt, startoffset, عدم);
	     دننه_واټن >= 0;
	     دننه_واټن = fdt_بل_غوټه(fdt, دننه_واټن, عدم)) {
		val = fdt_getprop(fdt, دننه_واټن, propname, &len);
		إذا (val && (len == proplen)
		    && (memcmp(val, propval, len) == 0))
			أرجع دننه_واټن;
	}

	أرجع دننه_واټن; /* error from fdt_next_node() */
}

صحيح fdt_غوټه_دننه_واټن_by_phandle(ثابت فراغ *fdt, uint32_t phandle)
{
	صحيح دننه_واټن;

	إذا ((phandle == 0) || (phandle == ~0U))
		أرجع -FDT_ERR_BADPHANDLE;

	FDT_RO_PROBE(fdt);

	/* FIXME: The algorithm here is pretty horrible: we
	 * potentially scan each property of a node in
	 * fdt_get_phandle(), then if that didn't find what
	 * we want, we scan over them again making our way to the next
	 * node.  Still it's the easiest to implement approach;
	 * performance can come later. */
	لكل (دننه_واټن = fdt_بل_غوټه(fdt, -1, عدم);
	     دننه_واټن >= 0;
	     دننه_واټن = fdt_بل_غوټه(fdt, دننه_واټن, عدم)) {
		إذا (fdt_get_phandle(fdt, دننه_واټن) == phandle)
			أرجع دننه_واټن;
	}

	أرجع دننه_واټن; /* error from fdt_next_node() */
}

صحيح fdt_stringlist_لري(ثابت حرف *strlist, صحيح listlen, ثابت حرف *str)
{
	صحيح len = مزي_اوږدوالی(str);
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

صحيح fdt_stringlist_شمېر(ثابت فراغ *fdt, صحيح nodeoffset, ثابت حرف *property)
{
	ثابت حرف *list, *پای_ټکی;
	صحيح اوږدتیا, شمېر = 0;

	list = fdt_getprop(fdt, nodeoffset, property, &اوږدتیا);
	إذا (!list)
		أرجع اوږدتیا;

	پای_ټکی = list + اوږدتیا;

	طالما (list < پای_ټکی) {
		اوږدتیا = strnlen(list, پای_ټکی - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		إذا (list + اوږدتیا > پای_ټکی)
			أرجع -FDT_ERR_BADVALUE;

		list += اوږدتیا;
		شمېر++;
	}

	أرجع شمېر;
}

صحيح fdt_stringlist_پلټنه(ثابت فراغ *fdt, صحيح nodeoffset, ثابت حرف *property,
			  ثابت حرف *string)
{
	صحيح اوږدتیا, len, idx = 0;
	ثابت حرف *list, *پای_ټکی;

	list = fdt_getprop(fdt, nodeoffset, property, &اوږدتیا);
	إذا (!list)
		أرجع اوږدتیا;

	len = مزي_اوږدوالی(string) + 1;
	پای_ټکی = list + اوږدتیا;

	طالما (list < پای_ټکی) {
		اوږدتیا = strnlen(list, پای_ټکی - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		إذا (list + اوږدتیا > پای_ټکی)
			أرجع -FDT_ERR_BADVALUE;

		إذا (اوږدتیا == len && memcmp(list, string, اوږدتیا) == 0)
			أرجع idx;

		list += اوږدتیا;
		idx++;
	}

	أرجع -FDT_ERR_NOTFOUND;
}

ثابت حرف *fdt_stringlist_get(ثابت فراغ *fdt, صحيح nodeoffset,
			       ثابت حرف *property, صحيح idx,
			       صحيح *lenp)
{
	ثابت حرف *list, *پای_ټکی;
	صحيح اوږدتیا;

	list = fdt_getprop(fdt, nodeoffset, property, &اوږدتیا);
	إذا (!list) {
		إذا (lenp)
			*lenp = اوږدتیا;

		أرجع عدم;
	}

	پای_ټکی = list + اوږدتیا;

	طالما (list < پای_ټکی) {
		اوږدتیا = strnlen(list, پای_ټکی - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		إذا (list + اوږدتیا > پای_ټکی) {
			إذا (lenp)
				*lenp = -FDT_ERR_BADVALUE;

			أرجع عدم;
		}

		إذا (idx == 0) {
			إذا (lenp)
				*lenp = اوږدتیا - 1;

			أرجع list;
		}

		list += اوږدتیا;
		idx--;
	}

	إذا (lenp)
		*lenp = -FDT_ERR_NOTFOUND;

	أرجع عدم;
}

صحيح fdt_غوټه_check_compatible(ثابت فراغ *fdt, صحيح nodeoffset,
			      ثابت حرف *compatible)
{
	ثابت فراغ *prop;
	صحيح len;

	prop = fdt_getprop(fdt, nodeoffset, "compatible", &len);
	إذا (!prop)
		أرجع len;

	أرجع !fdt_stringlist_لري(prop, len, compatible);
}

صحيح fdt_غوټه_دننه_واټن_by_compatible(ثابت فراغ *fdt, صحيح startoffset,
				  ثابت حرف *compatible)
{
	صحيح دننه_واټن, err;

	FDT_RO_PROBE(fdt);

	/* FIXME: The algorithm here is pretty horrible: we scan each
	 * property of a node in fdt_node_check_compatible(), then if
	 * that didn't find what we want, we scan over them again
	 * making our way to the next node.  Still it's the easiest to
	 * implement approach; performance can come later. */
	لكل (دننه_واټن = fdt_بل_غوټه(fdt, startoffset, عدم);
	     دننه_واټن >= 0;
	     دننه_واټن = fdt_بل_غوټه(fdt, دننه_واټن, عدم)) {
		err = fdt_غوټه_check_compatible(fdt, دننه_واټن, compatible);
		إذا ((err < 0) && (err != -FDT_ERR_NOTFOUND))
			أرجع err;
		وإلا إذا (err == 0)
			أرجع دننه_واټن;
	}

	أرجع دننه_واټن; /* error from fdt_next_node() */
}
