// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#अन्तर्भाव "libfdt_env.h"

#अन्तर्भाव <fdt.h>
#अन्तर्भाव <libfdt.h>

#अन्तर्भाव "libfdt_internal.h"

स्थिर पूर्णाङ्क fdt_nodename_eq_(अचर शून्य *fdt, पूर्णाङ्क offset,
			    अचर वर्ण *s, पूर्णाङ्क len)
{
	पूर्णाङ्क olen;
	अचर वर्ण *p = fdt_get_name(fdt, offset, &olen);

	यदि (!p || olen < len)
		/* short match */
		फर्काऊ 0;

	यदि (स_भेद(p, s, len) != 0)
		फर्काऊ 0;

	यदि (p[len] == '\0')
		फर्काऊ 1;
	अन्यथा यदि (!स_प्रथम(s, '@', len) && (p[len] == '@'))
		फर्काऊ 1;
	अन्यथा
		फर्काऊ 0;
}

अचर वर्ण *fdt_get_string(अचर शून्य *fdt, पूर्णाङ्क stroffset, पूर्णाङ्क *lenp)
{
	int32_t totalsize;
	uint32_t absoffset;
	माप_प्रकार len;
	पूर्णाङ्क err;
	अचर वर्ण *s, *n;

	यदि (can_assume(VALID_आगत)) {
		s = (अचर वर्ण *)fdt + fdt_off_dt_strings(fdt) + stroffset;

		यदि (lenp)
			*lenp = म_माप(s);
		फर्काऊ s;
	}
	totalsize = fdt_ro_probe_(fdt);
	err = totalsize;
	यदि (totalsize < 0)
		गच्छ fail;

	err = -FDT_ERR_BADOFFSET;
	absoffset = stroffset + fdt_off_dt_strings(fdt);
	यदि (absoffset >= (अचिन्हित)totalsize)
		गच्छ fail;
	len = totalsize - absoffset;

	यदि (fdt_magic(fdt) == FDT_MAGIC) {
		यदि (stroffset < 0)
			गच्छ fail;
		यदि (can_assume(LATEST) || fdt_version(fdt) >= 17) {
			यदि ((अचिन्हित)stroffset >= fdt_size_dt_strings(fdt))
				गच्छ fail;
			यदि ((fdt_size_dt_strings(fdt) - stroffset) < len)
				len = fdt_size_dt_strings(fdt) - stroffset;
		}
	} अन्यथा यदि (fdt_magic(fdt) == FDT_SW_MAGIC) {
		अचिन्हित पूर्णाङ्क sw_stroffset = -stroffset;

		यदि ((stroffset >= 0) ||
		    (sw_stroffset > fdt_size_dt_strings(fdt)))
			गच्छ fail;
		यदि (sw_stroffset < len)
			len = sw_stroffset;
	} अन्यथा {
		err = -FDT_ERR_INTERNAL;
		गच्छ fail;
	}

	s = (अचर वर्ण *)fdt + absoffset;
	n = स_प्रथम(s, '\0', len);
	यदि (!n) {
		/* missing terminating NULL */
		err = -FDT_ERR_TRUNCATED;
		गच्छ fail;
	}

	यदि (lenp)
		*lenp = n - s;
	फर्काऊ s;

fail:
	यदि (lenp)
		*lenp = err;
	फर्काऊ रिक्त;
}

अचर वर्ण *fdt_string(अचर शून्य *fdt, पूर्णाङ्क stroffset)
{
	फर्काऊ fdt_get_string(fdt, stroffset, रिक्त);
}

स्थिर पूर्णाङ्क fdt_string_eq_(अचर शून्य *fdt, पूर्णाङ्क stroffset,
			  अचर वर्ण *s, पूर्णाङ्क len)
{
	पूर्णाङ्क slen;
	अचर वर्ण *p = fdt_get_string(fdt, stroffset, &slen);

	फर्काऊ p && (slen == len) && (स_भेद(p, s, len) == 0);
}

पूर्णाङ्क fdt_find_max_phandle(अचर शून्य *fdt, uint32_t *phandle)
{
	uint32_t max = 0;
	पूर्णाङ्क offset = -1;

	जबसम्म (true) {
		uint32_t मान;

		offset = fdt_next_गाँठो(fdt, offset, रिक्त);
		यदि (offset < 0) {
			यदि (offset == -FDT_ERR_NOTFOUND)
				रोक;

			फर्काऊ offset;
		}

		मान = fdt_get_phandle(fdt, offset);

		यदि (मान > max)
			max = मान;
	}

	यदि (phandle)
		*phandle = max;

	फर्काऊ 0;
}

पूर्णाङ्क fdt_generate_phandle(अचर शून्य *fdt, uint32_t *phandle)
{
	uint32_t max;
	पूर्णाङ्क err;

	err = fdt_find_max_phandle(fdt, &max);
	यदि (err < 0)
		फर्काऊ err;

	यदि (max == FDT_MAX_PHANDLE)
		फर्काऊ -FDT_ERR_NOPHANDLES;

	यदि (phandle)
		*phandle = max + 1;

	फर्काऊ 0;
}

स्थिर अचर संरचना fdt_reserve_entry *fdt_mem_rsv(अचर शून्य *fdt, पूर्णाङ्क n)
{
	अचिन्हित पूर्णाङ्क offset = n * परिमाण(संरचना fdt_reserve_entry);
	अचिन्हित पूर्णाङ्क absoffset = fdt_off_mem_rsvmap(fdt) + offset;

	यदि (!can_assume(VALID_आगत)) {
		यदि (absoffset < fdt_off_mem_rsvmap(fdt))
			फर्काऊ रिक्त;
		यदि (absoffset > fdt_totalsize(fdt) -
		    परिमाण(संरचना fdt_reserve_entry))
			फर्काऊ रिक्त;
	}
	फर्काऊ fdt_mem_rsv_(fdt, n);
}

पूर्णाङ्क fdt_get_mem_rsv(अचर शून्य *fdt, पूर्णाङ्क n, uint64_t *address, uint64_t *size)
{
	अचर संरचना fdt_reserve_entry *re;

	FDT_RO_PROBE(fdt);
	re = fdt_mem_rsv(fdt, n);
	यदि (!can_assume(VALID_आगत) && !re)
		फर्काऊ -FDT_ERR_BADOFFSET;

	*address = fdt64_ld_(&re->address);
	*size = fdt64_ld_(&re->size);
	फर्काऊ 0;
}

पूर्णाङ्क fdt_अङ्क_mem_rsv(अचर शून्य *fdt)
{
	पूर्णाङ्क i;
	अचर संरचना fdt_reserve_entry *re;

	लागि (i = 0; (re = fdt_mem_rsv(fdt, i)) != रिक्त; i++) {
		यदि (fdt64_ld_(&re->size) == 0)
			फर्काऊ i;
	}
	फर्काऊ -FDT_ERR_TRUNCATED;
}

स्थिर पूर्णाङ्क nextprop_(अचर शून्य *fdt, पूर्णाङ्क offset)
{
	uint32_t tag;
	पूर्णाङ्क nextoffset;

	गर {
		tag = fdt_next_tag(fdt, offset, &nextoffset);

		विकल्प (tag) {
		पक्ष FDT_END:
			यदि (nextoffset >= 0)
				फर्काऊ -FDT_ERR_BADSTRUCTURE;
			अन्यथा
				फर्काऊ nextoffset;

		पक्ष FDT_PROP:
			फर्काऊ offset;
		}
		offset = nextoffset;
	} जबसम्म (tag == FDT_NOP);

	फर्काऊ -FDT_ERR_NOTFOUND;
}

पूर्णाङ्क fdt_subnode_offset_namelen(अचर शून्य *fdt, पूर्णाङ्क offset,
			       अचर वर्ण *name, पूर्णाङ्क namelen)
{
	पूर्णाङ्क depth;

	FDT_RO_PROBE(fdt);

	लागि (depth = 0;
	     (offset >= 0) && (depth >= 0);
	     offset = fdt_next_गाँठो(fdt, offset, &depth))
		यदि ((depth == 1)
		    && fdt_nodename_eq_(fdt, offset, name, namelen))
			फर्काऊ offset;

	यदि (depth < 0)
		फर्काऊ -FDT_ERR_NOTFOUND;
	फर्काऊ offset; /* error */
}

पूर्णाङ्क fdt_subnode_offset(अचर शून्य *fdt, पूर्णाङ्क parentoffset,
		       अचर वर्ण *name)
{
	फर्काऊ fdt_subnode_offset_namelen(fdt, parentoffset, name, म_माप(name));
}

पूर्णाङ्क fdt_path_offset_namelen(अचर शून्य *fdt, अचर वर्ण *path, पूर्णाङ्क namelen)
{
	अचर वर्ण *end = path + namelen;
	अचर वर्ण *p = path;
	पूर्णाङ्क offset = 0;

	FDT_RO_PROBE(fdt);

	/* see if we have an alias */
	यदि (*path != '/') {
		अचर वर्ण *q = स_प्रथम(path, '/', end - p);

		यदि (!q)
			q = end;

		p = fdt_get_alias_namelen(fdt, p, q - p);
		यदि (!p)
			फर्काऊ -FDT_ERR_BADPATH;
		offset = fdt_path_offset(fdt, p);

		p = q;
	}

	जबसम्म (p < end) {
		अचर वर्ण *q;

		जबसम्म (*p == '/') {
			p++;
			यदि (p == end)
				फर्काऊ offset;
		}
		q = स_प्रथम(p, '/', end - p);
		यदि (! q)
			q = end;

		offset = fdt_subnode_offset_namelen(fdt, offset, p, q-p);
		यदि (offset < 0)
			फर्काऊ offset;

		p = q;
	}

	फर्काऊ offset;
}

पूर्णाङ्क fdt_path_offset(अचर शून्य *fdt, अचर वर्ण *path)
{
	फर्काऊ fdt_path_offset_namelen(fdt, path, म_माप(path));
}

अचर वर्ण *fdt_get_name(अचर शून्य *fdt, पूर्णाङ्क nodeoffset, पूर्णाङ्क *len)
{
	अचर संरचना fdt_गाँठो_header *nh = fdt_offset_ptr_(fdt, nodeoffset);
	अचर वर्ण *nameptr;
	पूर्णाङ्क err;

	यदि (((err = fdt_ro_probe_(fdt)) < 0)
	    || ((err = fdt_check_गाँठो_offset(fdt, nodeoffset)) < 0))
			गच्छ fail;

	nameptr = nh->name;

	यदि (!can_assume(LATEST) && fdt_version(fdt) < 0x10) {
		/*
		 * For old FDT versions, match the naming conventions of V16:
		 * give only the leaf name (after all /). The actual tree
		 * contents are loosely checked.
		 */
		अचर वर्ण *leaf;
		leaf = म_खोजप(nameptr, '/');
		यदि (leaf == रिक्त) {
			err = -FDT_ERR_BADSTRUCTURE;
			गच्छ fail;
		}
		nameptr = leaf+1;
	}

	यदि (len)
		*len = म_माप(nameptr);

	फर्काऊ nameptr;

 fail:
	यदि (len)
		*len = err;
	फर्काऊ रिक्त;
}

पूर्णाङ्क fdt_first_property_offset(अचर शून्य *fdt, पूर्णाङ्क nodeoffset)
{
	पूर्णाङ्क offset;

	यदि ((offset = fdt_check_गाँठो_offset(fdt, nodeoffset)) < 0)
		फर्काऊ offset;

	फर्काऊ nextprop_(fdt, offset);
}

पूर्णाङ्क fdt_next_property_offset(अचर शून्य *fdt, पूर्णाङ्क offset)
{
	यदि ((offset = fdt_check_prop_offset_(fdt, offset)) < 0)
		फर्काऊ offset;

	फर्काऊ nextprop_(fdt, offset);
}

स्थिर अचर संरचना fdt_property *fdt_get_property_by_offset_(अचर शून्य *fdt,
						              पूर्णाङ्क offset,
						              पूर्णाङ्क *lenp)
{
	पूर्णाङ्क err;
	अचर संरचना fdt_property *prop;

	यदि (!can_assume(VALID_आगत) &&
	    (err = fdt_check_prop_offset_(fdt, offset)) < 0) {
		यदि (lenp)
			*lenp = err;
		फर्काऊ रिक्त;
	}

	prop = fdt_offset_ptr_(fdt, offset);

	यदि (lenp)
		*lenp = fdt32_ld_(&prop->len);

	फर्काऊ prop;
}

अचर संरचना fdt_property *fdt_get_property_by_offset(अचर शून्य *fdt,
						      पूर्णाङ्क offset,
						      पूर्णाङ्क *lenp)
{
	/* Prior to version 16, properties may need realignment
	 * and this API does not work. fdt_getprop_*() will, however. */

	यदि (!can_assume(LATEST) && fdt_version(fdt) < 0x10) {
		यदि (lenp)
			*lenp = -FDT_ERR_BADVERSION;
		फर्काऊ रिक्त;
	}

	फर्काऊ fdt_get_property_by_offset_(fdt, offset, lenp);
}

स्थिर अचर संरचना fdt_property *fdt_get_property_namelen_(अचर शून्य *fdt,
						            पूर्णाङ्क offset,
						            अचर वर्ण *name,
						            पूर्णाङ्क namelen,
							    पूर्णाङ्क *lenp,
							    पूर्णाङ्क *poffset)
{
	लागि (offset = fdt_first_property_offset(fdt, offset);
	     (offset >= 0);
	     (offset = fdt_next_property_offset(fdt, offset))) {
		अचर संरचना fdt_property *prop;

		prop = fdt_get_property_by_offset_(fdt, offset, lenp);
		यदि (!can_assume(LIBFDT_FLAWLESS) && !prop) {
			offset = -FDT_ERR_INTERNAL;
			रोक;
		}
		यदि (fdt_string_eq_(fdt, fdt32_ld_(&prop->nameoff),
				   name, namelen)) {
			यदि (poffset)
				*poffset = offset;
			फर्काऊ prop;
		}
	}

	यदि (lenp)
		*lenp = offset;
	फर्काऊ रिक्त;
}


अचर संरचना fdt_property *fdt_get_property_namelen(अचर शून्य *fdt,
						    पूर्णाङ्क offset,
						    अचर वर्ण *name,
						    पूर्णाङ्क namelen, पूर्णाङ्क *lenp)
{
	/* Prior to version 16, properties may need realignment
	 * and this API does not work. fdt_getprop_*() will, however. */
	यदि (!can_assume(LATEST) && fdt_version(fdt) < 0x10) {
		यदि (lenp)
			*lenp = -FDT_ERR_BADVERSION;
		फर्काऊ रिक्त;
	}

	फर्काऊ fdt_get_property_namelen_(fdt, offset, name, namelen, lenp,
					 रिक्त);
}


अचर संरचना fdt_property *fdt_get_property(अचर शून्य *fdt,
					    पूर्णाङ्क nodeoffset,
					    अचर वर्ण *name, पूर्णाङ्क *lenp)
{
	फर्काऊ fdt_get_property_namelen(fdt, nodeoffset, name,
					म_माप(name), lenp);
}

अचर शून्य *fdt_getprop_namelen(अचर शून्य *fdt, पूर्णाङ्क nodeoffset,
				अचर वर्ण *name, पूर्णाङ्क namelen, पूर्णाङ्क *lenp)
{
	पूर्णाङ्क poffset;
	अचर संरचना fdt_property *prop;

	prop = fdt_get_property_namelen_(fdt, nodeoffset, name, namelen, lenp,
					 &poffset);
	यदि (!prop)
		फर्काऊ रिक्त;

	/* Handle realignment */
	यदि (!can_assume(LATEST) && fdt_version(fdt) < 0x10 &&
	    (poffset + परिमाण(*prop)) % 8 && fdt32_ld_(&prop->len) >= 8)
		फर्काऊ prop->data + 4;
	फर्काऊ prop->data;
}

अचर शून्य *fdt_getprop_by_offset(अचर शून्य *fdt, पूर्णाङ्क offset,
				  अचर वर्ण **namep, पूर्णाङ्क *lenp)
{
	अचर संरचना fdt_property *prop;

	prop = fdt_get_property_by_offset_(fdt, offset, lenp);
	यदि (!prop)
		फर्काऊ रिक्त;
	यदि (namep) {
		अचर वर्ण *name;
		पूर्णाङ्क namelen;

		यदि (!can_assume(VALID_आगत)) {
			name = fdt_get_string(fdt, fdt32_ld_(&prop->nameoff),
					      &namelen);
			यदि (!name) {
				यदि (lenp)
					*lenp = namelen;
				फर्काऊ रिक्त;
			}
			*namep = name;
		} अन्यथा {
			*namep = fdt_string(fdt, fdt32_ld_(&prop->nameoff));
		}
	}

	/* Handle realignment */
	यदि (!can_assume(LATEST) && fdt_version(fdt) < 0x10 &&
	    (offset + परिमाण(*prop)) % 8 && fdt32_ld_(&prop->len) >= 8)
		फर्काऊ prop->data + 4;
	फर्काऊ prop->data;
}

अचर शून्य *fdt_getprop(अचर शून्य *fdt, पूर्णाङ्क nodeoffset,
			अचर वर्ण *name, पूर्णाङ्क *lenp)
{
	फर्काऊ fdt_getprop_namelen(fdt, nodeoffset, name, म_माप(name), lenp);
}

uint32_t fdt_get_phandle(अचर शून्य *fdt, पूर्णाङ्क nodeoffset)
{
	अचर fdt32_t *php;
	पूर्णाङ्क len;

	/* FIXME: This is a bit sub-optimal, since we potentially scan
	 * over all the properties twice. */
	php = fdt_getprop(fdt, nodeoffset, "phandle", &len);
	यदि (!php || (len != परिमाण(*php))) {
		php = fdt_getprop(fdt, nodeoffset, "linux,phandle", &len);
		यदि (!php || (len != परिमाण(*php)))
			फर्काऊ 0;
	}

	फर्काऊ fdt32_ld_(php);
}

अचर वर्ण *fdt_get_alias_namelen(अचर शून्य *fdt,
				  अचर वर्ण *name, पूर्णाङ्क namelen)
{
	पूर्णाङ्क aliasoffset;

	aliasoffset = fdt_path_offset(fdt, "/aliases");
	यदि (aliasoffset < 0)
		फर्काऊ रिक्त;

	फर्काऊ fdt_getprop_namelen(fdt, aliasoffset, name, namelen, रिक्त);
}

अचर वर्ण *fdt_get_alias(अचर शून्य *fdt, अचर वर्ण *name)
{
	फर्काऊ fdt_get_alias_namelen(fdt, name, म_माप(name));
}

पूर्णाङ्क fdt_get_path(अचर शून्य *fdt, पूर्णाङ्क nodeoffset, वर्ण *buf, पूर्णाङ्क buflen)
{
	पूर्णाङ्क pdepth = 0, p = 0;
	पूर्णाङ्क offset, depth, namelen;
	अचर वर्ण *name;

	FDT_RO_PROBE(fdt);

	यदि (buflen < 2)
		फर्काऊ -FDT_ERR_NOSPACE;

	लागि (offset = 0, depth = 0;
	     (offset >= 0) && (offset <= nodeoffset);
	     offset = fdt_next_गाँठो(fdt, offset, &depth)) {
		जबसम्म (pdepth > depth) {
			गर {
				p--;
			} जबसम्म (buf[p-1] != '/');
			pdepth--;
		}

		यदि (pdepth >= depth) {
			name = fdt_get_name(fdt, offset, &namelen);
			यदि (!name)
				फर्काऊ namelen;
			यदि ((p + namelen + 1) <= buflen) {
				स_नकल(buf + p, name, namelen);
				p += namelen;
				buf[p++] = '/';
				pdepth++;
			}
		}

		यदि (offset == nodeoffset) {
			यदि (pdepth < (depth + 1))
				फर्काऊ -FDT_ERR_NOSPACE;

			यदि (p > 1) /* special case so that root path is "/", not "" */
				p--;
			buf[p] = '\0';
			फर्काऊ 0;
		}
	}

	यदि ((offset == -FDT_ERR_NOTFOUND) || (offset >= 0))
		फर्काऊ -FDT_ERR_BADOFFSET;
	अन्यथा यदि (offset == -FDT_ERR_BADOFFSET)
		फर्काऊ -FDT_ERR_BADSTRUCTURE;

	फर्काऊ offset; /* error from fdt_next_node() */
}

पूर्णाङ्क fdt_supernode_atdepth_offset(अचर शून्य *fdt, पूर्णाङ्क nodeoffset,
				 पूर्णाङ्क supernodedepth, पूर्णाङ्क *nodedepth)
{
	पूर्णाङ्क offset, depth;
	पूर्णाङ्क supernodeoffset = -FDT_ERR_INTERNAL;

	FDT_RO_PROBE(fdt);

	यदि (supernodedepth < 0)
		फर्काऊ -FDT_ERR_NOTFOUND;

	लागि (offset = 0, depth = 0;
	     (offset >= 0) && (offset <= nodeoffset);
	     offset = fdt_next_गाँठो(fdt, offset, &depth)) {
		यदि (depth == supernodedepth)
			supernodeoffset = offset;

		यदि (offset == nodeoffset) {
			यदि (nodedepth)
				*nodedepth = depth;

			यदि (supernodedepth > depth)
				फर्काऊ -FDT_ERR_NOTFOUND;
			अन्यथा
				फर्काऊ supernodeoffset;
		}
	}

	यदि (!can_assume(VALID_आगत)) {
		यदि ((offset == -FDT_ERR_NOTFOUND) || (offset >= 0))
			फर्काऊ -FDT_ERR_BADOFFSET;
		अन्यथा यदि (offset == -FDT_ERR_BADOFFSET)
			फर्काऊ -FDT_ERR_BADSTRUCTURE;
	}

	फर्काऊ offset; /* error from fdt_next_node() */
}

पूर्णाङ्क fdt_गाँठो_depth(अचर शून्य *fdt, पूर्णाङ्क nodeoffset)
{
	पूर्णाङ्क nodedepth;
	पूर्णाङ्क err;

	err = fdt_supernode_atdepth_offset(fdt, nodeoffset, 0, &nodedepth);
	यदि (err)
		फर्काऊ (can_assume(LIBFDT_FLAWLESS) || err < 0) ? err :
			-FDT_ERR_INTERNAL;
	फर्काऊ nodedepth;
}

पूर्णाङ्क fdt_parent_offset(अचर शून्य *fdt, पूर्णाङ्क nodeoffset)
{
	पूर्णाङ्क nodedepth = fdt_गाँठो_depth(fdt, nodeoffset);

	यदि (nodedepth < 0)
		फर्काऊ nodedepth;
	फर्काऊ fdt_supernode_atdepth_offset(fdt, nodeoffset,
					    nodedepth - 1, रिक्त);
}

पूर्णाङ्क fdt_गाँठो_offset_by_prop_मान(अचर शून्य *fdt, पूर्णाङ्क startoffset,
				  अचर वर्ण *propname,
				  अचर शून्य *propval, पूर्णाङ्क proplen)
{
	पूर्णाङ्क offset;
	अचर शून्य *val;
	पूर्णाङ्क len;

	FDT_RO_PROBE(fdt);

	/* FIXME: The algorithm here is pretty horrible: we scan each
	 * property of a node in fdt_getprop(), then if that didn't
	 * find what we want, we scan over them again making our way
	 * to the next node.  Still it's the easiest to implement
	 * approach; performance can come later. */
	लागि (offset = fdt_next_गाँठो(fdt, startoffset, रिक्त);
	     offset >= 0;
	     offset = fdt_next_गाँठो(fdt, offset, रिक्त)) {
		val = fdt_getprop(fdt, offset, propname, &len);
		यदि (val && (len == proplen)
		    && (स_भेद(val, propval, len) == 0))
			फर्काऊ offset;
	}

	फर्काऊ offset; /* error from fdt_next_node() */
}

पूर्णाङ्क fdt_गाँठो_offset_by_phandle(अचर शून्य *fdt, uint32_t phandle)
{
	पूर्णाङ्क offset;

	यदि ((phandle == 0) || (phandle == ~0U))
		फर्काऊ -FDT_ERR_BADPHANDLE;

	FDT_RO_PROBE(fdt);

	/* FIXME: The algorithm here is pretty horrible: we
	 * potentially scan each property of a node in
	 * fdt_get_phandle(), then if that didn't find what
	 * we want, we scan over them again making our way to the next
	 * node.  Still it's the easiest to implement approach;
	 * performance can come later. */
	लागि (offset = fdt_next_गाँठो(fdt, -1, रिक्त);
	     offset >= 0;
	     offset = fdt_next_गाँठो(fdt, offset, रिक्त)) {
		यदि (fdt_get_phandle(fdt, offset) == phandle)
			फर्काऊ offset;
	}

	फर्काऊ offset; /* error from fdt_next_node() */
}

पूर्णाङ्क fdt_stringlist_contains(अचर वर्ण *strlist, पूर्णाङ्क listlen, अचर वर्ण *str)
{
	पूर्णाङ्क len = म_माप(str);
	अचर वर्ण *p;

	जबसम्म (listlen >= len) {
		यदि (स_भेद(str, strlist, len+1) == 0)
			फर्काऊ 1;
		p = स_प्रथम(strlist, '\0', listlen);
		यदि (!p)
			फर्काऊ 0; /* malformed strlist.. */
		listlen -= (p-strlist) + 1;
		strlist = p + 1;
	}
	फर्काऊ 0;
}

पूर्णाङ्क fdt_stringlist_गन्ती(अचर शून्य *fdt, पूर्णाङ्क nodeoffset, अचर वर्ण *property)
{
	अचर वर्ण *list, *end;
	पूर्णाङ्क length, गन्ती = 0;

	list = fdt_getprop(fdt, nodeoffset, property, &length);
	यदि (!list)
		फर्काऊ length;

	end = list + length;

	जबसम्म (list < end) {
		length = strnlen(list, end - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		यदि (list + length > end)
			फर्काऊ -FDT_ERR_BADVALUE;

		list += length;
		गन्ती++;
	}

	फर्काऊ गन्ती;
}

पूर्णाङ्क fdt_stringlist_खोज(अचर शून्य *fdt, पूर्णाङ्क nodeoffset, अचर वर्ण *property,
			  अचर वर्ण *string)
{
	पूर्णाङ्क length, len, idx = 0;
	अचर वर्ण *list, *end;

	list = fdt_getprop(fdt, nodeoffset, property, &length);
	यदि (!list)
		फर्काऊ length;

	len = म_माप(string) + 1;
	end = list + length;

	जबसम्म (list < end) {
		length = strnlen(list, end - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		यदि (list + length > end)
			फर्काऊ -FDT_ERR_BADVALUE;

		यदि (length == len && स_भेद(list, string, length) == 0)
			फर्काऊ idx;

		list += length;
		idx++;
	}

	फर्काऊ -FDT_ERR_NOTFOUND;
}

अचर वर्ण *fdt_stringlist_get(अचर शून्य *fdt, पूर्णाङ्क nodeoffset,
			       अचर वर्ण *property, पूर्णाङ्क idx,
			       पूर्णाङ्क *lenp)
{
	अचर वर्ण *list, *end;
	पूर्णाङ्क length;

	list = fdt_getprop(fdt, nodeoffset, property, &length);
	यदि (!list) {
		यदि (lenp)
			*lenp = length;

		फर्काऊ रिक्त;
	}

	end = list + length;

	जबसम्म (list < end) {
		length = strnlen(list, end - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		यदि (list + length > end) {
			यदि (lenp)
				*lenp = -FDT_ERR_BADVALUE;

			फर्काऊ रिक्त;
		}

		यदि (idx == 0) {
			यदि (lenp)
				*lenp = length - 1;

			फर्काऊ list;
		}

		list += length;
		idx--;
	}

	यदि (lenp)
		*lenp = -FDT_ERR_NOTFOUND;

	फर्काऊ रिक्त;
}

पूर्णाङ्क fdt_गाँठो_check_compatible(अचर शून्य *fdt, पूर्णाङ्क nodeoffset,
			      अचर वर्ण *compatible)
{
	अचर शून्य *prop;
	पूर्णाङ्क len;

	prop = fdt_getprop(fdt, nodeoffset, "compatible", &len);
	यदि (!prop)
		फर्काऊ len;

	फर्काऊ !fdt_stringlist_contains(prop, len, compatible);
}

पूर्णाङ्क fdt_गाँठो_offset_by_compatible(अचर शून्य *fdt, पूर्णाङ्क startoffset,
				  अचर वर्ण *compatible)
{
	पूर्णाङ्क offset, err;

	FDT_RO_PROBE(fdt);

	/* FIXME: The algorithm here is pretty horrible: we scan each
	 * property of a node in fdt_node_check_compatible(), then if
	 * that didn't find what we want, we scan over them again
	 * making our way to the next node.  Still it's the easiest to
	 * implement approach; performance can come later. */
	लागि (offset = fdt_next_गाँठो(fdt, startoffset, रिक्त);
	     offset >= 0;
	     offset = fdt_next_गाँठो(fdt, offset, रिक्त)) {
		err = fdt_गाँठो_check_compatible(fdt, offset, compatible);
		यदि ((err < 0) && (err != -FDT_ERR_NOTFOUND))
			फर्काऊ err;
		अन्यथा यदि (err == 0)
			फर्काऊ offset;
	}

	फर्काऊ offset; /* error from fdt_next_node() */
}
