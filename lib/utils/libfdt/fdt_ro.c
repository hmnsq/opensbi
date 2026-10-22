// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#ਅਨ੍ਤਰ੍ਭਾਵ "libfdt_env.h"

#ਅਨ੍ਤਰ੍ਭਾਵ <fdt.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <libfdt.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "libfdt_internal.h"

ਸਥਿਰ ਪੂਰਨ fdt_nodename_eq_(ਅਚਲ ਖਾਲੀ *fdt, ਪੂਰਨ ਵਿੱਥ,
			    ਅਚਲ ਅੱਖਰ *s, ਪੂਰਨ len)
{
	ਪੂਰਨ olen;
	ਅਚਲ ਅੱਖਰ *p = fdt_get_name(fdt, ਵਿੱਥ, &olen);

	ਜੇ (!p || olen < len)
		/* short match */
		ਵਾਪਸ 0;

	ਜੇ (memcmp(p, s, len) != 0)
		ਵਾਪਸ 0;

	ਜੇ (p[len] == '\0')
		ਵਾਪਸ 1;
	ਨਹੀਂਤਾਂ ਜੇ (!memchr(s, '@', len) && (p[len] == '@'))
		ਵਾਪਸ 1;
	ਨਹੀਂਤਾਂ
		ਵਾਪਸ 0;
}

ਅਚਲ ਅੱਖਰ *fdt_get_string(ਅਚਲ ਖਾਲੀ *fdt, ਪੂਰਨ stroffset, ਪੂਰਨ *lenp)
{
	int32_t totalsize;
	uint32_t absoffset;
	size_t len;
	ਪੂਰਨ err;
	ਅਚਲ ਅੱਖਰ *s, *n;

	ਜੇ (can_assume(VALID_ਨਿਵੇਸ਼)) {
		s = (ਅਚਲ ਅੱਖਰ *)fdt + fdt_off_dt_ਲੜੀਆਂ(fdt) + stroffset;

		ਜੇ (lenp)
			*lenp = ਲੜੀ_ਲੰਬਾਈ(s);
		ਵਾਪਸ s;
	}
	totalsize = fdt_ro_probe_(fdt);
	err = totalsize;
	ਜੇ (totalsize < 0)
		ਜਾਓ fail;

	err = -FDT_ERR_BADOFFSET;
	absoffset = stroffset + fdt_off_dt_ਲੜੀਆਂ(fdt);
	ਜੇ (absoffset >= (ਬਿਨਾਨਿਸ਼ਾਨੀ)totalsize)
		ਜਾਓ fail;
	len = totalsize - absoffset;

	ਜੇ (fdt_magic(fdt) == FDT_MAGIC) {
		ਜੇ (stroffset < 0)
			ਜਾਓ fail;
		ਜੇ (can_assume(LATEST) || fdt_ਸੰਸਕਰਣ(fdt) >= 17) {
			ਜੇ ((ਬਿਨਾਨਿਸ਼ਾਨੀ)stroffset >= fdt_ਮਾਪ_dt_ਲੜੀਆਂ(fdt))
				ਜਾਓ fail;
			ਜੇ ((fdt_ਮਾਪ_dt_ਲੜੀਆਂ(fdt) - stroffset) < len)
				len = fdt_ਮਾਪ_dt_ਲੜੀਆਂ(fdt) - stroffset;
		}
	} ਨਹੀਂਤਾਂ ਜੇ (fdt_magic(fdt) == FDT_SW_MAGIC) {
		ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ sw_stroffset = -stroffset;

		ਜੇ ((stroffset >= 0) ||
		    (sw_stroffset > fdt_ਮਾਪ_dt_ਲੜੀਆਂ(fdt)))
			ਜਾਓ fail;
		ਜੇ (sw_stroffset < len)
			len = sw_stroffset;
	} ਨਹੀਂਤਾਂ {
		err = -FDT_ERR_ਅੰਦਰੂਨੀ;
		ਜਾਓ fail;
	}

	s = (ਅਚਲ ਅੱਖਰ *)fdt + absoffset;
	n = memchr(s, '\0', len);
	ਜੇ (!n) {
		/* missing terminating NULL */
		err = -FDT_ERR_TRUNCATED;
		ਜਾਓ fail;
	}

	ਜੇ (lenp)
		*lenp = n - s;
	ਵਾਪਸ s;

fail:
	ਜੇ (lenp)
		*lenp = err;
	ਵਾਪਸ ਨਿਰਾ;
}

ਅਚਲ ਅੱਖਰ *fdt_string(ਅਚਲ ਖਾਲੀ *fdt, ਪੂਰਨ stroffset)
{
	ਵਾਪਸ fdt_get_string(fdt, stroffset, ਨਿਰਾ);
}

ਸਥਿਰ ਪੂਰਨ fdt_string_eq_(ਅਚਲ ਖਾਲੀ *fdt, ਪੂਰਨ stroffset,
			  ਅਚਲ ਅੱਖਰ *s, ਪੂਰਨ len)
{
	ਪੂਰਨ slen;
	ਅਚਲ ਅੱਖਰ *p = fdt_get_string(fdt, stroffset, &slen);

	ਵਾਪਸ p && (slen == len) && (memcmp(p, s, len) == 0);
}

ਪੂਰਨ fdt_ਲੱਭੋ_max_phandle(ਅਚਲ ਖਾਲੀ *fdt, uint32_t *phandle)
{
	uint32_t max = 0;
	ਪੂਰਨ ਵਿੱਥ = -1;

	ਜਦੋਂ (true) {
		uint32_t ਮੁੱਲ;

		ਵਿੱਥ = fdt_ਅਗਲਾ_ਗੰਢ(fdt, ਵਿੱਥ, ਨਿਰਾ);
		ਜੇ (ਵਿੱਥ < 0) {
			ਜੇ (ਵਿੱਥ == -FDT_ERR_NOTFOUND)
				ਰੋਕੋ;

			ਵਾਪਸ ਵਿੱਥ;
		}

		ਮੁੱਲ = fdt_get_phandle(fdt, ਵਿੱਥ);

		ਜੇ (ਮੁੱਲ > max)
			max = ਮੁੱਲ;
	}

	ਜੇ (phandle)
		*phandle = max;

	ਵਾਪਸ 0;
}

ਪੂਰਨ fdt_generate_phandle(ਅਚਲ ਖਾਲੀ *fdt, uint32_t *phandle)
{
	uint32_t max;
	ਪੂਰਨ err;

	err = fdt_ਲੱਭੋ_max_phandle(fdt, &max);
	ਜੇ (err < 0)
		ਵਾਪਸ err;

	ਜੇ (max == FDT_MAX_PHANDLE)
		ਵਾਪਸ -FDT_ERR_NOPHANDLES;

	ਜੇ (phandle)
		*phandle = max + 1;

	ਵਾਪਸ 0;
}

ਸਥਿਰ ਅਚਲ ਬਣਤਰ fdt_reserve_ਇੰਦਰਾਜ *fdt_mem_rsv(ਅਚਲ ਖਾਲੀ *fdt, ਪੂਰਨ n)
{
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ਵਿੱਥ = n * ਆਕਾਰ(ਬਣਤਰ fdt_reserve_ਇੰਦਰਾਜ);
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ absoffset = fdt_off_mem_rsvmap(fdt) + ਵਿੱਥ;

	ਜੇ (!can_assume(VALID_ਨਿਵੇਸ਼)) {
		ਜੇ (absoffset < fdt_off_mem_rsvmap(fdt))
			ਵਾਪਸ ਨਿਰਾ;
		ਜੇ (absoffset > fdt_totalsize(fdt) -
		    ਆਕਾਰ(ਬਣਤਰ fdt_reserve_ਇੰਦਰਾਜ))
			ਵਾਪਸ ਨਿਰਾ;
	}
	ਵਾਪਸ fdt_mem_rsv_(fdt, n);
}

ਪੂਰਨ fdt_get_mem_rsv(ਅਚਲ ਖਾਲੀ *fdt, ਪੂਰਨ n, uint64_t *ਪਤਾ, uint64_t *ਮਾਪ)
{
	ਅਚਲ ਬਣਤਰ fdt_reserve_ਇੰਦਰਾਜ *re;

	FDT_RO_PROBE(fdt);
	re = fdt_mem_rsv(fdt, n);
	ਜੇ (!can_assume(VALID_ਨਿਵੇਸ਼) && !re)
		ਵਾਪਸ -FDT_ERR_BADOFFSET;

	*ਪਤਾ = fdt64_ld_(&re->ਪਤਾ);
	*ਮਾਪ = fdt64_ld_(&re->ਮਾਪ);
	ਵਾਪਸ 0;
}

ਪੂਰਨ fdt_ਅੰਕ_mem_rsv(ਅਚਲ ਖਾਲੀ *fdt)
{
	ਪੂਰਨ i;
	ਅਚਲ ਬਣਤਰ fdt_reserve_ਇੰਦਰਾਜ *re;

	ਲਈ (i = 0; (re = fdt_mem_rsv(fdt, i)) != ਨਿਰਾ; i++) {
		ਜੇ (fdt64_ld_(&re->ਮਾਪ) == 0)
			ਵਾਪਸ i;
	}
	ਵਾਪਸ -FDT_ERR_TRUNCATED;
}

ਸਥਿਰ ਪੂਰਨ nextprop_(ਅਚਲ ਖਾਲੀ *fdt, ਪੂਰਨ ਵਿੱਥ)
{
	uint32_t ਟੈਗ;
	ਪੂਰਨ nextoffset;

	ਕਰੋ {
		ਟੈਗ = fdt_ਅਗਲਾ_ਟੈਗ(fdt, ਵਿੱਥ, &nextoffset);

		ਚੋਣ (ਟੈਗ) {
		ਹਾਲਤ FDT_ਸਿਰਾ:
			ਜੇ (nextoffset >= 0)
				ਵਾਪਸ -FDT_ERR_BADSTRUCTURE;
			ਨਹੀਂਤਾਂ
				ਵਾਪਸ nextoffset;

		ਹਾਲਤ FDT_PROP:
			ਵਾਪਸ ਵਿੱਥ;
		}
		ਵਿੱਥ = nextoffset;
	} ਜਦੋਂ (ਟੈਗ == FDT_NOP);

	ਵਾਪਸ -FDT_ERR_NOTFOUND;
}

ਪੂਰਨ fdt_subnode_ਵਿੱਥ_namelen(ਅਚਲ ਖਾਲੀ *fdt, ਪੂਰਨ ਵਿੱਥ,
			       ਅਚਲ ਅੱਖਰ *name, ਪੂਰਨ namelen)
{
	ਪੂਰਨ ਡੂੰਘਾਈ;

	FDT_RO_PROBE(fdt);

	ਲਈ (ਡੂੰਘਾਈ = 0;
	     (ਵਿੱਥ >= 0) && (ਡੂੰਘਾਈ >= 0);
	     ਵਿੱਥ = fdt_ਅਗਲਾ_ਗੰਢ(fdt, ਵਿੱਥ, &ਡੂੰਘਾਈ))
		ਜੇ ((ਡੂੰਘਾਈ == 1)
		    && fdt_nodename_eq_(fdt, ਵਿੱਥ, name, namelen))
			ਵਾਪਸ ਵਿੱਥ;

	ਜੇ (ਡੂੰਘਾਈ < 0)
		ਵਾਪਸ -FDT_ERR_NOTFOUND;
	ਵਾਪਸ ਵਿੱਥ; /* error */
}

ਪੂਰਨ fdt_subnode_ਵਿੱਥ(ਅਚਲ ਖਾਲੀ *fdt, ਪੂਰਨ parentoffset,
		       ਅਚਲ ਅੱਖਰ *name)
{
	ਵਾਪਸ fdt_subnode_ਵਿੱਥ_namelen(fdt, parentoffset, name, ਲੜੀ_ਲੰਬਾਈ(name));
}

ਪੂਰਨ fdt_ਰਸਤਾ_ਵਿੱਥ_namelen(ਅਚਲ ਖਾਲੀ *fdt, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਪੂਰਨ namelen)
{
	ਅਚਲ ਅੱਖਰ *ਸਿਰਾ = ਰਸਤਾ + namelen;
	ਅਚਲ ਅੱਖਰ *p = ਰਸਤਾ;
	ਪੂਰਨ ਵਿੱਥ = 0;

	FDT_RO_PROBE(fdt);

	/* see if we have an alias */
	ਜੇ (*ਰਸਤਾ != '/') {
		ਅਚਲ ਅੱਖਰ *q = memchr(ਰਸਤਾ, '/', ਸਿਰਾ - p);

		ਜੇ (!q)
			q = ਸਿਰਾ;

		p = fdt_get_alias_namelen(fdt, p, q - p);
		ਜੇ (!p)
			ਵਾਪਸ -FDT_ERR_BADPATH;
		ਵਿੱਥ = fdt_ਰਸਤਾ_ਵਿੱਥ(fdt, p);

		p = q;
	}

	ਜਦੋਂ (p < ਸਿਰਾ) {
		ਅਚਲ ਅੱਖਰ *q;

		ਜਦੋਂ (*p == '/') {
			p++;
			ਜੇ (p == ਸਿਰਾ)
				ਵਾਪਸ ਵਿੱਥ;
		}
		q = memchr(p, '/', ਸਿਰਾ - p);
		ਜੇ (! q)
			q = ਸਿਰਾ;

		ਵਿੱਥ = fdt_subnode_ਵਿੱਥ_namelen(fdt, ਵਿੱਥ, p, q-p);
		ਜੇ (ਵਿੱਥ < 0)
			ਵਾਪਸ ਵਿੱਥ;

		p = q;
	}

	ਵਾਪਸ ਵਿੱਥ;
}

ਪੂਰਨ fdt_ਰਸਤਾ_ਵਿੱਥ(ਅਚਲ ਖਾਲੀ *fdt, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ)
{
	ਵਾਪਸ fdt_ਰਸਤਾ_ਵਿੱਥ_namelen(fdt, ਰਸਤਾ, ਲੜੀ_ਲੰਬਾਈ(ਰਸਤਾ));
}

ਅਚਲ ਅੱਖਰ *fdt_get_name(ਅਚਲ ਖਾਲੀ *fdt, ਪੂਰਨ nodeoffset, ਪੂਰਨ *len)
{
	ਅਚਲ ਬਣਤਰ fdt_ਗੰਢ_ਸਿਰਲੇਖ *nh = fdt_ਵਿੱਥ_ptr_2(fdt, nodeoffset);
	ਅਚਲ ਅੱਖਰ *nameptr;
	ਪੂਰਨ err;

	ਜੇ (((err = fdt_ro_probe_(fdt)) < 0)
	    || ((err = fdt_check_ਗੰਢ_ਵਿੱਥ(fdt, nodeoffset)) < 0))
			ਜਾਓ fail;

	nameptr = nh->name;

	ਜੇ (!can_assume(LATEST) && fdt_ਸੰਸਕਰਣ(fdt) < 0x10) {
		/*
		 * For old FDT versions, match the naming conventions of V16:
		 * give only the leaf name (after all /). The actual tree
		 * contents are loosely checked.
		 */
		ਅਚਲ ਅੱਖਰ *ਪੱਤੀ;
		ਪੱਤੀ = strrchr(nameptr, '/');
		ਜੇ (ਪੱਤੀ == ਨਿਰਾ) {
			err = -FDT_ERR_BADSTRUCTURE;
			ਜਾਓ fail;
		}
		nameptr = ਪੱਤੀ+1;
	}

	ਜੇ (len)
		*len = ਲੜੀ_ਲੰਬਾਈ(nameptr);

	ਵਾਪਸ nameptr;

 fail:
	ਜੇ (len)
		*len = err;
	ਵਾਪਸ ਨਿਰਾ;
}

ਪੂਰਨ fdt_ਪਹਿਲਾ_property_ਵਿੱਥ(ਅਚਲ ਖਾਲੀ *fdt, ਪੂਰਨ nodeoffset)
{
	ਪੂਰਨ ਵਿੱਥ;

	ਜੇ ((ਵਿੱਥ = fdt_check_ਗੰਢ_ਵਿੱਥ(fdt, nodeoffset)) < 0)
		ਵਾਪਸ ਵਿੱਥ;

	ਵਾਪਸ nextprop_(fdt, ਵਿੱਥ);
}

ਪੂਰਨ fdt_ਅਗਲਾ_property_ਵਿੱਥ(ਅਚਲ ਖਾਲੀ *fdt, ਪੂਰਨ ਵਿੱਥ)
{
	ਜੇ ((ਵਿੱਥ = fdt_check_prop_ਵਿੱਥ(fdt, ਵਿੱਥ)) < 0)
		ਵਾਪਸ ਵਿੱਥ;

	ਵਾਪਸ nextprop_(fdt, ਵਿੱਥ);
}

ਸਥਿਰ ਅਚਲ ਬਣਤਰ fdt_property *fdt_get_property_by_ਵਿੱਥ(ਅਚਲ ਖਾਲੀ *fdt,
						              ਪੂਰਨ ਵਿੱਥ,
						              ਪੂਰਨ *lenp)
{
	ਪੂਰਨ err;
	ਅਚਲ ਬਣਤਰ fdt_property *prop;

	ਜੇ (!can_assume(VALID_ਨਿਵੇਸ਼) &&
	    (err = fdt_check_prop_ਵਿੱਥ(fdt, ਵਿੱਥ)) < 0) {
		ਜੇ (lenp)
			*lenp = err;
		ਵਾਪਸ ਨਿਰਾ;
	}

	prop = fdt_ਵਿੱਥ_ptr_2(fdt, ਵਿੱਥ);

	ਜੇ (lenp)
		*lenp = fdt32_ld_(&prop->len);

	ਵਾਪਸ prop;
}

ਅਚਲ ਬਣਤਰ fdt_property *fdt_get_property_by_ਵਿੱਥ_2(ਅਚਲ ਖਾਲੀ *fdt,
						      ਪੂਰਨ ਵਿੱਥ,
						      ਪੂਰਨ *lenp)
{
	/* Prior to version 16, properties may need realignment
	 * and this API does not work. fdt_getprop_*() will, however. */

	ਜੇ (!can_assume(LATEST) && fdt_ਸੰਸਕਰਣ(fdt) < 0x10) {
		ਜੇ (lenp)
			*lenp = -FDT_ERR_BADVERSION;
		ਵਾਪਸ ਨਿਰਾ;
	}

	ਵਾਪਸ fdt_get_property_by_ਵਿੱਥ(fdt, ਵਿੱਥ, lenp);
}

ਸਥਿਰ ਅਚਲ ਬਣਤਰ fdt_property *fdt_get_property_namelen_(ਅਚਲ ਖਾਲੀ *fdt,
						            ਪੂਰਨ ਵਿੱਥ,
						            ਅਚਲ ਅੱਖਰ *name,
						            ਪੂਰਨ namelen,
							    ਪੂਰਨ *lenp,
							    ਪੂਰਨ *poffset)
{
	ਲਈ (ਵਿੱਥ = fdt_ਪਹਿਲਾ_property_ਵਿੱਥ(fdt, ਵਿੱਥ);
	     (ਵਿੱਥ >= 0);
	     (ਵਿੱਥ = fdt_ਅਗਲਾ_property_ਵਿੱਥ(fdt, ਵਿੱਥ))) {
		ਅਚਲ ਬਣਤਰ fdt_property *prop;

		prop = fdt_get_property_by_ਵਿੱਥ(fdt, ਵਿੱਥ, lenp);
		ਜੇ (!can_assume(LIBFDT_FLAWLESS) && !prop) {
			ਵਿੱਥ = -FDT_ERR_ਅੰਦਰੂਨੀ;
			ਰੋਕੋ;
		}
		ਜੇ (fdt_string_eq_(fdt, fdt32_ld_(&prop->nameoff),
				   name, namelen)) {
			ਜੇ (poffset)
				*poffset = ਵਿੱਥ;
			ਵਾਪਸ prop;
		}
	}

	ਜੇ (lenp)
		*lenp = ਵਿੱਥ;
	ਵਾਪਸ ਨਿਰਾ;
}


ਅਚਲ ਬਣਤਰ fdt_property *fdt_get_property_namelen(ਅਚਲ ਖਾਲੀ *fdt,
						    ਪੂਰਨ ਵਿੱਥ,
						    ਅਚਲ ਅੱਖਰ *name,
						    ਪੂਰਨ namelen, ਪੂਰਨ *lenp)
{
	/* Prior to version 16, properties may need realignment
	 * and this API does not work. fdt_getprop_*() will, however. */
	ਜੇ (!can_assume(LATEST) && fdt_ਸੰਸਕਰਣ(fdt) < 0x10) {
		ਜੇ (lenp)
			*lenp = -FDT_ERR_BADVERSION;
		ਵਾਪਸ ਨਿਰਾ;
	}

	ਵਾਪਸ fdt_get_property_namelen_(fdt, ਵਿੱਥ, name, namelen, lenp,
					 ਨਿਰਾ);
}


ਅਚਲ ਬਣਤਰ fdt_property *fdt_get_property(ਅਚਲ ਖਾਲੀ *fdt,
					    ਪੂਰਨ nodeoffset,
					    ਅਚਲ ਅੱਖਰ *name, ਪੂਰਨ *lenp)
{
	ਵਾਪਸ fdt_get_property_namelen(fdt, nodeoffset, name,
					ਲੜੀ_ਲੰਬਾਈ(name), lenp);
}

ਅਚਲ ਖਾਲੀ *fdt_getprop_namelen(ਅਚਲ ਖਾਲੀ *fdt, ਪੂਰਨ nodeoffset,
				ਅਚਲ ਅੱਖਰ *name, ਪੂਰਨ namelen, ਪੂਰਨ *lenp)
{
	ਪੂਰਨ poffset;
	ਅਚਲ ਬਣਤਰ fdt_property *prop;

	prop = fdt_get_property_namelen_(fdt, nodeoffset, name, namelen, lenp,
					 &poffset);
	ਜੇ (!prop)
		ਵਾਪਸ ਨਿਰਾ;

	/* Handle realignment */
	ਜੇ (!can_assume(LATEST) && fdt_ਸੰਸਕਰਣ(fdt) < 0x10 &&
	    (poffset + ਆਕਾਰ(*prop)) % 8 && fdt32_ld_(&prop->len) >= 8)
		ਵਾਪਸ prop->ਅੰਕੜੇ + 4;
	ਵਾਪਸ prop->ਅੰਕੜੇ;
}

ਅਚਲ ਖਾਲੀ *fdt_getprop_by_ਵਿੱਥ(ਅਚਲ ਖਾਲੀ *fdt, ਪੂਰਨ ਵਿੱਥ,
				  ਅਚਲ ਅੱਖਰ **namep, ਪੂਰਨ *lenp)
{
	ਅਚਲ ਬਣਤਰ fdt_property *prop;

	prop = fdt_get_property_by_ਵਿੱਥ(fdt, ਵਿੱਥ, lenp);
	ਜੇ (!prop)
		ਵਾਪਸ ਨਿਰਾ;
	ਜੇ (namep) {
		ਅਚਲ ਅੱਖਰ *name;
		ਪੂਰਨ namelen;

		ਜੇ (!can_assume(VALID_ਨਿਵੇਸ਼)) {
			name = fdt_get_string(fdt, fdt32_ld_(&prop->nameoff),
					      &namelen);
			ਜੇ (!name) {
				ਜੇ (lenp)
					*lenp = namelen;
				ਵਾਪਸ ਨਿਰਾ;
			}
			*namep = name;
		} ਨਹੀਂਤਾਂ {
			*namep = fdt_string(fdt, fdt32_ld_(&prop->nameoff));
		}
	}

	/* Handle realignment */
	ਜੇ (!can_assume(LATEST) && fdt_ਸੰਸਕਰਣ(fdt) < 0x10 &&
	    (ਵਿੱਥ + ਆਕਾਰ(*prop)) % 8 && fdt32_ld_(&prop->len) >= 8)
		ਵਾਪਸ prop->ਅੰਕੜੇ + 4;
	ਵਾਪਸ prop->ਅੰਕੜੇ;
}

ਅਚਲ ਖਾਲੀ *fdt_getprop(ਅਚਲ ਖਾਲੀ *fdt, ਪੂਰਨ nodeoffset,
			ਅਚਲ ਅੱਖਰ *name, ਪੂਰਨ *lenp)
{
	ਵਾਪਸ fdt_getprop_namelen(fdt, nodeoffset, name, ਲੜੀ_ਲੰਬਾਈ(name), lenp);
}

uint32_t fdt_get_phandle(ਅਚਲ ਖਾਲੀ *fdt, ਪੂਰਨ nodeoffset)
{
	ਅਚਲ fdt32_t *php;
	ਪੂਰਨ len;

	/* FIXME: This is a bit sub-optimal, since we potentially scan
	 * over all the properties twice. */
	php = fdt_getprop(fdt, nodeoffset, "phandle", &len);
	ਜੇ (!php || (len != ਆਕਾਰ(*php))) {
		php = fdt_getprop(fdt, nodeoffset, "linux,phandle", &len);
		ਜੇ (!php || (len != ਆਕਾਰ(*php)))
			ਵਾਪਸ 0;
	}

	ਵਾਪਸ fdt32_ld_(php);
}

ਅਚਲ ਅੱਖਰ *fdt_get_alias_namelen(ਅਚਲ ਖਾਲੀ *fdt,
				  ਅਚਲ ਅੱਖਰ *name, ਪੂਰਨ namelen)
{
	ਪੂਰਨ aliasoffset;

	aliasoffset = fdt_ਰਸਤਾ_ਵਿੱਥ(fdt, "/aliases");
	ਜੇ (aliasoffset < 0)
		ਵਾਪਸ ਨਿਰਾ;

	ਵਾਪਸ fdt_getprop_namelen(fdt, aliasoffset, name, namelen, ਨਿਰਾ);
}

ਅਚਲ ਅੱਖਰ *fdt_get_alias(ਅਚਲ ਖਾਲੀ *fdt, ਅਚਲ ਅੱਖਰ *name)
{
	ਵਾਪਸ fdt_get_alias_namelen(fdt, name, ਲੜੀ_ਲੰਬਾਈ(name));
}

ਪੂਰਨ fdt_get_ਰਸਤਾ(ਅਚਲ ਖਾਲੀ *fdt, ਪੂਰਨ nodeoffset, ਅੱਖਰ *buf, ਪੂਰਨ buflen)
{
	ਪੂਰਨ pdepth = 0, p = 0;
	ਪੂਰਨ ਵਿੱਥ, ਡੂੰਘਾਈ, namelen;
	ਅਚਲ ਅੱਖਰ *name;

	FDT_RO_PROBE(fdt);

	ਜੇ (buflen < 2)
		ਵਾਪਸ -FDT_ERR_NOSPACE;

	ਲਈ (ਵਿੱਥ = 0, ਡੂੰਘਾਈ = 0;
	     (ਵਿੱਥ >= 0) && (ਵਿੱਥ <= nodeoffset);
	     ਵਿੱਥ = fdt_ਅਗਲਾ_ਗੰਢ(fdt, ਵਿੱਥ, &ਡੂੰਘਾਈ)) {
		ਜਦੋਂ (pdepth > ਡੂੰਘਾਈ) {
			ਕਰੋ {
				p--;
			} ਜਦੋਂ (buf[p-1] != '/');
			pdepth--;
		}

		ਜੇ (pdepth >= ਡੂੰਘਾਈ) {
			name = fdt_get_name(fdt, ਵਿੱਥ, &namelen);
			ਜੇ (!name)
				ਵਾਪਸ namelen;
			ਜੇ ((p + namelen + 1) <= buflen) {
				ਸਮ੍ਰਿਤੀ_ਨਕਲ(buf + p, name, namelen);
				p += namelen;
				buf[p++] = '/';
				pdepth++;
			}
		}

		ਜੇ (ਵਿੱਥ == nodeoffset) {
			ਜੇ (pdepth < (ਡੂੰਘਾਈ + 1))
				ਵਾਪਸ -FDT_ERR_NOSPACE;

			ਜੇ (p > 1) /* special case so that root path is "/", not "" */
				p--;
			buf[p] = '\0';
			ਵਾਪਸ 0;
		}
	}

	ਜੇ ((ਵਿੱਥ == -FDT_ERR_NOTFOUND) || (ਵਿੱਥ >= 0))
		ਵਾਪਸ -FDT_ERR_BADOFFSET;
	ਨਹੀਂਤਾਂ ਜੇ (ਵਿੱਥ == -FDT_ERR_BADOFFSET)
		ਵਾਪਸ -FDT_ERR_BADSTRUCTURE;

	ਵਾਪਸ ਵਿੱਥ; /* error from fdt_next_node() */
}

ਪੂਰਨ fdt_supernode_atdepth_ਵਿੱਥ(ਅਚਲ ਖਾਲੀ *fdt, ਪੂਰਨ nodeoffset,
				 ਪੂਰਨ supernodedepth, ਪੂਰਨ *nodedepth)
{
	ਪੂਰਨ ਵਿੱਥ, ਡੂੰਘਾਈ;
	ਪੂਰਨ supernodeoffset = -FDT_ERR_ਅੰਦਰੂਨੀ;

	FDT_RO_PROBE(fdt);

	ਜੇ (supernodedepth < 0)
		ਵਾਪਸ -FDT_ERR_NOTFOUND;

	ਲਈ (ਵਿੱਥ = 0, ਡੂੰਘਾਈ = 0;
	     (ਵਿੱਥ >= 0) && (ਵਿੱਥ <= nodeoffset);
	     ਵਿੱਥ = fdt_ਅਗਲਾ_ਗੰਢ(fdt, ਵਿੱਥ, &ਡੂੰਘਾਈ)) {
		ਜੇ (ਡੂੰਘਾਈ == supernodedepth)
			supernodeoffset = ਵਿੱਥ;

		ਜੇ (ਵਿੱਥ == nodeoffset) {
			ਜੇ (nodedepth)
				*nodedepth = ਡੂੰਘਾਈ;

			ਜੇ (supernodedepth > ਡੂੰਘਾਈ)
				ਵਾਪਸ -FDT_ERR_NOTFOUND;
			ਨਹੀਂਤਾਂ
				ਵਾਪਸ supernodeoffset;
		}
	}

	ਜੇ (!can_assume(VALID_ਨਿਵੇਸ਼)) {
		ਜੇ ((ਵਿੱਥ == -FDT_ERR_NOTFOUND) || (ਵਿੱਥ >= 0))
			ਵਾਪਸ -FDT_ERR_BADOFFSET;
		ਨਹੀਂਤਾਂ ਜੇ (ਵਿੱਥ == -FDT_ERR_BADOFFSET)
			ਵਾਪਸ -FDT_ERR_BADSTRUCTURE;
	}

	ਵਾਪਸ ਵਿੱਥ; /* error from fdt_next_node() */
}

ਪੂਰਨ fdt_ਗੰਢ_ਡੂੰਘਾਈ(ਅਚਲ ਖਾਲੀ *fdt, ਪੂਰਨ nodeoffset)
{
	ਪੂਰਨ nodedepth;
	ਪੂਰਨ err;

	err = fdt_supernode_atdepth_ਵਿੱਥ(fdt, nodeoffset, 0, &nodedepth);
	ਜੇ (err)
		ਵਾਪਸ (can_assume(LIBFDT_FLAWLESS) || err < 0) ? err :
			-FDT_ERR_ਅੰਦਰੂਨੀ;
	ਵਾਪਸ nodedepth;
}

ਪੂਰਨ fdt_ਮਾਪਾ_ਵਿੱਥ(ਅਚਲ ਖਾਲੀ *fdt, ਪੂਰਨ nodeoffset)
{
	ਪੂਰਨ nodedepth = fdt_ਗੰਢ_ਡੂੰਘਾਈ(fdt, nodeoffset);

	ਜੇ (nodedepth < 0)
		ਵਾਪਸ nodedepth;
	ਵਾਪਸ fdt_supernode_atdepth_ਵਿੱਥ(fdt, nodeoffset,
					    nodedepth - 1, ਨਿਰਾ);
}

ਪੂਰਨ fdt_ਗੰਢ_ਵਿੱਥ_by_prop_ਮੁੱਲ(ਅਚਲ ਖਾਲੀ *fdt, ਪੂਰਨ startoffset,
				  ਅਚਲ ਅੱਖਰ *propname,
				  ਅਚਲ ਖਾਲੀ *propval, ਪੂਰਨ proplen)
{
	ਪੂਰਨ ਵਿੱਥ;
	ਅਚਲ ਖਾਲੀ *val;
	ਪੂਰਨ len;

	FDT_RO_PROBE(fdt);

	/* FIXME: The algorithm here is pretty horrible: we scan each
	 * property of a node in fdt_getprop(), then if that didn't
	 * find what we want, we scan over them again making our way
	 * to the next node.  Still it's the easiest to implement
	 * approach; performance can come later. */
	ਲਈ (ਵਿੱਥ = fdt_ਅਗਲਾ_ਗੰਢ(fdt, startoffset, ਨਿਰਾ);
	     ਵਿੱਥ >= 0;
	     ਵਿੱਥ = fdt_ਅਗਲਾ_ਗੰਢ(fdt, ਵਿੱਥ, ਨਿਰਾ)) {
		val = fdt_getprop(fdt, ਵਿੱਥ, propname, &len);
		ਜੇ (val && (len == proplen)
		    && (memcmp(val, propval, len) == 0))
			ਵਾਪਸ ਵਿੱਥ;
	}

	ਵਾਪਸ ਵਿੱਥ; /* error from fdt_next_node() */
}

ਪੂਰਨ fdt_ਗੰਢ_ਵਿੱਥ_by_phandle(ਅਚਲ ਖਾਲੀ *fdt, uint32_t phandle)
{
	ਪੂਰਨ ਵਿੱਥ;

	ਜੇ ((phandle == 0) || (phandle == ~0U))
		ਵਾਪਸ -FDT_ERR_BADPHANDLE;

	FDT_RO_PROBE(fdt);

	/* FIXME: The algorithm here is pretty horrible: we
	 * potentially scan each property of a node in
	 * fdt_get_phandle(), then if that didn't find what
	 * we want, we scan over them again making our way to the next
	 * node.  Still it's the easiest to implement approach;
	 * performance can come later. */
	ਲਈ (ਵਿੱਥ = fdt_ਅਗਲਾ_ਗੰਢ(fdt, -1, ਨਿਰਾ);
	     ਵਿੱਥ >= 0;
	     ਵਿੱਥ = fdt_ਅਗਲਾ_ਗੰਢ(fdt, ਵਿੱਥ, ਨਿਰਾ)) {
		ਜੇ (fdt_get_phandle(fdt, ਵਿੱਥ) == phandle)
			ਵਾਪਸ ਵਿੱਥ;
	}

	ਵਾਪਸ ਵਿੱਥ; /* error from fdt_next_node() */
}

ਪੂਰਨ fdt_stringlist_ਸ਼ਾਮਲ_ਹੈ(ਅਚਲ ਅੱਖਰ *strlist, ਪੂਰਨ listlen, ਅਚਲ ਅੱਖਰ *str)
{
	ਪੂਰਨ len = ਲੜੀ_ਲੰਬਾਈ(str);
	ਅਚਲ ਅੱਖਰ *p;

	ਜਦੋਂ (listlen >= len) {
		ਜੇ (memcmp(str, strlist, len+1) == 0)
			ਵਾਪਸ 1;
		p = memchr(strlist, '\0', listlen);
		ਜੇ (!p)
			ਵਾਪਸ 0; /* malformed strlist.. */
		listlen -= (p-strlist) + 1;
		strlist = p + 1;
	}
	ਵਾਪਸ 0;
}

ਪੂਰਨ fdt_stringlist_ਗਿਣਤੀ(ਅਚਲ ਖਾਲੀ *fdt, ਪੂਰਨ nodeoffset, ਅਚਲ ਅੱਖਰ *property)
{
	ਅਚਲ ਅੱਖਰ *list, *ਸਿਰਾ;
	ਪੂਰਨ ਲੰਮਾਈ, ਗਿਣਤੀ_2 = 0;

	list = fdt_getprop(fdt, nodeoffset, property, &ਲੰਮਾਈ);
	ਜੇ (!list)
		ਵਾਪਸ ਲੰਮਾਈ;

	ਸਿਰਾ = list + ਲੰਮਾਈ;

	ਜਦੋਂ (list < ਸਿਰਾ) {
		ਲੰਮਾਈ = strnlen(list, ਸਿਰਾ - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		ਜੇ (list + ਲੰਮਾਈ > ਸਿਰਾ)
			ਵਾਪਸ -FDT_ERR_BADVALUE;

		list += ਲੰਮਾਈ;
		ਗਿਣਤੀ_2++;
	}

	ਵਾਪਸ ਗਿਣਤੀ_2;
}

ਪੂਰਨ fdt_stringlist_ਖੋਜ(ਅਚਲ ਖਾਲੀ *fdt, ਪੂਰਨ nodeoffset, ਅਚਲ ਅੱਖਰ *property,
			  ਅਚਲ ਅੱਖਰ *string)
{
	ਪੂਰਨ ਲੰਮਾਈ, len, idx = 0;
	ਅਚਲ ਅੱਖਰ *list, *ਸਿਰਾ;

	list = fdt_getprop(fdt, nodeoffset, property, &ਲੰਮਾਈ);
	ਜੇ (!list)
		ਵਾਪਸ ਲੰਮਾਈ;

	len = ਲੜੀ_ਲੰਬਾਈ(string) + 1;
	ਸਿਰਾ = list + ਲੰਮਾਈ;

	ਜਦੋਂ (list < ਸਿਰਾ) {
		ਲੰਮਾਈ = strnlen(list, ਸਿਰਾ - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		ਜੇ (list + ਲੰਮਾਈ > ਸਿਰਾ)
			ਵਾਪਸ -FDT_ERR_BADVALUE;

		ਜੇ (ਲੰਮਾਈ == len && memcmp(list, string, ਲੰਮਾਈ) == 0)
			ਵਾਪਸ idx;

		list += ਲੰਮਾਈ;
		idx++;
	}

	ਵਾਪਸ -FDT_ERR_NOTFOUND;
}

ਅਚਲ ਅੱਖਰ *fdt_stringlist_get(ਅਚਲ ਖਾਲੀ *fdt, ਪੂਰਨ nodeoffset,
			       ਅਚਲ ਅੱਖਰ *property, ਪੂਰਨ idx,
			       ਪੂਰਨ *lenp)
{
	ਅਚਲ ਅੱਖਰ *list, *ਸਿਰਾ;
	ਪੂਰਨ ਲੰਮਾਈ;

	list = fdt_getprop(fdt, nodeoffset, property, &ਲੰਮਾਈ);
	ਜੇ (!list) {
		ਜੇ (lenp)
			*lenp = ਲੰਮਾਈ;

		ਵਾਪਸ ਨਿਰਾ;
	}

	ਸਿਰਾ = list + ਲੰਮਾਈ;

	ਜਦੋਂ (list < ਸਿਰਾ) {
		ਲੰਮਾਈ = strnlen(list, ਸਿਰਾ - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		ਜੇ (list + ਲੰਮਾਈ > ਸਿਰਾ) {
			ਜੇ (lenp)
				*lenp = -FDT_ERR_BADVALUE;

			ਵਾਪਸ ਨਿਰਾ;
		}

		ਜੇ (idx == 0) {
			ਜੇ (lenp)
				*lenp = ਲੰਮਾਈ - 1;

			ਵਾਪਸ list;
		}

		list += ਲੰਮਾਈ;
		idx--;
	}

	ਜੇ (lenp)
		*lenp = -FDT_ERR_NOTFOUND;

	ਵਾਪਸ ਨਿਰਾ;
}

ਪੂਰਨ fdt_ਗੰਢ_check_compatible(ਅਚਲ ਖਾਲੀ *fdt, ਪੂਰਨ nodeoffset,
			      ਅਚਲ ਅੱਖਰ *compatible)
{
	ਅਚਲ ਖਾਲੀ *prop;
	ਪੂਰਨ len;

	prop = fdt_getprop(fdt, nodeoffset, "compatible", &len);
	ਜੇ (!prop)
		ਵਾਪਸ len;

	ਵਾਪਸ !fdt_stringlist_ਸ਼ਾਮਲ_ਹੈ(prop, len, compatible);
}

ਪੂਰਨ fdt_ਗੰਢ_ਵਿੱਥ_by_compatible(ਅਚਲ ਖਾਲੀ *fdt, ਪੂਰਨ startoffset,
				  ਅਚਲ ਅੱਖਰ *compatible)
{
	ਪੂਰਨ ਵਿੱਥ, err;

	FDT_RO_PROBE(fdt);

	/* FIXME: The algorithm here is pretty horrible: we scan each
	 * property of a node in fdt_node_check_compatible(), then if
	 * that didn't find what we want, we scan over them again
	 * making our way to the next node.  Still it's the easiest to
	 * implement approach; performance can come later. */
	ਲਈ (ਵਿੱਥ = fdt_ਅਗਲਾ_ਗੰਢ(fdt, startoffset, ਨਿਰਾ);
	     ਵਿੱਥ >= 0;
	     ਵਿੱਥ = fdt_ਅਗਲਾ_ਗੰਢ(fdt, ਵਿੱਥ, ਨਿਰਾ)) {
		err = fdt_ਗੰਢ_check_compatible(fdt, ਵਿੱਥ, compatible);
		ਜੇ ((err < 0) && (err != -FDT_ERR_NOTFOUND))
			ਵਾਪਸ err;
		ਨਹੀਂਤਾਂ ਜੇ (err == 0)
			ਵਾਪਸ ਵਿੱਥ;
	}

	ਵਾਪਸ ਵਿੱਥ; /* error from fdt_next_node() */
}
