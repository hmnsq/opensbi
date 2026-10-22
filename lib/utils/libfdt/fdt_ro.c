// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#अन्तर्भाव "libfdt_env.h"

#अन्तर्भाव <fdt.h>
#अन्तर्भाव <libfdt.h>

#अन्तर्भाव "libfdt_internal.h"

स्थिर पूर्णाङ्क fdt_nodename_eq_(ध्रुव शून्य *fdt, पूर्णाङ्क offset,
			    ध्रुव वर्ण *s, पूर्णाङ्क len)
{
	पूर्णाङ्क olen;
	ध्रुव वर्ण *p = fdt_get_name(fdt, offset, &olen);

	जइ (!p || olen < len)
		/* short match */
		प्रत्यागम 0;

	जइ (memcmp(p, s, len) != 0)
		प्रत्यागम 0;

	जइ (p[len] == '\0')
		प्रत्यागम 1;
	अण्णहा जइ (!memchr(s, '@', len) && (p[len] == '@'))
		प्रत्यागम 1;
	अण्णहा
		प्रत्यागम 0;
}

ध्रुव वर्ण *fdt_get_string(ध्रुव शून्य *fdt, पूर्णाङ्क stroffset, पूर्णाङ्क *lenp)
{
	int32_t totalsize;
	uint32_t absoffset;
	size_t len;
	पूर्णाङ्क err;
	ध्रुव वर्ण *s, *n;

	जइ (can_assume(VALID_निवेशः)) {
		s = (ध्रुव वर्ण *)fdt + fdt_off_dt_strings(fdt) + stroffset;

		जइ (lenp)
			*lenp = strlen(s);
		प्रत्यागम s;
	}
	totalsize = fdt_ro_probe_(fdt);
	err = totalsize;
	जइ (totalsize < 0)
		गच्छ fail;

	err = -FDT_ERR_BADOFFSET;
	absoffset = stroffset + fdt_off_dt_strings(fdt);
	जइ (absoffset >= (unsigned)totalsize)
		गच्छ fail;
	len = totalsize - absoffset;

	जइ (fdt_magic(fdt) == FDT_MAGIC) {
		जइ (stroffset < 0)
			गच्छ fail;
		जइ (can_assume(LATEST) || fdt_version(fdt) >= 17) {
			जइ ((unsigned)stroffset >= fdt_size_dt_strings(fdt))
				गच्छ fail;
			जइ ((fdt_size_dt_strings(fdt) - stroffset) < len)
				len = fdt_size_dt_strings(fdt) - stroffset;
		}
	} अण्णहा जइ (fdt_magic(fdt) == FDT_SW_MAGIC) {
		unsigned पूर्णाङ्क sw_stroffset = -stroffset;

		जइ ((stroffset >= 0) ||
		    (sw_stroffset > fdt_size_dt_strings(fdt)))
			गच्छ fail;
		जइ (sw_stroffset < len)
			len = sw_stroffset;
	} अण्णहा {
		err = -FDT_ERR_INTERNAL;
		गच्छ fail;
	}

	s = (ध्रुव वर्ण *)fdt + absoffset;
	n = memchr(s, '\0', len);
	जइ (!n) {
		/* missing terminating NULL */
		err = -FDT_ERR_TRUNCATED;
		गच्छ fail;
	}

	जइ (lenp)
		*lenp = n - s;
	प्रत्यागम s;

fail:
	जइ (lenp)
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
	पूर्णाङ्क offset = -1;

	जाव (true) {
		uint32_t मानम्;

		offset = fdt_next_ग्रन्थिः(fdt, offset, रिक्त);
		जइ (offset < 0) {
			जइ (offset == -FDT_ERR_NOTFOUND)
				भङ्ग;

			प्रत्यागम offset;
		}

		मानम् = fdt_get_phandle(fdt, offset);

		जइ (मानम् > max)
			max = मानम्;
	}

	जइ (phandle)
		*phandle = max;

	प्रत्यागम 0;
}

पूर्णाङ्क fdt_generate_phandle(ध्रुव शून्य *fdt, uint32_t *phandle)
{
	uint32_t max;
	पूर्णाङ्क err;

	err = fdt_find_max_phandle(fdt, &max);
	जइ (err < 0)
		प्रत्यागम err;

	जइ (max == FDT_MAX_PHANDLE)
		प्रत्यागम -FDT_ERR_NOPHANDLES;

	जइ (phandle)
		*phandle = max + 1;

	प्रत्यागम 0;
}

स्थिर ध्रुव संरचना fdt_reserve_entry *fdt_mem_rsv(ध्रुव शून्य *fdt, पूर्णाङ्क n)
{
	unsigned पूर्णाङ्क offset = n * परिमाण(संरचना fdt_reserve_entry);
	unsigned पूर्णाङ्क absoffset = fdt_off_mem_rsvmap(fdt) + offset;

	जइ (!can_assume(VALID_निवेशः)) {
		जइ (absoffset < fdt_off_mem_rsvmap(fdt))
			प्रत्यागम रिक्त;
		जइ (absoffset > fdt_totalsize(fdt) -
		    परिमाण(संरचना fdt_reserve_entry))
			प्रत्यागम रिक्त;
	}
	प्रत्यागम fdt_mem_rsv_(fdt, n);
}

पूर्णाङ्क fdt_get_mem_rsv(ध्रुव शून्य *fdt, पूर्णाङ्क n, uint64_t *address, uint64_t *size)
{
	ध्रुव संरचना fdt_reserve_entry *re;

	FDT_RO_PROBE(fdt);
	re = fdt_mem_rsv(fdt, n);
	जइ (!can_assume(VALID_निवेशः) && !re)
		प्रत्यागम -FDT_ERR_BADOFFSET;

	*address = fdt64_ld_(&re->address);
	*size = fdt64_ld_(&re->size);
	प्रत्यागम 0;
}

पूर्णाङ्क fdt_अङ्कः_mem_rsv(ध्रुव शून्य *fdt)
{
	पूर्णाङ्क i;
	ध्रुव संरचना fdt_reserve_entry *re;

	प्रति (i = 0; (re = fdt_mem_rsv(fdt, i)) != रिक्त; i++) {
		जइ (fdt64_ld_(&re->size) == 0)
			प्रत्यागम i;
	}
	प्रत्यागम -FDT_ERR_TRUNCATED;
}

स्थिर पूर्णाङ्क nextprop_(ध्रुव शून्य *fdt, पूर्णाङ्क offset)
{
	uint32_t tag;
	पूर्णाङ्क nextoffset;

	करेहि {
		tag = fdt_next_tag(fdt, offset, &nextoffset);

		विकल्प (tag) {
		पक्ष FDT_END:
			जइ (nextoffset >= 0)
				प्रत्यागम -FDT_ERR_BADSTRUCTURE;
			अण्णहा
				प्रत्यागम nextoffset;

		पक्ष FDT_PROP:
			प्रत्यागम offset;
		}
		offset = nextoffset;
	} जाव (tag == FDT_NOP);

	प्रत्यागम -FDT_ERR_NOTFOUND;
}

पूर्णाङ्क fdt_subnode_offset_namelen(ध्रुव शून्य *fdt, पूर्णाङ्क offset,
			       ध्रुव वर्ण *name, पूर्णाङ्क namelen)
{
	पूर्णाङ्क depth;

	FDT_RO_PROBE(fdt);

	प्रति (depth = 0;
	     (offset >= 0) && (depth >= 0);
	     offset = fdt_next_ग्रन्थिः(fdt, offset, &depth))
		जइ ((depth == 1)
		    && fdt_nodename_eq_(fdt, offset, name, namelen))
			प्रत्यागम offset;

	जइ (depth < 0)
		प्रत्यागम -FDT_ERR_NOTFOUND;
	प्रत्यागम offset; /* error */
}

पूर्णाङ्क fdt_subnode_offset(ध्रुव शून्य *fdt, पूर्णाङ्क parentoffset,
		       ध्रुव वर्ण *name)
{
	प्रत्यागम fdt_subnode_offset_namelen(fdt, parentoffset, name, strlen(name));
}

पूर्णाङ्क fdt_path_offset_namelen(ध्रुव शून्य *fdt, ध्रुव वर्ण *path, पूर्णाङ्क namelen)
{
	ध्रुव वर्ण *end = path + namelen;
	ध्रुव वर्ण *p = path;
	पूर्णाङ्क offset = 0;

	FDT_RO_PROBE(fdt);

	/* see if we have an alias */
	जइ (*path != '/') {
		ध्रुव वर्ण *q = memchr(path, '/', end - p);

		जइ (!q)
			q = end;

		p = fdt_get_alias_namelen(fdt, p, q - p);
		जइ (!p)
			प्रत्यागम -FDT_ERR_BADPATH;
		offset = fdt_path_offset(fdt, p);

		p = q;
	}

	जाव (p < end) {
		ध्रुव वर्ण *q;

		जाव (*p == '/') {
			p++;
			जइ (p == end)
				प्रत्यागम offset;
		}
		q = memchr(p, '/', end - p);
		जइ (! q)
			q = end;

		offset = fdt_subnode_offset_namelen(fdt, offset, p, q-p);
		जइ (offset < 0)
			प्रत्यागम offset;

		p = q;
	}

	प्रत्यागम offset;
}

पूर्णाङ्क fdt_path_offset(ध्रुव शून्य *fdt, ध्रुव वर्ण *path)
{
	प्रत्यागम fdt_path_offset_namelen(fdt, path, strlen(path));
}

ध्रुव वर्ण *fdt_get_name(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoffset, पूर्णाङ्क *len)
{
	ध्रुव संरचना fdt_ग्रन्थिः_header *nh = fdt_offset_ptr_(fdt, nodeoffset);
	ध्रुव वर्ण *nameptr;
	पूर्णाङ्क err;

	जइ (((err = fdt_ro_probe_(fdt)) < 0)
	    || ((err = fdt_check_ग्रन्थिः_offset(fdt, nodeoffset)) < 0))
			गच्छ fail;

	nameptr = nh->name;

	जइ (!can_assume(LATEST) && fdt_version(fdt) < 0x10) {
		/*
		 * For old FDT versions, match the naming conventions of V16:
		 * give only the leaf name (after all /). The actual tree
		 * contents are loosely checked.
		 */
		ध्रुव वर्ण *leaf;
		leaf = strrchr(nameptr, '/');
		जइ (leaf == रिक्त) {
			err = -FDT_ERR_BADSTRUCTURE;
			गच्छ fail;
		}
		nameptr = leaf+1;
	}

	जइ (len)
		*len = strlen(nameptr);

	प्रत्यागम nameptr;

 fail:
	जइ (len)
		*len = err;
	प्रत्यागम रिक्त;
}

पूर्णाङ्क fdt_first_property_offset(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoffset)
{
	पूर्णाङ्क offset;

	जइ ((offset = fdt_check_ग्रन्थिः_offset(fdt, nodeoffset)) < 0)
		प्रत्यागम offset;

	प्रत्यागम nextprop_(fdt, offset);
}

पूर्णाङ्क fdt_next_property_offset(ध्रुव शून्य *fdt, पूर्णाङ्क offset)
{
	जइ ((offset = fdt_check_prop_offset_(fdt, offset)) < 0)
		प्रत्यागम offset;

	प्रत्यागम nextprop_(fdt, offset);
}

स्थिर ध्रुव संरचना fdt_property *fdt_get_property_by_offset_(ध्रुव शून्य *fdt,
						              पूर्णाङ्क offset,
						              पूर्णाङ्क *lenp)
{
	पूर्णाङ्क err;
	ध्रुव संरचना fdt_property *prop;

	जइ (!can_assume(VALID_निवेशः) &&
	    (err = fdt_check_prop_offset_(fdt, offset)) < 0) {
		जइ (lenp)
			*lenp = err;
		प्रत्यागम रिक्त;
	}

	prop = fdt_offset_ptr_(fdt, offset);

	जइ (lenp)
		*lenp = fdt32_ld_(&prop->len);

	प्रत्यागम prop;
}

ध्रुव संरचना fdt_property *fdt_get_property_by_offset(ध्रुव शून्य *fdt,
						      पूर्णाङ्क offset,
						      पूर्णाङ्क *lenp)
{
	/* Prior to version 16, properties may need realignment
	 * and this API does not work. fdt_getprop_*() will, however. */

	जइ (!can_assume(LATEST) && fdt_version(fdt) < 0x10) {
		जइ (lenp)
			*lenp = -FDT_ERR_BADVERSION;
		प्रत्यागम रिक्त;
	}

	प्रत्यागम fdt_get_property_by_offset_(fdt, offset, lenp);
}

स्थिर ध्रुव संरचना fdt_property *fdt_get_property_namelen_(ध्रुव शून्य *fdt,
						            पूर्णाङ्क offset,
						            ध्रुव वर्ण *name,
						            पूर्णाङ्क namelen,
							    पूर्णाङ्क *lenp,
							    पूर्णाङ्क *poffset)
{
	प्रति (offset = fdt_first_property_offset(fdt, offset);
	     (offset >= 0);
	     (offset = fdt_next_property_offset(fdt, offset))) {
		ध्रुव संरचना fdt_property *prop;

		prop = fdt_get_property_by_offset_(fdt, offset, lenp);
		जइ (!can_assume(LIBFDT_FLAWLESS) && !prop) {
			offset = -FDT_ERR_INTERNAL;
			भङ्ग;
		}
		जइ (fdt_string_eq_(fdt, fdt32_ld_(&prop->nameoff),
				   name, namelen)) {
			जइ (poffset)
				*poffset = offset;
			प्रत्यागम prop;
		}
	}

	जइ (lenp)
		*lenp = offset;
	प्रत्यागम रिक्त;
}


ध्रुव संरचना fdt_property *fdt_get_property_namelen(ध्रुव शून्य *fdt,
						    पूर्णाङ्क offset,
						    ध्रुव वर्ण *name,
						    पूर्णाङ्क namelen, पूर्णाङ्क *lenp)
{
	/* Prior to version 16, properties may need realignment
	 * and this API does not work. fdt_getprop_*() will, however. */
	जइ (!can_assume(LATEST) && fdt_version(fdt) < 0x10) {
		जइ (lenp)
			*lenp = -FDT_ERR_BADVERSION;
		प्रत्यागम रिक्त;
	}

	प्रत्यागम fdt_get_property_namelen_(fdt, offset, name, namelen, lenp,
					 रिक्त);
}


ध्रुव संरचना fdt_property *fdt_get_property(ध्रुव शून्य *fdt,
					    पूर्णाङ्क nodeoffset,
					    ध्रुव वर्ण *name, पूर्णाङ्क *lenp)
{
	प्रत्यागम fdt_get_property_namelen(fdt, nodeoffset, name,
					strlen(name), lenp);
}

ध्रुव शून्य *fdt_getprop_namelen(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoffset,
				ध्रुव वर्ण *name, पूर्णाङ्क namelen, पूर्णाङ्क *lenp)
{
	पूर्णाङ्क poffset;
	ध्रुव संरचना fdt_property *prop;

	prop = fdt_get_property_namelen_(fdt, nodeoffset, name, namelen, lenp,
					 &poffset);
	जइ (!prop)
		प्रत्यागम रिक्त;

	/* Handle realignment */
	जइ (!can_assume(LATEST) && fdt_version(fdt) < 0x10 &&
	    (poffset + परिमाण(*prop)) % 8 && fdt32_ld_(&prop->len) >= 8)
		प्रत्यागम prop->data + 4;
	प्रत्यागम prop->data;
}

ध्रुव शून्य *fdt_getprop_by_offset(ध्रुव शून्य *fdt, पूर्णाङ्क offset,
				  ध्रुव वर्ण **namep, पूर्णाङ्क *lenp)
{
	ध्रुव संरचना fdt_property *prop;

	prop = fdt_get_property_by_offset_(fdt, offset, lenp);
	जइ (!prop)
		प्रत्यागम रिक्त;
	जइ (namep) {
		ध्रुव वर्ण *name;
		पूर्णाङ्क namelen;

		जइ (!can_assume(VALID_निवेशः)) {
			name = fdt_get_string(fdt, fdt32_ld_(&prop->nameoff),
					      &namelen);
			जइ (!name) {
				जइ (lenp)
					*lenp = namelen;
				प्रत्यागम रिक्त;
			}
			*namep = name;
		} अण्णहा {
			*namep = fdt_string(fdt, fdt32_ld_(&prop->nameoff));
		}
	}

	/* Handle realignment */
	जइ (!can_assume(LATEST) && fdt_version(fdt) < 0x10 &&
	    (offset + परिमाण(*prop)) % 8 && fdt32_ld_(&prop->len) >= 8)
		प्रत्यागम prop->data + 4;
	प्रत्यागम prop->data;
}

ध्रुव शून्य *fdt_getprop(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoffset,
			ध्रुव वर्ण *name, पूर्णाङ्क *lenp)
{
	प्रत्यागम fdt_getprop_namelen(fdt, nodeoffset, name, strlen(name), lenp);
}

uint32_t fdt_get_phandle(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoffset)
{
	ध्रुव fdt32_t *php;
	पूर्णाङ्क len;

	/* FIXME: This is a bit sub-optimal, since we potentially scan
	 * over all the properties twice. */
	php = fdt_getprop(fdt, nodeoffset, "phandle", &len);
	जइ (!php || (len != परिमाण(*php))) {
		php = fdt_getprop(fdt, nodeoffset, "linux,phandle", &len);
		जइ (!php || (len != परिमाण(*php)))
			प्रत्यागम 0;
	}

	प्रत्यागम fdt32_ld_(php);
}

ध्रुव वर्ण *fdt_get_alias_namelen(ध्रुव शून्य *fdt,
				  ध्रुव वर्ण *name, पूर्णाङ्क namelen)
{
	पूर्णाङ्क aliasoffset;

	aliasoffset = fdt_path_offset(fdt, "/aliases");
	जइ (aliasoffset < 0)
		प्रत्यागम रिक्त;

	प्रत्यागम fdt_getprop_namelen(fdt, aliasoffset, name, namelen, रिक्त);
}

ध्रुव वर्ण *fdt_get_alias(ध्रुव शून्य *fdt, ध्रुव वर्ण *name)
{
	प्रत्यागम fdt_get_alias_namelen(fdt, name, strlen(name));
}

पूर्णाङ्क fdt_get_path(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoffset, वर्ण *buf, पूर्णाङ्क buflen)
{
	पूर्णाङ्क pdepth = 0, p = 0;
	पूर्णाङ्क offset, depth, namelen;
	ध्रुव वर्ण *name;

	FDT_RO_PROBE(fdt);

	जइ (buflen < 2)
		प्रत्यागम -FDT_ERR_NOSPACE;

	प्रति (offset = 0, depth = 0;
	     (offset >= 0) && (offset <= nodeoffset);
	     offset = fdt_next_ग्रन्थिः(fdt, offset, &depth)) {
		जाव (pdepth > depth) {
			करेहि {
				p--;
			} जाव (buf[p-1] != '/');
			pdepth--;
		}

		जइ (pdepth >= depth) {
			name = fdt_get_name(fdt, offset, &namelen);
			जइ (!name)
				प्रत्यागम namelen;
			जइ ((p + namelen + 1) <= buflen) {
				memcpy(buf + p, name, namelen);
				p += namelen;
				buf[p++] = '/';
				pdepth++;
			}
		}

		जइ (offset == nodeoffset) {
			जइ (pdepth < (depth + 1))
				प्रत्यागम -FDT_ERR_NOSPACE;

			जइ (p > 1) /* special case so that root path is "/", not "" */
				p--;
			buf[p] = '\0';
			प्रत्यागम 0;
		}
	}

	जइ ((offset == -FDT_ERR_NOTFOUND) || (offset >= 0))
		प्रत्यागम -FDT_ERR_BADOFFSET;
	अण्णहा जइ (offset == -FDT_ERR_BADOFFSET)
		प्रत्यागम -FDT_ERR_BADSTRUCTURE;

	प्रत्यागम offset; /* error from fdt_next_node() */
}

पूर्णाङ्क fdt_supernode_atdepth_offset(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoffset,
				 पूर्णाङ्क supernodedepth, पूर्णाङ्क *nodedepth)
{
	पूर्णाङ्क offset, depth;
	पूर्णाङ्क supernodeoffset = -FDT_ERR_INTERNAL;

	FDT_RO_PROBE(fdt);

	जइ (supernodedepth < 0)
		प्रत्यागम -FDT_ERR_NOTFOUND;

	प्रति (offset = 0, depth = 0;
	     (offset >= 0) && (offset <= nodeoffset);
	     offset = fdt_next_ग्रन्थिः(fdt, offset, &depth)) {
		जइ (depth == supernodedepth)
			supernodeoffset = offset;

		जइ (offset == nodeoffset) {
			जइ (nodedepth)
				*nodedepth = depth;

			जइ (supernodedepth > depth)
				प्रत्यागम -FDT_ERR_NOTFOUND;
			अण्णहा
				प्रत्यागम supernodeoffset;
		}
	}

	जइ (!can_assume(VALID_निवेशः)) {
		जइ ((offset == -FDT_ERR_NOTFOUND) || (offset >= 0))
			प्रत्यागम -FDT_ERR_BADOFFSET;
		अण्णहा जइ (offset == -FDT_ERR_BADOFFSET)
			प्रत्यागम -FDT_ERR_BADSTRUCTURE;
	}

	प्रत्यागम offset; /* error from fdt_next_node() */
}

पूर्णाङ्क fdt_ग्रन्थिः_depth(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoffset)
{
	पूर्णाङ्क nodedepth;
	पूर्णाङ्क err;

	err = fdt_supernode_atdepth_offset(fdt, nodeoffset, 0, &nodedepth);
	जइ (err)
		प्रत्यागम (can_assume(LIBFDT_FLAWLESS) || err < 0) ? err :
			-FDT_ERR_INTERNAL;
	प्रत्यागम nodedepth;
}

पूर्णाङ्क fdt_parent_offset(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoffset)
{
	पूर्णाङ्क nodedepth = fdt_ग्रन्थिः_depth(fdt, nodeoffset);

	जइ (nodedepth < 0)
		प्रत्यागम nodedepth;
	प्रत्यागम fdt_supernode_atdepth_offset(fdt, nodeoffset,
					    nodedepth - 1, रिक्त);
}

पूर्णाङ्क fdt_ग्रन्थिः_offset_by_prop_मानम्(ध्रुव शून्य *fdt, पूर्णाङ्क startoffset,
				  ध्रुव वर्ण *propname,
				  ध्रुव शून्य *propval, पूर्णाङ्क proplen)
{
	पूर्णाङ्क offset;
	ध्रुव शून्य *val;
	पूर्णाङ्क len;

	FDT_RO_PROBE(fdt);

	/* FIXME: The algorithm here is pretty horrible: we scan each
	 * property of a node in fdt_getprop(), then if that didn't
	 * find what we want, we scan over them again making our way
	 * to the next node.  Still it's the easiest to implement
	 * approach; performance can come later. */
	प्रति (offset = fdt_next_ग्रन्थिः(fdt, startoffset, रिक्त);
	     offset >= 0;
	     offset = fdt_next_ग्रन्थिः(fdt, offset, रिक्त)) {
		val = fdt_getprop(fdt, offset, propname, &len);
		जइ (val && (len == proplen)
		    && (memcmp(val, propval, len) == 0))
			प्रत्यागम offset;
	}

	प्रत्यागम offset; /* error from fdt_next_node() */
}

पूर्णाङ्क fdt_ग्रन्थिः_offset_by_phandle(ध्रुव शून्य *fdt, uint32_t phandle)
{
	पूर्णाङ्क offset;

	जइ ((phandle == 0) || (phandle == ~0U))
		प्रत्यागम -FDT_ERR_BADPHANDLE;

	FDT_RO_PROBE(fdt);

	/* FIXME: The algorithm here is pretty horrible: we
	 * potentially scan each property of a node in
	 * fdt_get_phandle(), then if that didn't find what
	 * we want, we scan over them again making our way to the next
	 * node.  Still it's the easiest to implement approach;
	 * performance can come later. */
	प्रति (offset = fdt_next_ग्रन्थिः(fdt, -1, रिक्त);
	     offset >= 0;
	     offset = fdt_next_ग्रन्थिः(fdt, offset, रिक्त)) {
		जइ (fdt_get_phandle(fdt, offset) == phandle)
			प्रत्यागम offset;
	}

	प्रत्यागम offset; /* error from fdt_next_node() */
}

पूर्णाङ्क fdt_stringlist_contains(ध्रुव वर्ण *strlist, पूर्णाङ्क listlen, ध्रुव वर्ण *str)
{
	पूर्णाङ्क len = strlen(str);
	ध्रुव वर्ण *p;

	जाव (listlen >= len) {
		जइ (memcmp(str, strlist, len+1) == 0)
			प्रत्यागम 1;
		p = memchr(strlist, '\0', listlen);
		जइ (!p)
			प्रत्यागम 0; /* malformed strlist.. */
		listlen -= (p-strlist) + 1;
		strlist = p + 1;
	}
	प्रत्यागम 0;
}

पूर्णाङ्क fdt_stringlist_गणनम्(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoffset, ध्रुव वर्ण *property)
{
	ध्रुव वर्ण *list, *end;
	पूर्णाङ्क length, गणनम् = 0;

	list = fdt_getprop(fdt, nodeoffset, property, &length);
	जइ (!list)
		प्रत्यागम length;

	end = list + length;

	जाव (list < end) {
		length = strnlen(list, end - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		जइ (list + length > end)
			प्रत्यागम -FDT_ERR_BADVALUE;

		list += length;
		गणनम्++;
	}

	प्रत्यागम गणनम्;
}

पूर्णाङ्क fdt_stringlist_अन्वेषणम्(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoffset, ध्रुव वर्ण *property,
			  ध्रुव वर्ण *string)
{
	पूर्णाङ्क length, len, idx = 0;
	ध्रुव वर्ण *list, *end;

	list = fdt_getprop(fdt, nodeoffset, property, &length);
	जइ (!list)
		प्रत्यागम length;

	len = strlen(string) + 1;
	end = list + length;

	जाव (list < end) {
		length = strnlen(list, end - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		जइ (list + length > end)
			प्रत्यागम -FDT_ERR_BADVALUE;

		जइ (length == len && memcmp(list, string, length) == 0)
			प्रत्यागम idx;

		list += length;
		idx++;
	}

	प्रत्यागम -FDT_ERR_NOTFOUND;
}

ध्रुव वर्ण *fdt_stringlist_get(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoffset,
			       ध्रुव वर्ण *property, पूर्णाङ्क idx,
			       पूर्णाङ्क *lenp)
{
	ध्रुव वर्ण *list, *end;
	पूर्णाङ्क length;

	list = fdt_getprop(fdt, nodeoffset, property, &length);
	जइ (!list) {
		जइ (lenp)
			*lenp = length;

		प्रत्यागम रिक्त;
	}

	end = list + length;

	जाव (list < end) {
		length = strnlen(list, end - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		जइ (list + length > end) {
			जइ (lenp)
				*lenp = -FDT_ERR_BADVALUE;

			प्रत्यागम रिक्त;
		}

		जइ (idx == 0) {
			जइ (lenp)
				*lenp = length - 1;

			प्रत्यागम list;
		}

		list += length;
		idx--;
	}

	जइ (lenp)
		*lenp = -FDT_ERR_NOTFOUND;

	प्रत्यागम रिक्त;
}

पूर्णाङ्क fdt_ग्रन्थिः_check_compatible(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoffset,
			      ध्रुव वर्ण *compatible)
{
	ध्रुव शून्य *prop;
	पूर्णाङ्क len;

	prop = fdt_getprop(fdt, nodeoffset, "compatible", &len);
	जइ (!prop)
		प्रत्यागम len;

	प्रत्यागम !fdt_stringlist_contains(prop, len, compatible);
}

पूर्णाङ्क fdt_ग्रन्थिः_offset_by_compatible(ध्रुव शून्य *fdt, पूर्णाङ्क startoffset,
				  ध्रुव वर्ण *compatible)
{
	पूर्णाङ्क offset, err;

	FDT_RO_PROBE(fdt);

	/* FIXME: The algorithm here is pretty horrible: we scan each
	 * property of a node in fdt_node_check_compatible(), then if
	 * that didn't find what we want, we scan over them again
	 * making our way to the next node.  Still it's the easiest to
	 * implement approach; performance can come later. */
	प्रति (offset = fdt_next_ग्रन्थिः(fdt, startoffset, रिक्त);
	     offset >= 0;
	     offset = fdt_next_ग्रन्थिः(fdt, offset, रिक्त)) {
		err = fdt_ग्रन्थिः_check_compatible(fdt, offset, compatible);
		जइ ((err < 0) && (err != -FDT_ERR_NOTFOUND))
			प्रत्यागम err;
		अण्णहा जइ (err == 0)
			प्रत्यागम offset;
	}

	प्रत्यागम offset; /* error from fdt_next_node() */
}
