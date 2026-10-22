// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#अन्तर्भाव "libfdt_env.h"

#अन्तर्भाव <fdt.h>
#अन्तर्भाव <libfdt.h>

#अन्तर्भाव "libfdt_internal.h"

स्थिर पूर्णाङ्क fdt_nodename_eq_(ध्रुव शून्य *fdt, पूर्णाङ्क विस्थापनम्,
			    ध्रुव वर्ण *s, पूर्णाङ्क len)
{
	पूर्णाङ्क olen;
	ध्रुव वर्ण *p = fdt_get_name(fdt, विस्थापनम्, &olen);

	यदि (!p || olen < len)
		/* short match */
		प्रत्यागम 0;

	यदि (memcmp(p, s, len) != 0)
		प्रत्यागम 0;

	यदि (p[len] == '\0')
		प्रत्यागम 1;
	अन्यथा यदि (!memchr(s, '@', len) && (p[len] == '@'))
		प्रत्यागम 1;
	अन्यथा
		प्रत्यागम 0;
}

ध्रुव वर्ण *fdt_get_string(ध्रुव शून्य *fdt, पूर्णाङ्क stroffset, पूर्णाङ्क *lenp)
{
	int32_t totalsize;
	uint32_t absoffset;
	size_t len;
	पूर्णाङ्क err;
	ध्रुव वर्ण *s, *n;

	यदि (can_assume(VALID_निवेशः)) {
		s = (ध्रुव वर्ण *)fdt + fdt_off_dt_वाक्यानि(fdt) + stroffset;

		यदि (lenp)
			*lenp = वाक्यदैर्घ्यम्(s);
		प्रत्यागम s;
	}
	totalsize = fdt_ro_probe_(fdt);
	err = totalsize;
	यदि (totalsize < 0)
		गच्छ fail;

	err = -FDT_ERR_BADOFFSET;
	absoffset = stroffset + fdt_off_dt_वाक्यानि(fdt);
	यदि (absoffset >= (अचिह्नित)totalsize)
		गच्छ fail;
	len = totalsize - absoffset;

	यदि (fdt_magic(fdt) == FDT_MAGIC) {
		यदि (stroffset < 0)
			गच्छ fail;
		यदि (can_assume(LATEST) || fdt_संस्करणम्(fdt) >= 17) {
			यदि ((अचिह्नित)stroffset >= fdt_आकार_dt_वाक्यानि(fdt))
				गच्छ fail;
			यदि ((fdt_आकार_dt_वाक्यानि(fdt) - stroffset) < len)
				len = fdt_आकार_dt_वाक्यानि(fdt) - stroffset;
		}
	} अन्यथा यदि (fdt_magic(fdt) == FDT_SW_MAGIC) {
		अचिह्नित पूर्णाङ्क sw_stroffset = -stroffset;

		यदि ((stroffset >= 0) ||
		    (sw_stroffset > fdt_आकार_dt_वाक्यानि(fdt)))
			गच्छ fail;
		यदि (sw_stroffset < len)
			len = sw_stroffset;
	} अन्यथा {
		err = -FDT_ERR_आन्तरिकम्;
		गच्छ fail;
	}

	s = (ध्रुव वर्ण *)fdt + absoffset;
	n = memchr(s, '\0', len);
	यदि (!n) {
		/* missing terminating NULL */
		err = -FDT_ERR_TRUNCATED;
		गच्छ fail;
	}

	यदि (lenp)
		*lenp = n - s;
	प्रत्यागम s;

fail:
	यदि (lenp)
		*lenp = err;
	प्रत्यागम रिक्त;
}

ध्रुव वर्ण *fdt_string(ध्रुव शून्य *fdt, पूर्णाङ्क stroffset)
{
	प्रत्यागम fdt_get_string(fdt, stroffset, रिक्त);
}

स्थिर पूर्णाङ्क fdt_string_eq_(ध्रुव शून्य *fdt, पूर्णाङ्क stroffset,
			  ध्रुव वर्ण *s, पूर्णाङ्क len)
{
	पूर्णाङ्क slen;
	ध्रुव वर्ण *p = fdt_get_string(fdt, stroffset, &slen);

	प्रत्यागम p && (slen == len) && (memcmp(p, s, len) == 0);
}

पूर्णाङ्क fdt_find_max_phandle(ध्रुव शून्य *fdt, uint32_t *phandle)
{
	uint32_t max = 0;
	पूर्णाङ्क विस्थापनम् = -1;

	यावत् (true) {
		uint32_t मानम्;

		विस्थापनम् = fdt_अग्र_ग्रन्थिः(fdt, विस्थापनम्, रिक्त);
		यदि (विस्थापनम् < 0) {
			यदि (विस्थापनम् == -FDT_ERR_NOTFOUND)
				भङ्ग;

			प्रत्यागम विस्थापनम्;
		}

		मानम् = fdt_get_phandle(fdt, विस्थापनम्);

		यदि (मानम् > max)
			max = मानम्;
	}

	यदि (phandle)
		*phandle = max;

	प्रत्यागम 0;
}

पूर्णाङ्क fdt_generate_phandle(ध्रुव शून्य *fdt, uint32_t *phandle)
{
	uint32_t max;
	पूर्णाङ्क err;

	err = fdt_find_max_phandle(fdt, &max);
	यदि (err < 0)
		प्रत्यागम err;

	यदि (max == FDT_MAX_PHANDLE)
		प्रत्यागम -FDT_ERR_NOPHANDLES;

	यदि (phandle)
		*phandle = max + 1;

	प्रत्यागम 0;
}

स्थिर ध्रुव संरचना fdt_reserve_प्रविष्टिः *fdt_mem_rsv(ध्रुव शून्य *fdt, पूर्णाङ्क n)
{
	अचिह्नित पूर्णाङ्क विस्थापनम् = n * परिमाण(संरचना fdt_reserve_प्रविष्टिः);
	अचिह्नित पूर्णाङ्क absoffset = fdt_off_mem_rsvmap(fdt) + विस्थापनम्;

	यदि (!can_assume(VALID_निवेशः)) {
		यदि (absoffset < fdt_off_mem_rsvmap(fdt))
			प्रत्यागम रिक्त;
		यदि (absoffset > fdt_totalsize(fdt) -
		    परिमाण(संरचना fdt_reserve_प्रविष्टिः))
			प्रत्यागम रिक्त;
	}
	प्रत्यागम fdt_mem_rsv_(fdt, n);
}

पूर्णाङ्क fdt_get_mem_rsv(ध्रुव शून्य *fdt, पूर्णाङ्क n, uint64_t *स्थानाङ्कः, uint64_t *आकार)
{
	ध्रुव संरचना fdt_reserve_प्रविष्टिः *re;

	FDT_RO_PROBE(fdt);
	re = fdt_mem_rsv(fdt, n);
	यदि (!can_assume(VALID_निवेशः) && !re)
		प्रत्यागम -FDT_ERR_BADOFFSET;

	*स्थानाङ्कः = fdt64_ld_(&re->स्थानाङ्कः);
	*आकार = fdt64_ld_(&re->आकार);
	प्रत्यागम 0;
}

पूर्णाङ्क fdt_अङ्कः_mem_rsv(ध्रुव शून्य *fdt)
{
	पूर्णाङ्क i;
	ध्रुव संरचना fdt_reserve_प्रविष्टिः *re;

	प्रति (i = 0; (re = fdt_mem_rsv(fdt, i)) != रिक्त; i++) {
		यदि (fdt64_ld_(&re->आकार) == 0)
			प्रत्यागम i;
	}
	प्रत्यागम -FDT_ERR_TRUNCATED;
}

स्थिर पूर्णाङ्क nextprop_(ध्रुव शून्य *fdt, पूर्णाङ्क विस्थापनम्)
{
	uint32_t चिह्नकम्;
	पूर्णाङ्क nextoffset;

	कुरु {
		चिह्नकम् = fdt_अग्र_चिह्नकम्(fdt, विस्थापनम्, &nextoffset);

		विकल्प (चिह्नकम्) {
		पक्ष FDT_अन्तः:
			यदि (nextoffset >= 0)
				प्रत्यागम -FDT_ERR_BADSTRUCTURE;
			अन्यथा
				प्रत्यागम nextoffset;

		पक्ष FDT_PROP:
			प्रत्यागम विस्थापनम्;
		}
		विस्थापनम् = nextoffset;
	} यावत् (चिह्नकम् == FDT_NOP);

	प्रत्यागम -FDT_ERR_NOTFOUND;
}

पूर्णाङ्क fdt_subnode_विस्थापनम्_namelen(ध्रुव शून्य *fdt, पूर्णाङ्क विस्थापनम्,
			       ध्रुव वर्ण *name, पूर्णाङ्क namelen)
{
	पूर्णाङ्क गभीरता;

	FDT_RO_PROBE(fdt);

	प्रति (गभीरता = 0;
	     (विस्थापनम् >= 0) && (गभीरता >= 0);
	     विस्थापनम् = fdt_अग्र_ग्रन्थिः(fdt, विस्थापनम्, &गभीरता))
		यदि ((गभीरता == 1)
		    && fdt_nodename_eq_(fdt, विस्थापनम्, name, namelen))
			प्रत्यागम विस्थापनम्;

	यदि (गभीरता < 0)
		प्रत्यागम -FDT_ERR_NOTFOUND;
	प्रत्यागम विस्थापनम्; /* error */
}

पूर्णाङ्क fdt_subnode_विस्थापनम्(ध्रुव शून्य *fdt, पूर्णाङ्क parentoffset,
		       ध्रुव वर्ण *name)
{
	प्रत्यागम fdt_subnode_विस्थापनम्_namelen(fdt, parentoffset, name, वाक्यदैर्घ्यम्(name));
}

पूर्णाङ्क fdt_मार्गः_विस्थापनम्_namelen(ध्रुव शून्य *fdt, ध्रुव वर्ण *मार्गः, पूर्णाङ्क namelen)
{
	ध्रुव वर्ण *अन्तः_2 = मार्गः + namelen;
	ध्रुव वर्ण *p = मार्गः;
	पूर्णाङ्क विस्थापनम् = 0;

	FDT_RO_PROBE(fdt);

	/* see if we have an alias */
	यदि (*मार्गः != '/') {
		ध्रुव वर्ण *q = memchr(मार्गः, '/', अन्तः_2 - p);

		यदि (!q)
			q = अन्तः_2;

		p = fdt_get_alias_namelen(fdt, p, q - p);
		यदि (!p)
			प्रत्यागम -FDT_ERR_BADPATH;
		विस्थापनम् = fdt_मार्गः_विस्थापनम्(fdt, p);

		p = q;
	}

	यावत् (p < अन्तः_2) {
		ध्रुव वर्ण *q;

		यावत् (*p == '/') {
			p++;
			यदि (p == अन्तः_2)
				प्रत्यागम विस्थापनम्;
		}
		q = memchr(p, '/', अन्तः_2 - p);
		यदि (! q)
			q = अन्तः_2;

		विस्थापनम् = fdt_subnode_विस्थापनम्_namelen(fdt, विस्थापनम्, p, q-p);
		यदि (विस्थापनम् < 0)
			प्रत्यागम विस्थापनम्;

		p = q;
	}

	प्रत्यागम विस्थापनम्;
}

पूर्णाङ्क fdt_मार्गः_विस्थापनम्(ध्रुव शून्य *fdt, ध्रुव वर्ण *मार्गः)
{
	प्रत्यागम fdt_मार्गः_विस्थापनम्_namelen(fdt, मार्गः, वाक्यदैर्घ्यम्(मार्गः));
}

ध्रुव वर्ण *fdt_get_name(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoffset, पूर्णाङ्क *len)
{
	ध्रुव संरचना fdt_ग्रन्थिः_शीर्षकम् *nh = fdt_विस्थापनम्_ptr_2(fdt, nodeoffset);
	ध्रुव वर्ण *nameptr;
	पूर्णाङ्क err;

	यदि (((err = fdt_ro_probe_(fdt)) < 0)
	    || ((err = fdt_check_ग्रन्थिः_विस्थापनम्(fdt, nodeoffset)) < 0))
			गच्छ fail;

	nameptr = nh->name;

	यदि (!can_assume(LATEST) && fdt_संस्करणम्(fdt) < 0x10) {
		/*
		 * For old FDT versions, match the naming conventions of V16:
		 * give only the leaf name (after all /). The actual tree
		 * contents are loosely checked.
		 */
		ध्रुव वर्ण *पर्णम्;
		पर्णम् = strrchr(nameptr, '/');
		यदि (पर्णम् == रिक्त) {
			err = -FDT_ERR_BADSTRUCTURE;
			गच्छ fail;
		}
		nameptr = पर्णम्+1;
	}

	यदि (len)
		*len = वाक्यदैर्घ्यम्(nameptr);

	प्रत्यागम nameptr;

 fail:
	यदि (len)
		*len = err;
	प्रत्यागम रिक्त;
}

पूर्णाङ्क fdt_first_property_विस्थापनम्(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoffset)
{
	पूर्णाङ्क विस्थापनम्;

	यदि ((विस्थापनम् = fdt_check_ग्रन्थिः_विस्थापनम्(fdt, nodeoffset)) < 0)
		प्रत्यागम विस्थापनम्;

	प्रत्यागम nextprop_(fdt, विस्थापनम्);
}

पूर्णाङ्क fdt_अग्र_property_विस्थापनम्(ध्रुव शून्य *fdt, पूर्णाङ्क विस्थापनम्)
{
	यदि ((विस्थापनम् = fdt_check_prop_विस्थापनम्(fdt, विस्थापनम्)) < 0)
		प्रत्यागम विस्थापनम्;

	प्रत्यागम nextprop_(fdt, विस्थापनम्);
}

स्थिर ध्रुव संरचना fdt_property *fdt_get_property_by_विस्थापनम्(ध्रुव शून्य *fdt,
						              पूर्णाङ्क विस्थापनम्,
						              पूर्णाङ्क *lenp)
{
	पूर्णाङ्क err;
	ध्रुव संरचना fdt_property *prop;

	यदि (!can_assume(VALID_निवेशः) &&
	    (err = fdt_check_prop_विस्थापनम्(fdt, विस्थापनम्)) < 0) {
		यदि (lenp)
			*lenp = err;
		प्रत्यागम रिक्त;
	}

	prop = fdt_विस्थापनम्_ptr_2(fdt, विस्थापनम्);

	यदि (lenp)
		*lenp = fdt32_ld_(&prop->len);

	प्रत्यागम prop;
}

ध्रुव संरचना fdt_property *fdt_get_property_by_विस्थापनम्_2(ध्रुव शून्य *fdt,
						      पूर्णाङ्क विस्थापनम्,
						      पूर्णाङ्क *lenp)
{
	/* Prior to version 16, properties may need realignment
	 * and this API does not work. fdt_getprop_*() will, however. */

	यदि (!can_assume(LATEST) && fdt_संस्करणम्(fdt) < 0x10) {
		यदि (lenp)
			*lenp = -FDT_ERR_BADVERSION;
		प्रत्यागम रिक्त;
	}

	प्रत्यागम fdt_get_property_by_विस्थापनम्(fdt, विस्थापनम्, lenp);
}

स्थिर ध्रुव संरचना fdt_property *fdt_get_property_namelen_(ध्रुव शून्य *fdt,
						            पूर्णाङ्क विस्थापनम्,
						            ध्रुव वर्ण *name,
						            पूर्णाङ्क namelen,
							    पूर्णाङ्क *lenp,
							    पूर्णाङ्क *poffset)
{
	प्रति (विस्थापनम् = fdt_first_property_विस्थापनम्(fdt, विस्थापनम्);
	     (विस्थापनम् >= 0);
	     (विस्थापनम् = fdt_अग्र_property_विस्थापनम्(fdt, विस्थापनम्))) {
		ध्रुव संरचना fdt_property *prop;

		prop = fdt_get_property_by_विस्थापनम्(fdt, विस्थापनम्, lenp);
		यदि (!can_assume(LIBFDT_FLAWLESS) && !prop) {
			विस्थापनम् = -FDT_ERR_आन्तरिकम्;
			भङ्ग;
		}
		यदि (fdt_string_eq_(fdt, fdt32_ld_(&prop->nameoff),
				   name, namelen)) {
			यदि (poffset)
				*poffset = विस्थापनम्;
			प्रत्यागम prop;
		}
	}

	यदि (lenp)
		*lenp = विस्थापनम्;
	प्रत्यागम रिक्त;
}


ध्रुव संरचना fdt_property *fdt_get_property_namelen(ध्रुव शून्य *fdt,
						    पूर्णाङ्क विस्थापनम्,
						    ध्रुव वर्ण *name,
						    पूर्णाङ्क namelen, पूर्णाङ्क *lenp)
{
	/* Prior to version 16, properties may need realignment
	 * and this API does not work. fdt_getprop_*() will, however. */
	यदि (!can_assume(LATEST) && fdt_संस्करणम्(fdt) < 0x10) {
		यदि (lenp)
			*lenp = -FDT_ERR_BADVERSION;
		प्रत्यागम रिक्त;
	}

	प्रत्यागम fdt_get_property_namelen_(fdt, विस्थापनम्, name, namelen, lenp,
					 रिक्त);
}


ध्रुव संरचना fdt_property *fdt_get_property(ध्रुव शून्य *fdt,
					    पूर्णाङ्क nodeoffset,
					    ध्रुव वर्ण *name, पूर्णाङ्क *lenp)
{
	प्रत्यागम fdt_get_property_namelen(fdt, nodeoffset, name,
					वाक्यदैर्घ्यम्(name), lenp);
}

ध्रुव शून्य *fdt_getprop_namelen(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoffset,
				ध्रुव वर्ण *name, पूर्णाङ्क namelen, पूर्णाङ्क *lenp)
{
	पूर्णाङ्क poffset;
	ध्रुव संरचना fdt_property *prop;

	prop = fdt_get_property_namelen_(fdt, nodeoffset, name, namelen, lenp,
					 &poffset);
	यदि (!prop)
		प्रत्यागम रिक्त;

	/* Handle realignment */
	यदि (!can_assume(LATEST) && fdt_संस्करणम्(fdt) < 0x10 &&
	    (poffset + परिमाण(*prop)) % 8 && fdt32_ld_(&prop->len) >= 8)
		प्रत्यागम prop->दत्त + 4;
	प्रत्यागम prop->दत्त;
}

ध्रुव शून्य *fdt_getprop_by_विस्थापनम्(ध्रुव शून्य *fdt, पूर्णाङ्क विस्थापनम्,
				  ध्रुव वर्ण **namep, पूर्णाङ्क *lenp)
{
	ध्रुव संरचना fdt_property *prop;

	prop = fdt_get_property_by_विस्थापनम्(fdt, विस्थापनम्, lenp);
	यदि (!prop)
		प्रत्यागम रिक्त;
	यदि (namep) {
		ध्रुव वर्ण *name;
		पूर्णाङ्क namelen;

		यदि (!can_assume(VALID_निवेशः)) {
			name = fdt_get_string(fdt, fdt32_ld_(&prop->nameoff),
					      &namelen);
			यदि (!name) {
				यदि (lenp)
					*lenp = namelen;
				प्रत्यागम रिक्त;
			}
			*namep = name;
		} अन्यथा {
			*namep = fdt_string(fdt, fdt32_ld_(&prop->nameoff));
		}
	}

	/* Handle realignment */
	यदि (!can_assume(LATEST) && fdt_संस्करणम्(fdt) < 0x10 &&
	    (विस्थापनम् + परिमाण(*prop)) % 8 && fdt32_ld_(&prop->len) >= 8)
		प्रत्यागम prop->दत्त + 4;
	प्रत्यागम prop->दत्त;
}

ध्रुव शून्य *fdt_getprop(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoffset,
			ध्रुव वर्ण *name, पूर्णाङ्क *lenp)
{
	प्रत्यागम fdt_getprop_namelen(fdt, nodeoffset, name, वाक्यदैर्घ्यम्(name), lenp);
}

uint32_t fdt_get_phandle(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoffset)
{
	ध्रुव fdt32_t *php;
	पूर्णाङ्क len;

	/* FIXME: This is a bit sub-optimal, since we potentially scan
	 * over all the properties twice. */
	php = fdt_getprop(fdt, nodeoffset, "phandle", &len);
	यदि (!php || (len != परिमाण(*php))) {
		php = fdt_getprop(fdt, nodeoffset, "linux,phandle", &len);
		यदि (!php || (len != परिमाण(*php)))
			प्रत्यागम 0;
	}

	प्रत्यागम fdt32_ld_(php);
}

ध्रुव वर्ण *fdt_get_alias_namelen(ध्रुव शून्य *fdt,
				  ध्रुव वर्ण *name, पूर्णाङ्क namelen)
{
	पूर्णाङ्क aliasoffset;

	aliasoffset = fdt_मार्गः_विस्थापनम्(fdt, "/aliases");
	यदि (aliasoffset < 0)
		प्रत्यागम रिक्त;

	प्रत्यागम fdt_getprop_namelen(fdt, aliasoffset, name, namelen, रिक्त);
}

ध्रुव वर्ण *fdt_get_alias(ध्रुव शून्य *fdt, ध्रुव वर्ण *name)
{
	प्रत्यागम fdt_get_alias_namelen(fdt, name, वाक्यदैर्घ्यम्(name));
}

पूर्णाङ्क fdt_get_मार्गः(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoffset, वर्ण *buf, पूर्णाङ्क buflen)
{
	पूर्णाङ्क pdepth = 0, p = 0;
	पूर्णाङ्क विस्थापनम्, गभीरता, namelen;
	ध्रुव वर्ण *name;

	FDT_RO_PROBE(fdt);

	यदि (buflen < 2)
		प्रत्यागम -FDT_ERR_NOSPACE;

	प्रति (विस्थापनम् = 0, गभीरता = 0;
	     (विस्थापनम् >= 0) && (विस्थापनम् <= nodeoffset);
	     विस्थापनम् = fdt_अग्र_ग्रन्थिः(fdt, विस्थापनम्, &गभीरता)) {
		यावत् (pdepth > गभीरता) {
			कुरु {
				p--;
			} यावत् (buf[p-1] != '/');
			pdepth--;
		}

		यदि (pdepth >= गभीरता) {
			name = fdt_get_name(fdt, विस्थापनम्, &namelen);
			यदि (!name)
				प्रत्यागम namelen;
			यदि ((p + namelen + 1) <= buflen) {
				स्मृतिं_प्रतिलिख(buf + p, name, namelen);
				p += namelen;
				buf[p++] = '/';
				pdepth++;
			}
		}

		यदि (विस्थापनम् == nodeoffset) {
			यदि (pdepth < (गभीरता + 1))
				प्रत्यागम -FDT_ERR_NOSPACE;

			यदि (p > 1) /* special case so that root path is "/", not "" */
				p--;
			buf[p] = '\0';
			प्रत्यागम 0;
		}
	}

	यदि ((विस्थापनम् == -FDT_ERR_NOTFOUND) || (विस्थापनम् >= 0))
		प्रत्यागम -FDT_ERR_BADOFFSET;
	अन्यथा यदि (विस्थापनम् == -FDT_ERR_BADOFFSET)
		प्रत्यागम -FDT_ERR_BADSTRUCTURE;

	प्रत्यागम विस्थापनम्; /* error from fdt_next_node() */
}

पूर्णाङ्क fdt_supernode_atdepth_विस्थापनम्(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoffset,
				 पूर्णाङ्क supernodedepth, पूर्णाङ्क *nodedepth)
{
	पूर्णाङ्क विस्थापनम्, गभीरता;
	पूर्णाङ्क supernodeoffset = -FDT_ERR_आन्तरिकम्;

	FDT_RO_PROBE(fdt);

	यदि (supernodedepth < 0)
		प्रत्यागम -FDT_ERR_NOTFOUND;

	प्रति (विस्थापनम् = 0, गभीरता = 0;
	     (विस्थापनम् >= 0) && (विस्थापनम् <= nodeoffset);
	     विस्थापनम् = fdt_अग्र_ग्रन्थिः(fdt, विस्थापनम्, &गभीरता)) {
		यदि (गभीरता == supernodedepth)
			supernodeoffset = विस्थापनम्;

		यदि (विस्थापनम् == nodeoffset) {
			यदि (nodedepth)
				*nodedepth = गभीरता;

			यदि (supernodedepth > गभीरता)
				प्रत्यागम -FDT_ERR_NOTFOUND;
			अन्यथा
				प्रत्यागम supernodeoffset;
		}
	}

	यदि (!can_assume(VALID_निवेशः)) {
		यदि ((विस्थापनम् == -FDT_ERR_NOTFOUND) || (विस्थापनम् >= 0))
			प्रत्यागम -FDT_ERR_BADOFFSET;
		अन्यथा यदि (विस्थापनम् == -FDT_ERR_BADOFFSET)
			प्रत्यागम -FDT_ERR_BADSTRUCTURE;
	}

	प्रत्यागम विस्थापनम्; /* error from fdt_next_node() */
}

पूर्णाङ्क fdt_ग्रन्थिः_गभीरता(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoffset)
{
	पूर्णाङ्क nodedepth;
	पूर्णाङ्क err;

	err = fdt_supernode_atdepth_विस्थापनम्(fdt, nodeoffset, 0, &nodedepth);
	यदि (err)
		प्रत्यागम (can_assume(LIBFDT_FLAWLESS) || err < 0) ? err :
			-FDT_ERR_आन्तरिकम्;
	प्रत्यागम nodedepth;
}

पूर्णाङ्क fdt_parent_विस्थापनम्(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoffset)
{
	पूर्णाङ्क nodedepth = fdt_ग्रन्थिः_गभीरता(fdt, nodeoffset);

	यदि (nodedepth < 0)
		प्रत्यागम nodedepth;
	प्रत्यागम fdt_supernode_atdepth_विस्थापनम्(fdt, nodeoffset,
					    nodedepth - 1, रिक्त);
}

पूर्णाङ्क fdt_ग्रन्थिः_विस्थापनम्_by_prop_मानम्(ध्रुव शून्य *fdt, पूर्णाङ्क startoffset,
				  ध्रुव वर्ण *propname,
				  ध्रुव शून्य *propval, पूर्णाङ्क proplen)
{
	पूर्णाङ्क विस्थापनम्;
	ध्रुव शून्य *val;
	पूर्णाङ्क len;

	FDT_RO_PROBE(fdt);

	/* FIXME: The algorithm here is pretty horrible: we scan each
	 * property of a node in fdt_getprop(), then if that didn't
	 * find what we want, we scan over them again making our way
	 * to the next node.  Still it's the easiest to implement
	 * approach; performance can come later. */
	प्रति (विस्थापनम् = fdt_अग्र_ग्रन्थिः(fdt, startoffset, रिक्त);
	     विस्थापनम् >= 0;
	     विस्थापनम् = fdt_अग्र_ग्रन्थिः(fdt, विस्थापनम्, रिक्त)) {
		val = fdt_getprop(fdt, विस्थापनम्, propname, &len);
		यदि (val && (len == proplen)
		    && (memcmp(val, propval, len) == 0))
			प्रत्यागम विस्थापनम्;
	}

	प्रत्यागम विस्थापनम्; /* error from fdt_next_node() */
}

पूर्णाङ्क fdt_ग्रन्थिः_विस्थापनम्_by_phandle(ध्रुव शून्य *fdt, uint32_t phandle)
{
	पूर्णाङ्क विस्थापनम्;

	यदि ((phandle == 0) || (phandle == ~0U))
		प्रत्यागम -FDT_ERR_BADPHANDLE;

	FDT_RO_PROBE(fdt);

	/* FIXME: The algorithm here is pretty horrible: we
	 * potentially scan each property of a node in
	 * fdt_get_phandle(), then if that didn't find what
	 * we want, we scan over them again making our way to the next
	 * node.  Still it's the easiest to implement approach;
	 * performance can come later. */
	प्रति (विस्थापनम् = fdt_अग्र_ग्रन्थिः(fdt, -1, रिक्त);
	     विस्थापनम् >= 0;
	     विस्थापनम् = fdt_अग्र_ग्रन्थिः(fdt, विस्थापनम्, रिक्त)) {
		यदि (fdt_get_phandle(fdt, विस्थापनम्) == phandle)
			प्रत्यागम विस्थापनम्;
	}

	प्रत्यागम विस्थापनम्; /* error from fdt_next_node() */
}

पूर्णाङ्क fdt_stringlist_अन्तर्भवति(ध्रुव वर्ण *strlist, पूर्णाङ्क listlen, ध्रुव वर्ण *str)
{
	पूर्णाङ्क len = वाक्यदैर्घ्यम्(str);
	ध्रुव वर्ण *p;

	यावत् (listlen >= len) {
		यदि (memcmp(str, strlist, len+1) == 0)
			प्रत्यागम 1;
		p = memchr(strlist, '\0', listlen);
		यदि (!p)
			प्रत्यागम 0; /* malformed strlist.. */
		listlen -= (p-strlist) + 1;
		strlist = p + 1;
	}
	प्रत्यागम 0;
}

पूर्णाङ्क fdt_stringlist_गणनम्(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoffset, ध्रुव वर्ण *property)
{
	ध्रुव वर्ण *list, *अन्तः_2;
	पूर्णाङ्क दीर्घता, गणनम् = 0;

	list = fdt_getprop(fdt, nodeoffset, property, &दीर्घता);
	यदि (!list)
		प्रत्यागम दीर्घता;

	अन्तः_2 = list + दीर्घता;

	यावत् (list < अन्तः_2) {
		दीर्घता = strnlen(list, अन्तः_2 - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		यदि (list + दीर्घता > अन्तः_2)
			प्रत्यागम -FDT_ERR_BADVALUE;

		list += दीर्घता;
		गणनम्++;
	}

	प्रत्यागम गणनम्;
}

पूर्णाङ्क fdt_stringlist_अन्वेषणम्(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoffset, ध्रुव वर्ण *property,
			  ध्रुव वर्ण *string)
{
	पूर्णाङ्क दीर्घता, len, idx = 0;
	ध्रुव वर्ण *list, *अन्तः_2;

	list = fdt_getprop(fdt, nodeoffset, property, &दीर्घता);
	यदि (!list)
		प्रत्यागम दीर्घता;

	len = वाक्यदैर्घ्यम्(string) + 1;
	अन्तः_2 = list + दीर्घता;

	यावत् (list < अन्तः_2) {
		दीर्घता = strnlen(list, अन्तः_2 - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		यदि (list + दीर्घता > अन्तः_2)
			प्रत्यागम -FDT_ERR_BADVALUE;

		यदि (दीर्घता == len && memcmp(list, string, दीर्घता) == 0)
			प्रत्यागम idx;

		list += दीर्घता;
		idx++;
	}

	प्रत्यागम -FDT_ERR_NOTFOUND;
}

ध्रुव वर्ण *fdt_stringlist_get(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoffset,
			       ध्रुव वर्ण *property, पूर्णाङ्क idx,
			       पूर्णाङ्क *lenp)
{
	ध्रुव वर्ण *list, *अन्तः_2;
	पूर्णाङ्क दीर्घता;

	list = fdt_getprop(fdt, nodeoffset, property, &दीर्घता);
	यदि (!list) {
		यदि (lenp)
			*lenp = दीर्घता;

		प्रत्यागम रिक्त;
	}

	अन्तः_2 = list + दीर्घता;

	यावत् (list < अन्तः_2) {
		दीर्घता = strnlen(list, अन्तः_2 - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		यदि (list + दीर्घता > अन्तः_2) {
			यदि (lenp)
				*lenp = -FDT_ERR_BADVALUE;

			प्रत्यागम रिक्त;
		}

		यदि (idx == 0) {
			यदि (lenp)
				*lenp = दीर्घता - 1;

			प्रत्यागम list;
		}

		list += दीर्घता;
		idx--;
	}

	यदि (lenp)
		*lenp = -FDT_ERR_NOTFOUND;

	प्रत्यागम रिक्त;
}

पूर्णाङ्क fdt_ग्रन्थिः_check_compatible(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoffset,
			      ध्रुव वर्ण *compatible)
{
	ध्रुव शून्य *prop;
	पूर्णाङ्क len;

	prop = fdt_getprop(fdt, nodeoffset, "compatible", &len);
	यदि (!prop)
		प्रत्यागम len;

	प्रत्यागम !fdt_stringlist_अन्तर्भवति(prop, len, compatible);
}

पूर्णाङ्क fdt_ग्रन्थिः_विस्थापनम्_by_compatible(ध्रुव शून्य *fdt, पूर्णाङ्क startoffset,
				  ध्रुव वर्ण *compatible)
{
	पूर्णाङ्क विस्थापनम्, err;

	FDT_RO_PROBE(fdt);

	/* FIXME: The algorithm here is pretty horrible: we scan each
	 * property of a node in fdt_node_check_compatible(), then if
	 * that didn't find what we want, we scan over them again
	 * making our way to the next node.  Still it's the easiest to
	 * implement approach; performance can come later. */
	प्रति (विस्थापनम् = fdt_अग्र_ग्रन्थिः(fdt, startoffset, रिक्त);
	     विस्थापनम् >= 0;
	     विस्थापनम् = fdt_अग्र_ग्रन्थिः(fdt, विस्थापनम्, रिक्त)) {
		err = fdt_ग्रन्थिः_check_compatible(fdt, विस्थापनम्, compatible);
		यदि ((err < 0) && (err != -FDT_ERR_NOTFOUND))
			प्रत्यागम err;
		अन्यथा यदि (err == 0)
			प्रत्यागम विस्थापनम्;
	}

	प्रत्यागम विस्थापनम्; /* error from fdt_next_node() */
}
