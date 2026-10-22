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

	सचे (!p || olen < len)
		/* short match */
		पटिनिवत्त 0;

	सचे (memcmp(p, s, len) != 0)
		पटिनिवत्त 0;

	सचे (p[len] == '\0')
		पटिनिवत्त 1;
	अञ्ञथा सचे (!memchr(s, '@', len) && (p[len] == '@'))
		पटिनिवत्त 1;
	अञ्ञथा
		पटिनिवत्त 0;
}

ध्रुव वर्ण *fdt_get_string(ध्रुव शून्य *fdt, पूर्णाङ्क stroffset, पूर्णाङ्क *lenp)
{
	int32_t totalsize;
	uint32_t absoffset;
	size_t len;
	पूर्णाङ्क err;
	ध्रुव वर्ण *s, *n;

	सचे (can_assume(VALID_निवेशः)) {
		s = (ध्रुव वर्ण *)fdt + fdt_off_dt_strings(fdt) + stroffset;

		सचे (lenp)
			*lenp = strlen(s);
		पटिनिवत्त s;
	}
	totalsize = fdt_ro_probe_(fdt);
	err = totalsize;
	सचे (totalsize < 0)
		गच्छ fail;

	err = -FDT_ERR_BADOFFSET;
	absoffset = stroffset + fdt_off_dt_strings(fdt);
	सचे (absoffset >= (unsigned)totalsize)
		गच्छ fail;
	len = totalsize - absoffset;

	सचे (fdt_magic(fdt) == FDT_MAGIC) {
		सचे (stroffset < 0)
			गच्छ fail;
		सचे (can_assume(LATEST) || fdt_version(fdt) >= 17) {
			सचे ((unsigned)stroffset >= fdt_size_dt_strings(fdt))
				गच्छ fail;
			सचे ((fdt_size_dt_strings(fdt) - stroffset) < len)
				len = fdt_size_dt_strings(fdt) - stroffset;
		}
	} अञ्ञथा सचे (fdt_magic(fdt) == FDT_SW_MAGIC) {
		unsigned पूर्णाङ्क sw_stroffset = -stroffset;

		सचे ((stroffset >= 0) ||
		    (sw_stroffset > fdt_size_dt_strings(fdt)))
			गच्छ fail;
		सचे (sw_stroffset < len)
			len = sw_stroffset;
	} अञ्ञथा {
		err = -FDT_ERR_INTERNAL;
		गच्छ fail;
	}

	s = (ध्रुव वर्ण *)fdt + absoffset;
	n = memchr(s, '\0', len);
	सचे (!n) {
		/* missing terminating NULL */
		err = -FDT_ERR_TRUNCATED;
		गच्छ fail;
	}

	सचे (lenp)
		*lenp = n - s;
	पटिनिवत्त s;

fail:
	सचे (lenp)
		*lenp = err;
	पटिनिवत्त रिक्त;
}

ध्रुव वर्ण *fdt_string(ध्रुव शून्य *fdt, पूर्णाङ्क stroffset)
{
	पटिनिवत्त fdt_get_string(fdt, stroffset, रिक्त);
}

स्थिर पूर्णाङ्क fdt_string_eq_(ध्रुव शून्य *fdt, पूर्णाङ्क stroffset,
			  ध्रुव वर्ण *s, पूर्णाङ्क len)
{
	पूर्णाङ्क slen;
	ध्रुव वर्ण *p = fdt_get_string(fdt, stroffset, &slen);

	पटिनिवत्त p && (slen == len) && (memcmp(p, s, len) == 0);
}

पूर्णाङ्क fdt_find_max_phandle(ध्रुव शून्य *fdt, uint32_t *phandle)
{
	uint32_t max = 0;
	पूर्णाङ्क offset = -1;

	याव (true) {
		uint32_t मानम्;

		offset = fdt_next_ग्रन्थिः(fdt, offset, रिक्त);
		सचे (offset < 0) {
			सचे (offset == -FDT_ERR_NOTFOUND)
				भङ्ग;

			पटिनिवत्त offset;
		}

		मानम् = fdt_get_phandle(fdt, offset);

		सचे (मानम् > max)
			max = मानम्;
	}

	सचे (phandle)
		*phandle = max;

	पटिनिवत्त 0;
}

पूर्णाङ्क fdt_generate_phandle(ध्रुव शून्य *fdt, uint32_t *phandle)
{
	uint32_t max;
	पूर्णाङ्क err;

	err = fdt_find_max_phandle(fdt, &max);
	सचे (err < 0)
		पटिनिवत्त err;

	सचे (max == FDT_MAX_PHANDLE)
		पटिनिवत्त -FDT_ERR_NOPHANDLES;

	सचे (phandle)
		*phandle = max + 1;

	पटिनिवत्त 0;
}

स्थिर ध्रुव संरचना fdt_reserve_entry *fdt_mem_rsv(ध्रुव शून्य *fdt, पूर्णाङ्क n)
{
	unsigned पूर्णाङ्क offset = n * परिमाण(संरचना fdt_reserve_entry);
	unsigned पूर्णाङ्क absoffset = fdt_off_mem_rsvmap(fdt) + offset;

	सचे (!can_assume(VALID_निवेशः)) {
		सचे (absoffset < fdt_off_mem_rsvmap(fdt))
			पटिनिवत्त रिक्त;
		सचे (absoffset > fdt_totalsize(fdt) -
		    परिमाण(संरचना fdt_reserve_entry))
			पटिनिवत्त रिक्त;
	}
	पटिनिवत्त fdt_mem_rsv_(fdt, n);
}

पूर्णाङ्क fdt_get_mem_rsv(ध्रुव शून्य *fdt, पूर्णाङ्क n, uint64_t *address, uint64_t *size)
{
	ध्रुव संरचना fdt_reserve_entry *re;

	FDT_RO_PROBE(fdt);
	re = fdt_mem_rsv(fdt, n);
	सचे (!can_assume(VALID_निवेशः) && !re)
		पटिनिवत्त -FDT_ERR_BADOFFSET;

	*address = fdt64_ld_(&re->address);
	*size = fdt64_ld_(&re->size);
	पटिनिवत्त 0;
}

पूर्णाङ्क fdt_अङ्कः_mem_rsv(ध्रुव शून्य *fdt)
{
	पूर्णाङ्क i;
	ध्रुव संरचना fdt_reserve_entry *re;

	अत्थाय (i = 0; (re = fdt_mem_rsv(fdt, i)) != रिक्त; i++) {
		सचे (fdt64_ld_(&re->size) == 0)
			पटिनिवत्त i;
	}
	पटिनिवत्त -FDT_ERR_TRUNCATED;
}

स्थिर पूर्णाङ्क nextprop_(ध्रुव शून्य *fdt, पूर्णाङ्क offset)
{
	uint32_t tag;
	पूर्णाङ्क nextoffset;

	करोहि {
		tag = fdt_next_tag(fdt, offset, &nextoffset);

		विकल्प (tag) {
		पक्ष FDT_END:
			सचे (nextoffset >= 0)
				पटिनिवत्त -FDT_ERR_BADSTRUCTURE;
			अञ्ञथा
				पटिनिवत्त nextoffset;

		पक्ष FDT_PROP:
			पटिनिवत्त offset;
		}
		offset = nextoffset;
	} याव (tag == FDT_NOP);

	पटिनिवत्त -FDT_ERR_NOTFOUND;
}

पूर्णाङ्क fdt_subnode_offset_namelen(ध्रुव शून्य *fdt, पूर्णाङ्क offset,
			       ध्रुव वर्ण *name, पूर्णाङ्क namelen)
{
	पूर्णाङ्क depth;

	FDT_RO_PROBE(fdt);

	अत्थाय (depth = 0;
	     (offset >= 0) && (depth >= 0);
	     offset = fdt_next_ग्रन्थिः(fdt, offset, &depth))
		सचे ((depth == 1)
		    && fdt_nodename_eq_(fdt, offset, name, namelen))
			पटिनिवत्त offset;

	सचे (depth < 0)
		पटिनिवत्त -FDT_ERR_NOTFOUND;
	पटिनिवत्त offset; /* error */
}

पूर्णाङ्क fdt_subnode_offset(ध्रुव शून्य *fdt, पूर्णाङ्क parentoffset,
		       ध्रुव वर्ण *name)
{
	पटिनिवत्त fdt_subnode_offset_namelen(fdt, parentoffset, name, strlen(name));
}

पूर्णाङ्क fdt_path_offset_namelen(ध्रुव शून्य *fdt, ध्रुव वर्ण *path, पूर्णाङ्क namelen)
{
	ध्रुव वर्ण *end = path + namelen;
	ध्रुव वर्ण *p = path;
	पूर्णाङ्क offset = 0;

	FDT_RO_PROBE(fdt);

	/* see if we have an alias */
	सचे (*path != '/') {
		ध्रुव वर्ण *q = memchr(path, '/', end - p);

		सचे (!q)
			q = end;

		p = fdt_get_alias_namelen(fdt, p, q - p);
		सचे (!p)
			पटिनिवत्त -FDT_ERR_BADPATH;
		offset = fdt_path_offset(fdt, p);

		p = q;
	}

	याव (p < end) {
		ध्रुव वर्ण *q;

		याव (*p == '/') {
			p++;
			सचे (p == end)
				पटिनिवत्त offset;
		}
		q = memchr(p, '/', end - p);
		सचे (! q)
			q = end;

		offset = fdt_subnode_offset_namelen(fdt, offset, p, q-p);
		सचे (offset < 0)
			पटिनिवत्त offset;

		p = q;
	}

	पटिनिवत्त offset;
}

पूर्णाङ्क fdt_path_offset(ध्रुव शून्य *fdt, ध्रुव वर्ण *path)
{
	पटिनिवत्त fdt_path_offset_namelen(fdt, path, strlen(path));
}

ध्रुव वर्ण *fdt_get_name(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoffset, पूर्णाङ्क *len)
{
	ध्रुव संरचना fdt_ग्रन्थिः_header *nh = fdt_offset_ptr_(fdt, nodeoffset);
	ध्रुव वर्ण *nameptr;
	पूर्णाङ्क err;

	सचे (((err = fdt_ro_probe_(fdt)) < 0)
	    || ((err = fdt_check_ग्रन्थिः_offset(fdt, nodeoffset)) < 0))
			गच्छ fail;

	nameptr = nh->name;

	सचे (!can_assume(LATEST) && fdt_version(fdt) < 0x10) {
		/*
		 * For old FDT versions, match the naming conventions of V16:
		 * give only the leaf name (after all /). The actual tree
		 * contents are loosely checked.
		 */
		ध्रुव वर्ण *leaf;
		leaf = strrchr(nameptr, '/');
		सचे (leaf == रिक्त) {
			err = -FDT_ERR_BADSTRUCTURE;
			गच्छ fail;
		}
		nameptr = leaf+1;
	}

	सचे (len)
		*len = strlen(nameptr);

	पटिनिवत्त nameptr;

 fail:
	सचे (len)
		*len = err;
	पटिनिवत्त रिक्त;
}

पूर्णाङ्क fdt_first_property_offset(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoffset)
{
	पूर्णाङ्क offset;

	सचे ((offset = fdt_check_ग्रन्थिः_offset(fdt, nodeoffset)) < 0)
		पटिनिवत्त offset;

	पटिनिवत्त nextprop_(fdt, offset);
}

पूर्णाङ्क fdt_next_property_offset(ध्रुव शून्य *fdt, पूर्णाङ्क offset)
{
	सचे ((offset = fdt_check_prop_offset_(fdt, offset)) < 0)
		पटिनिवत्त offset;

	पटिनिवत्त nextprop_(fdt, offset);
}

स्थिर ध्रुव संरचना fdt_property *fdt_get_property_by_offset_(ध्रुव शून्य *fdt,
						              पूर्णाङ्क offset,
						              पूर्णाङ्क *lenp)
{
	पूर्णाङ्क err;
	ध्रुव संरचना fdt_property *prop;

	सचे (!can_assume(VALID_निवेशः) &&
	    (err = fdt_check_prop_offset_(fdt, offset)) < 0) {
		सचे (lenp)
			*lenp = err;
		पटिनिवत्त रिक्त;
	}

	prop = fdt_offset_ptr_(fdt, offset);

	सचे (lenp)
		*lenp = fdt32_ld_(&prop->len);

	पटिनिवत्त prop;
}

ध्रुव संरचना fdt_property *fdt_get_property_by_offset(ध्रुव शून्य *fdt,
						      पूर्णाङ्क offset,
						      पूर्णाङ्क *lenp)
{
	/* Prior to version 16, properties may need realignment
	 * and this API does not work. fdt_getprop_*() will, however. */

	सचे (!can_assume(LATEST) && fdt_version(fdt) < 0x10) {
		सचे (lenp)
			*lenp = -FDT_ERR_BADVERSION;
		पटिनिवत्त रिक्त;
	}

	पटिनिवत्त fdt_get_property_by_offset_(fdt, offset, lenp);
}

स्थिर ध्रुव संरचना fdt_property *fdt_get_property_namelen_(ध्रुव शून्य *fdt,
						            पूर्णाङ्क offset,
						            ध्रुव वर्ण *name,
						            पूर्णाङ्क namelen,
							    पूर्णाङ्क *lenp,
							    पूर्णाङ्क *poffset)
{
	अत्थाय (offset = fdt_first_property_offset(fdt, offset);
	     (offset >= 0);
	     (offset = fdt_next_property_offset(fdt, offset))) {
		ध्रुव संरचना fdt_property *prop;

		prop = fdt_get_property_by_offset_(fdt, offset, lenp);
		सचे (!can_assume(LIBFDT_FLAWLESS) && !prop) {
			offset = -FDT_ERR_INTERNAL;
			भङ्ग;
		}
		सचे (fdt_string_eq_(fdt, fdt32_ld_(&prop->nameoff),
				   name, namelen)) {
			सचे (poffset)
				*poffset = offset;
			पटिनिवत्त prop;
		}
	}

	सचे (lenp)
		*lenp = offset;
	पटिनिवत्त रिक्त;
}


ध्रुव संरचना fdt_property *fdt_get_property_namelen(ध्रुव शून्य *fdt,
						    पूर्णाङ्क offset,
						    ध्रुव वर्ण *name,
						    पूर्णाङ्क namelen, पूर्णाङ्क *lenp)
{
	/* Prior to version 16, properties may need realignment
	 * and this API does not work. fdt_getprop_*() will, however. */
	सचे (!can_assume(LATEST) && fdt_version(fdt) < 0x10) {
		सचे (lenp)
			*lenp = -FDT_ERR_BADVERSION;
		पटिनिवत्त रिक्त;
	}

	पटिनिवत्त fdt_get_property_namelen_(fdt, offset, name, namelen, lenp,
					 रिक्त);
}


ध्रुव संरचना fdt_property *fdt_get_property(ध्रुव शून्य *fdt,
					    पूर्णाङ्क nodeoffset,
					    ध्रुव वर्ण *name, पूर्णाङ्क *lenp)
{
	पटिनिवत्त fdt_get_property_namelen(fdt, nodeoffset, name,
					strlen(name), lenp);
}

ध्रुव शून्य *fdt_getprop_namelen(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoffset,
				ध्रुव वर्ण *name, पूर्णाङ्क namelen, पूर्णाङ्क *lenp)
{
	पूर्णाङ्क poffset;
	ध्रुव संरचना fdt_property *prop;

	prop = fdt_get_property_namelen_(fdt, nodeoffset, name, namelen, lenp,
					 &poffset);
	सचे (!prop)
		पटिनिवत्त रिक्त;

	/* Handle realignment */
	सचे (!can_assume(LATEST) && fdt_version(fdt) < 0x10 &&
	    (poffset + परिमाण(*prop)) % 8 && fdt32_ld_(&prop->len) >= 8)
		पटिनिवत्त prop->data + 4;
	पटिनिवत्त prop->data;
}

ध्रुव शून्य *fdt_getprop_by_offset(ध्रुव शून्य *fdt, पूर्णाङ्क offset,
				  ध्रुव वर्ण **namep, पूर्णाङ्क *lenp)
{
	ध्रुव संरचना fdt_property *prop;

	prop = fdt_get_property_by_offset_(fdt, offset, lenp);
	सचे (!prop)
		पटिनिवत्त रिक्त;
	सचे (namep) {
		ध्रुव वर्ण *name;
		पूर्णाङ्क namelen;

		सचे (!can_assume(VALID_निवेशः)) {
			name = fdt_get_string(fdt, fdt32_ld_(&prop->nameoff),
					      &namelen);
			सचे (!name) {
				सचे (lenp)
					*lenp = namelen;
				पटिनिवत्त रिक्त;
			}
			*namep = name;
		} अञ्ञथा {
			*namep = fdt_string(fdt, fdt32_ld_(&prop->nameoff));
		}
	}

	/* Handle realignment */
	सचे (!can_assume(LATEST) && fdt_version(fdt) < 0x10 &&
	    (offset + परिमाण(*prop)) % 8 && fdt32_ld_(&prop->len) >= 8)
		पटिनिवत्त prop->data + 4;
	पटिनिवत्त prop->data;
}

ध्रुव शून्य *fdt_getprop(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoffset,
			ध्रुव वर्ण *name, पूर्णाङ्क *lenp)
{
	पटिनिवत्त fdt_getprop_namelen(fdt, nodeoffset, name, strlen(name), lenp);
}

uint32_t fdt_get_phandle(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoffset)
{
	ध्रुव fdt32_t *php;
	पूर्णाङ्क len;

	/* FIXME: This is a bit sub-optimal, since we potentially scan
	 * over all the properties twice. */
	php = fdt_getprop(fdt, nodeoffset, "phandle", &len);
	सचे (!php || (len != परिमाण(*php))) {
		php = fdt_getprop(fdt, nodeoffset, "linux,phandle", &len);
		सचे (!php || (len != परिमाण(*php)))
			पटिनिवत्त 0;
	}

	पटिनिवत्त fdt32_ld_(php);
}

ध्रुव वर्ण *fdt_get_alias_namelen(ध्रुव शून्य *fdt,
				  ध्रुव वर्ण *name, पूर्णाङ्क namelen)
{
	पूर्णाङ्क aliasoffset;

	aliasoffset = fdt_path_offset(fdt, "/aliases");
	सचे (aliasoffset < 0)
		पटिनिवत्त रिक्त;

	पटिनिवत्त fdt_getprop_namelen(fdt, aliasoffset, name, namelen, रिक्त);
}

ध्रुव वर्ण *fdt_get_alias(ध्रुव शून्य *fdt, ध्रुव वर्ण *name)
{
	पटिनिवत्त fdt_get_alias_namelen(fdt, name, strlen(name));
}

पूर्णाङ्क fdt_get_path(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoffset, वर्ण *buf, पूर्णाङ्क buflen)
{
	पूर्णाङ्क pdepth = 0, p = 0;
	पूर्णाङ्क offset, depth, namelen;
	ध्रुव वर्ण *name;

	FDT_RO_PROBE(fdt);

	सचे (buflen < 2)
		पटिनिवत्त -FDT_ERR_NOSPACE;

	अत्थाय (offset = 0, depth = 0;
	     (offset >= 0) && (offset <= nodeoffset);
	     offset = fdt_next_ग्रन्थिः(fdt, offset, &depth)) {
		याव (pdepth > depth) {
			करोहि {
				p--;
			} याव (buf[p-1] != '/');
			pdepth--;
		}

		सचे (pdepth >= depth) {
			name = fdt_get_name(fdt, offset, &namelen);
			सचे (!name)
				पटिनिवत्त namelen;
			सचे ((p + namelen + 1) <= buflen) {
				memcpy(buf + p, name, namelen);
				p += namelen;
				buf[p++] = '/';
				pdepth++;
			}
		}

		सचे (offset == nodeoffset) {
			सचे (pdepth < (depth + 1))
				पटिनिवत्त -FDT_ERR_NOSPACE;

			सचे (p > 1) /* special case so that root path is "/", not "" */
				p--;
			buf[p] = '\0';
			पटिनिवत्त 0;
		}
	}

	सचे ((offset == -FDT_ERR_NOTFOUND) || (offset >= 0))
		पटिनिवत्त -FDT_ERR_BADOFFSET;
	अञ्ञथा सचे (offset == -FDT_ERR_BADOFFSET)
		पटिनिवत्त -FDT_ERR_BADSTRUCTURE;

	पटिनिवत्त offset; /* error from fdt_next_node() */
}

पूर्णाङ्क fdt_supernode_atdepth_offset(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoffset,
				 पूर्णाङ्क supernodedepth, पूर्णाङ्क *nodedepth)
{
	पूर्णाङ्क offset, depth;
	पूर्णाङ्क supernodeoffset = -FDT_ERR_INTERNAL;

	FDT_RO_PROBE(fdt);

	सचे (supernodedepth < 0)
		पटिनिवत्त -FDT_ERR_NOTFOUND;

	अत्थाय (offset = 0, depth = 0;
	     (offset >= 0) && (offset <= nodeoffset);
	     offset = fdt_next_ग्रन्थिः(fdt, offset, &depth)) {
		सचे (depth == supernodedepth)
			supernodeoffset = offset;

		सचे (offset == nodeoffset) {
			सचे (nodedepth)
				*nodedepth = depth;

			सचे (supernodedepth > depth)
				पटिनिवत्त -FDT_ERR_NOTFOUND;
			अञ्ञथा
				पटिनिवत्त supernodeoffset;
		}
	}

	सचे (!can_assume(VALID_निवेशः)) {
		सचे ((offset == -FDT_ERR_NOTFOUND) || (offset >= 0))
			पटिनिवत्त -FDT_ERR_BADOFFSET;
		अञ्ञथा सचे (offset == -FDT_ERR_BADOFFSET)
			पटिनिवत्त -FDT_ERR_BADSTRUCTURE;
	}

	पटिनिवत्त offset; /* error from fdt_next_node() */
}

पूर्णाङ्क fdt_ग्रन्थिः_depth(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoffset)
{
	पूर्णाङ्क nodedepth;
	पूर्णाङ्क err;

	err = fdt_supernode_atdepth_offset(fdt, nodeoffset, 0, &nodedepth);
	सचे (err)
		पटिनिवत्त (can_assume(LIBFDT_FLAWLESS) || err < 0) ? err :
			-FDT_ERR_INTERNAL;
	पटिनिवत्त nodedepth;
}

पूर्णाङ्क fdt_parent_offset(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoffset)
{
	पूर्णाङ्क nodedepth = fdt_ग्रन्थिः_depth(fdt, nodeoffset);

	सचे (nodedepth < 0)
		पटिनिवत्त nodedepth;
	पटिनिवत्त fdt_supernode_atdepth_offset(fdt, nodeoffset,
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
	अत्थाय (offset = fdt_next_ग्रन्थिः(fdt, startoffset, रिक्त);
	     offset >= 0;
	     offset = fdt_next_ग्रन्थिः(fdt, offset, रिक्त)) {
		val = fdt_getprop(fdt, offset, propname, &len);
		सचे (val && (len == proplen)
		    && (memcmp(val, propval, len) == 0))
			पटिनिवत्त offset;
	}

	पटिनिवत्त offset; /* error from fdt_next_node() */
}

पूर्णाङ्क fdt_ग्रन्थिः_offset_by_phandle(ध्रुव शून्य *fdt, uint32_t phandle)
{
	पूर्णाङ्क offset;

	सचे ((phandle == 0) || (phandle == ~0U))
		पटिनिवत्त -FDT_ERR_BADPHANDLE;

	FDT_RO_PROBE(fdt);

	/* FIXME: The algorithm here is pretty horrible: we
	 * potentially scan each property of a node in
	 * fdt_get_phandle(), then if that didn't find what
	 * we want, we scan over them again making our way to the next
	 * node.  Still it's the easiest to implement approach;
	 * performance can come later. */
	अत्थाय (offset = fdt_next_ग्रन्थिः(fdt, -1, रिक्त);
	     offset >= 0;
	     offset = fdt_next_ग्रन्थिः(fdt, offset, रिक्त)) {
		सचे (fdt_get_phandle(fdt, offset) == phandle)
			पटिनिवत्त offset;
	}

	पटिनिवत्त offset; /* error from fdt_next_node() */
}

पूर्णाङ्क fdt_stringlist_contains(ध्रुव वर्ण *strlist, पूर्णाङ्क listlen, ध्रुव वर्ण *str)
{
	पूर्णाङ्क len = strlen(str);
	ध्रुव वर्ण *p;

	याव (listlen >= len) {
		सचे (memcmp(str, strlist, len+1) == 0)
			पटिनिवत्त 1;
		p = memchr(strlist, '\0', listlen);
		सचे (!p)
			पटिनिवत्त 0; /* malformed strlist.. */
		listlen -= (p-strlist) + 1;
		strlist = p + 1;
	}
	पटिनिवत्त 0;
}

पूर्णाङ्क fdt_stringlist_गणनम्(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoffset, ध्रुव वर्ण *property)
{
	ध्रुव वर्ण *list, *end;
	पूर्णाङ्क length, गणनम् = 0;

	list = fdt_getprop(fdt, nodeoffset, property, &length);
	सचे (!list)
		पटिनिवत्त length;

	end = list + length;

	याव (list < end) {
		length = strnlen(list, end - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		सचे (list + length > end)
			पटिनिवत्त -FDT_ERR_BADVALUE;

		list += length;
		गणनम्++;
	}

	पटिनिवत्त गणनम्;
}

पूर्णाङ्क fdt_stringlist_अन्वेषणम्(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoffset, ध्रुव वर्ण *property,
			  ध्रुव वर्ण *string)
{
	पूर्णाङ्क length, len, idx = 0;
	ध्रुव वर्ण *list, *end;

	list = fdt_getprop(fdt, nodeoffset, property, &length);
	सचे (!list)
		पटिनिवत्त length;

	len = strlen(string) + 1;
	end = list + length;

	याव (list < end) {
		length = strnlen(list, end - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		सचे (list + length > end)
			पटिनिवत्त -FDT_ERR_BADVALUE;

		सचे (length == len && memcmp(list, string, length) == 0)
			पटिनिवत्त idx;

		list += length;
		idx++;
	}

	पटिनिवत्त -FDT_ERR_NOTFOUND;
}

ध्रुव वर्ण *fdt_stringlist_get(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoffset,
			       ध्रुव वर्ण *property, पूर्णाङ्क idx,
			       पूर्णाङ्क *lenp)
{
	ध्रुव वर्ण *list, *end;
	पूर्णाङ्क length;

	list = fdt_getprop(fdt, nodeoffset, property, &length);
	सचे (!list) {
		सचे (lenp)
			*lenp = length;

		पटिनिवत्त रिक्त;
	}

	end = list + length;

	याव (list < end) {
		length = strnlen(list, end - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		सचे (list + length > end) {
			सचे (lenp)
				*lenp = -FDT_ERR_BADVALUE;

			पटिनिवत्त रिक्त;
		}

		सचे (idx == 0) {
			सचे (lenp)
				*lenp = length - 1;

			पटिनिवत्त list;
		}

		list += length;
		idx--;
	}

	सचे (lenp)
		*lenp = -FDT_ERR_NOTFOUND;

	पटिनिवत्त रिक्त;
}

पूर्णाङ्क fdt_ग्रन्थिः_check_compatible(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoffset,
			      ध्रुव वर्ण *compatible)
{
	ध्रुव शून्य *prop;
	पूर्णाङ्क len;

	prop = fdt_getprop(fdt, nodeoffset, "compatible", &len);
	सचे (!prop)
		पटिनिवत्त len;

	पटिनिवत्त !fdt_stringlist_contains(prop, len, compatible);
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
	अत्थाय (offset = fdt_next_ग्रन्थिः(fdt, startoffset, रिक्त);
	     offset >= 0;
	     offset = fdt_next_ग्रन्थिः(fdt, offset, रिक्त)) {
		err = fdt_ग्रन्थिः_check_compatible(fdt, offset, compatible);
		सचे ((err < 0) && (err != -FDT_ERR_NOTFOUND))
			पटिनिवत्त err;
		अञ्ञथा सचे (err == 0)
			पटिनिवत्त offset;
	}

	पटिनिवत्त offset; /* error from fdt_next_node() */
}
