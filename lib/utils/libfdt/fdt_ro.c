// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#समावेश "libfdt_env.h"

#समावेश <fdt.h>
#समावेश <libfdt.h>

#समावेश "libfdt_internal.h"

अटल पूर्णांक fdt_nodename_eq_(स्थिर व्योम *fdt, पूर्णांक offset,
			    स्थिर अक्षर *s, पूर्णांक len)
{
	पूर्णांक olen;
	स्थिर अक्षर *p = fdt_get_name(fdt, offset, &olen);

	अगर (!p || olen < len)
		/* short match */
		वापस 0;

	अगर (स_भेद(p, s, len) != 0)
		वापस 0;

	अगर (p[len] == '\0')
		वापस 1;
	अन्यथा अगर (!स_प्रथम(s, '@', len) && (p[len] == '@'))
		वापस 1;
	अन्यथा
		वापस 0;
}

स्थिर अक्षर *fdt_get_string(स्थिर व्योम *fdt, पूर्णांक stroffset, पूर्णांक *lenp)
{
	int32_t totalsize;
	uint32_t absoffset;
	माप_प्रकार len;
	पूर्णांक err;
	स्थिर अक्षर *s, *n;

	अगर (can_assume(VALID_निवेश)) {
		s = (स्थिर अक्षर *)fdt + fdt_off_dt_strings(fdt) + stroffset;

		अगर (lenp)
			*lenp = म_माप(s);
		वापस s;
	}
	totalsize = fdt_ro_probe_(fdt);
	err = totalsize;
	अगर (totalsize < 0)
		जाओ fail;

	err = -FDT_ERR_BADOFFSET;
	absoffset = stroffset + fdt_off_dt_strings(fdt);
	अगर (absoffset >= (अचिन्हित)totalsize)
		जाओ fail;
	len = totalsize - absoffset;

	अगर (fdt_magic(fdt) == FDT_MAGIC) {
		अगर (stroffset < 0)
			जाओ fail;
		अगर (can_assume(LATEST) || fdt_version(fdt) >= 17) {
			अगर ((अचिन्हित)stroffset >= fdt_आकार_dt_strings(fdt))
				जाओ fail;
			अगर ((fdt_आकार_dt_strings(fdt) - stroffset) < len)
				len = fdt_आकार_dt_strings(fdt) - stroffset;
		}
	} अन्यथा अगर (fdt_magic(fdt) == FDT_SW_MAGIC) {
		अचिन्हित पूर्णांक sw_stroffset = -stroffset;

		अगर ((stroffset >= 0) ||
		    (sw_stroffset > fdt_आकार_dt_strings(fdt)))
			जाओ fail;
		अगर (sw_stroffset < len)
			len = sw_stroffset;
	} अन्यथा {
		err = -FDT_ERR_INTERNAL;
		जाओ fail;
	}

	s = (स्थिर अक्षर *)fdt + absoffset;
	n = स_प्रथम(s, '\0', len);
	अगर (!n) {
		/* missing terminating NULL */
		err = -FDT_ERR_TRUNCATED;
		जाओ fail;
	}

	अगर (lenp)
		*lenp = n - s;
	वापस s;

fail:
	अगर (lenp)
		*lenp = err;
	वापस शून्य;
}

स्थिर अक्षर *fdt_string(स्थिर व्योम *fdt, पूर्णांक stroffset)
{
	वापस fdt_get_string(fdt, stroffset, शून्य);
}

अटल पूर्णांक fdt_string_eq_(स्थिर व्योम *fdt, पूर्णांक stroffset,
			  स्थिर अक्षर *s, पूर्णांक len)
{
	पूर्णांक slen;
	स्थिर अक्षर *p = fdt_get_string(fdt, stroffset, &slen);

	वापस p && (slen == len) && (स_भेद(p, s, len) == 0);
}

पूर्णांक fdt_find_max_phandle(स्थिर व्योम *fdt, uint32_t *phandle)
{
	uint32_t max = 0;
	पूर्णांक offset = -1;

	जबतक (true) {
		uint32_t मूल्य;

		offset = fdt_अग्र_गाँठ(fdt, offset, शून्य);
		अगर (offset < 0) {
			अगर (offset == -FDT_ERR_NOTFOUND)
				अवरोध;

			वापस offset;
		}

		मूल्य = fdt_get_phandle(fdt, offset);

		अगर (मूल्य > max)
			max = मूल्य;
	}

	अगर (phandle)
		*phandle = max;

	वापस 0;
}

पूर्णांक fdt_generate_phandle(स्थिर व्योम *fdt, uint32_t *phandle)
{
	uint32_t max;
	पूर्णांक err;

	err = fdt_find_max_phandle(fdt, &max);
	अगर (err < 0)
		वापस err;

	अगर (max == FDT_MAX_PHANDLE)
		वापस -FDT_ERR_NOPHANDLES;

	अगर (phandle)
		*phandle = max + 1;

	वापस 0;
}

अटल स्थिर काष्ठा fdt_reserve_entry *fdt_mem_rsv(स्थिर व्योम *fdt, पूर्णांक n)
{
	अचिन्हित पूर्णांक offset = n * माप(काष्ठा fdt_reserve_entry);
	अचिन्हित पूर्णांक absoffset = fdt_off_mem_rsvmap(fdt) + offset;

	अगर (!can_assume(VALID_निवेश)) {
		अगर (absoffset < fdt_off_mem_rsvmap(fdt))
			वापस शून्य;
		अगर (absoffset > fdt_totalsize(fdt) -
		    माप(काष्ठा fdt_reserve_entry))
			वापस शून्य;
	}
	वापस fdt_mem_rsv_(fdt, n);
}

पूर्णांक fdt_get_mem_rsv(स्थिर व्योम *fdt, पूर्णांक n, uint64_t *address, uint64_t *आकार)
{
	स्थिर काष्ठा fdt_reserve_entry *re;

	FDT_RO_PROBE(fdt);
	re = fdt_mem_rsv(fdt, n);
	अगर (!can_assume(VALID_निवेश) && !re)
		वापस -FDT_ERR_BADOFFSET;

	*address = fdt64_ld_(&re->address);
	*आकार = fdt64_ld_(&re->आकार);
	वापस 0;
}

पूर्णांक fdt_अंक_mem_rsv(स्थिर व्योम *fdt)
{
	पूर्णांक i;
	स्थिर काष्ठा fdt_reserve_entry *re;

	क्रम (i = 0; (re = fdt_mem_rsv(fdt, i)) != शून्य; i++) {
		अगर (fdt64_ld_(&re->आकार) == 0)
			वापस i;
	}
	वापस -FDT_ERR_TRUNCATED;
}

अटल पूर्णांक nextprop_(स्थिर व्योम *fdt, पूर्णांक offset)
{
	uint32_t tag;
	पूर्णांक nextoffset;

	करो {
		tag = fdt_अग्र_tag(fdt, offset, &nextoffset);

		चयन (tag) {
		हाल FDT_END:
			अगर (nextoffset >= 0)
				वापस -FDT_ERR_BADSTRUCTURE;
			अन्यथा
				वापस nextoffset;

		हाल FDT_PROP:
			वापस offset;
		}
		offset = nextoffset;
	} जबतक (tag == FDT_NOP);

	वापस -FDT_ERR_NOTFOUND;
}

पूर्णांक fdt_subnode_offset_namelen(स्थिर व्योम *fdt, पूर्णांक offset,
			       स्थिर अक्षर *name, पूर्णांक namelen)
{
	पूर्णांक depth;

	FDT_RO_PROBE(fdt);

	क्रम (depth = 0;
	     (offset >= 0) && (depth >= 0);
	     offset = fdt_अग्र_गाँठ(fdt, offset, &depth))
		अगर ((depth == 1)
		    && fdt_nodename_eq_(fdt, offset, name, namelen))
			वापस offset;

	अगर (depth < 0)
		वापस -FDT_ERR_NOTFOUND;
	वापस offset; /* error */
}

पूर्णांक fdt_subnode_offset(स्थिर व्योम *fdt, पूर्णांक parentoffset,
		       स्थिर अक्षर *name)
{
	वापस fdt_subnode_offset_namelen(fdt, parentoffset, name, म_माप(name));
}

पूर्णांक fdt_path_offset_namelen(स्थिर व्योम *fdt, स्थिर अक्षर *path, पूर्णांक namelen)
{
	स्थिर अक्षर *end = path + namelen;
	स्थिर अक्षर *p = path;
	पूर्णांक offset = 0;

	FDT_RO_PROBE(fdt);

	/* see if we have an alias */
	अगर (*path != '/') {
		स्थिर अक्षर *q = स_प्रथम(path, '/', end - p);

		अगर (!q)
			q = end;

		p = fdt_get_alias_namelen(fdt, p, q - p);
		अगर (!p)
			वापस -FDT_ERR_BADPATH;
		offset = fdt_path_offset(fdt, p);

		p = q;
	}

	जबतक (p < end) {
		स्थिर अक्षर *q;

		जबतक (*p == '/') {
			p++;
			अगर (p == end)
				वापस offset;
		}
		q = स_प्रथम(p, '/', end - p);
		अगर (! q)
			q = end;

		offset = fdt_subnode_offset_namelen(fdt, offset, p, q-p);
		अगर (offset < 0)
			वापस offset;

		p = q;
	}

	वापस offset;
}

पूर्णांक fdt_path_offset(स्थिर व्योम *fdt, स्थिर अक्षर *path)
{
	वापस fdt_path_offset_namelen(fdt, path, म_माप(path));
}

स्थिर अक्षर *fdt_get_name(स्थिर व्योम *fdt, पूर्णांक nodeoffset, पूर्णांक *len)
{
	स्थिर काष्ठा fdt_गाँठ_header *nh = fdt_offset_ptr_(fdt, nodeoffset);
	स्थिर अक्षर *nameptr;
	पूर्णांक err;

	अगर (((err = fdt_ro_probe_(fdt)) < 0)
	    || ((err = fdt_check_गाँठ_offset(fdt, nodeoffset)) < 0))
			जाओ fail;

	nameptr = nh->name;

	अगर (!can_assume(LATEST) && fdt_version(fdt) < 0x10) {
		/*
		 * For old FDT versions, match the naming conventions of V16:
		 * give only the leaf name (after all /). The actual tree
		 * contents are loosely checked.
		 */
		स्थिर अक्षर *leaf;
		leaf = म_खोजप(nameptr, '/');
		अगर (leaf == शून्य) {
			err = -FDT_ERR_BADSTRUCTURE;
			जाओ fail;
		}
		nameptr = leaf+1;
	}

	अगर (len)
		*len = म_माप(nameptr);

	वापस nameptr;

 fail:
	अगर (len)
		*len = err;
	वापस शून्य;
}

पूर्णांक fdt_first_property_offset(स्थिर व्योम *fdt, पूर्णांक nodeoffset)
{
	पूर्णांक offset;

	अगर ((offset = fdt_check_गाँठ_offset(fdt, nodeoffset)) < 0)
		वापस offset;

	वापस nextprop_(fdt, offset);
}

पूर्णांक fdt_अग्र_property_offset(स्थिर व्योम *fdt, पूर्णांक offset)
{
	अगर ((offset = fdt_check_prop_offset_(fdt, offset)) < 0)
		वापस offset;

	वापस nextprop_(fdt, offset);
}

अटल स्थिर काष्ठा fdt_property *fdt_get_property_by_offset_(स्थिर व्योम *fdt,
						              पूर्णांक offset,
						              पूर्णांक *lenp)
{
	पूर्णांक err;
	स्थिर काष्ठा fdt_property *prop;

	अगर (!can_assume(VALID_निवेश) &&
	    (err = fdt_check_prop_offset_(fdt, offset)) < 0) {
		अगर (lenp)
			*lenp = err;
		वापस शून्य;
	}

	prop = fdt_offset_ptr_(fdt, offset);

	अगर (lenp)
		*lenp = fdt32_ld_(&prop->len);

	वापस prop;
}

स्थिर काष्ठा fdt_property *fdt_get_property_by_offset(स्थिर व्योम *fdt,
						      पूर्णांक offset,
						      पूर्णांक *lenp)
{
	/* Prior to version 16, properties may need realignment
	 * and this API does not work. fdt_getprop_*() will, however. */

	अगर (!can_assume(LATEST) && fdt_version(fdt) < 0x10) {
		अगर (lenp)
			*lenp = -FDT_ERR_BADVERSION;
		वापस शून्य;
	}

	वापस fdt_get_property_by_offset_(fdt, offset, lenp);
}

अटल स्थिर काष्ठा fdt_property *fdt_get_property_namelen_(स्थिर व्योम *fdt,
						            पूर्णांक offset,
						            स्थिर अक्षर *name,
						            पूर्णांक namelen,
							    पूर्णांक *lenp,
							    पूर्णांक *poffset)
{
	क्रम (offset = fdt_first_property_offset(fdt, offset);
	     (offset >= 0);
	     (offset = fdt_अग्र_property_offset(fdt, offset))) {
		स्थिर काष्ठा fdt_property *prop;

		prop = fdt_get_property_by_offset_(fdt, offset, lenp);
		अगर (!can_assume(LIBFDT_FLAWLESS) && !prop) {
			offset = -FDT_ERR_INTERNAL;
			अवरोध;
		}
		अगर (fdt_string_eq_(fdt, fdt32_ld_(&prop->nameoff),
				   name, namelen)) {
			अगर (poffset)
				*poffset = offset;
			वापस prop;
		}
	}

	अगर (lenp)
		*lenp = offset;
	वापस शून्य;
}


स्थिर काष्ठा fdt_property *fdt_get_property_namelen(स्थिर व्योम *fdt,
						    पूर्णांक offset,
						    स्थिर अक्षर *name,
						    पूर्णांक namelen, पूर्णांक *lenp)
{
	/* Prior to version 16, properties may need realignment
	 * and this API does not work. fdt_getprop_*() will, however. */
	अगर (!can_assume(LATEST) && fdt_version(fdt) < 0x10) {
		अगर (lenp)
			*lenp = -FDT_ERR_BADVERSION;
		वापस शून्य;
	}

	वापस fdt_get_property_namelen_(fdt, offset, name, namelen, lenp,
					 शून्य);
}


स्थिर काष्ठा fdt_property *fdt_get_property(स्थिर व्योम *fdt,
					    पूर्णांक nodeoffset,
					    स्थिर अक्षर *name, पूर्णांक *lenp)
{
	वापस fdt_get_property_namelen(fdt, nodeoffset, name,
					म_माप(name), lenp);
}

स्थिर व्योम *fdt_getprop_namelen(स्थिर व्योम *fdt, पूर्णांक nodeoffset,
				स्थिर अक्षर *name, पूर्णांक namelen, पूर्णांक *lenp)
{
	पूर्णांक poffset;
	स्थिर काष्ठा fdt_property *prop;

	prop = fdt_get_property_namelen_(fdt, nodeoffset, name, namelen, lenp,
					 &poffset);
	अगर (!prop)
		वापस शून्य;

	/* Handle realignment */
	अगर (!can_assume(LATEST) && fdt_version(fdt) < 0x10 &&
	    (poffset + माप(*prop)) % 8 && fdt32_ld_(&prop->len) >= 8)
		वापस prop->तथ्य + 4;
	वापस prop->तथ्य;
}

स्थिर व्योम *fdt_getprop_by_offset(स्थिर व्योम *fdt, पूर्णांक offset,
				  स्थिर अक्षर **namep, पूर्णांक *lenp)
{
	स्थिर काष्ठा fdt_property *prop;

	prop = fdt_get_property_by_offset_(fdt, offset, lenp);
	अगर (!prop)
		वापस शून्य;
	अगर (namep) {
		स्थिर अक्षर *name;
		पूर्णांक namelen;

		अगर (!can_assume(VALID_निवेश)) {
			name = fdt_get_string(fdt, fdt32_ld_(&prop->nameoff),
					      &namelen);
			अगर (!name) {
				अगर (lenp)
					*lenp = namelen;
				वापस शून्य;
			}
			*namep = name;
		} अन्यथा {
			*namep = fdt_string(fdt, fdt32_ld_(&prop->nameoff));
		}
	}

	/* Handle realignment */
	अगर (!can_assume(LATEST) && fdt_version(fdt) < 0x10 &&
	    (offset + माप(*prop)) % 8 && fdt32_ld_(&prop->len) >= 8)
		वापस prop->तथ्य + 4;
	वापस prop->तथ्य;
}

स्थिर व्योम *fdt_getprop(स्थिर व्योम *fdt, पूर्णांक nodeoffset,
			स्थिर अक्षर *name, पूर्णांक *lenp)
{
	वापस fdt_getprop_namelen(fdt, nodeoffset, name, म_माप(name), lenp);
}

uint32_t fdt_get_phandle(स्थिर व्योम *fdt, पूर्णांक nodeoffset)
{
	स्थिर fdt32_t *php;
	पूर्णांक len;

	/* FIXME: This is a bit sub-optimal, since we potentially scan
	 * over all the properties twice. */
	php = fdt_getprop(fdt, nodeoffset, "phandle", &len);
	अगर (!php || (len != माप(*php))) {
		php = fdt_getprop(fdt, nodeoffset, "linux,phandle", &len);
		अगर (!php || (len != माप(*php)))
			वापस 0;
	}

	वापस fdt32_ld_(php);
}

स्थिर अक्षर *fdt_get_alias_namelen(स्थिर व्योम *fdt,
				  स्थिर अक्षर *name, पूर्णांक namelen)
{
	पूर्णांक aliasoffset;

	aliasoffset = fdt_path_offset(fdt, "/aliases");
	अगर (aliasoffset < 0)
		वापस शून्य;

	वापस fdt_getprop_namelen(fdt, aliasoffset, name, namelen, शून्य);
}

स्थिर अक्षर *fdt_get_alias(स्थिर व्योम *fdt, स्थिर अक्षर *name)
{
	वापस fdt_get_alias_namelen(fdt, name, म_माप(name));
}

पूर्णांक fdt_get_path(स्थिर व्योम *fdt, पूर्णांक nodeoffset, अक्षर *buf, पूर्णांक buflen)
{
	पूर्णांक pdepth = 0, p = 0;
	पूर्णांक offset, depth, namelen;
	स्थिर अक्षर *name;

	FDT_RO_PROBE(fdt);

	अगर (buflen < 2)
		वापस -FDT_ERR_NOSPACE;

	क्रम (offset = 0, depth = 0;
	     (offset >= 0) && (offset <= nodeoffset);
	     offset = fdt_अग्र_गाँठ(fdt, offset, &depth)) {
		जबतक (pdepth > depth) {
			करो {
				p--;
			} जबतक (buf[p-1] != '/');
			pdepth--;
		}

		अगर (pdepth >= depth) {
			name = fdt_get_name(fdt, offset, &namelen);
			अगर (!name)
				वापस namelen;
			अगर ((p + namelen + 1) <= buflen) {
				स_नकल(buf + p, name, namelen);
				p += namelen;
				buf[p++] = '/';
				pdepth++;
			}
		}

		अगर (offset == nodeoffset) {
			अगर (pdepth < (depth + 1))
				वापस -FDT_ERR_NOSPACE;

			अगर (p > 1) /* special case so that root path is "/", not "" */
				p--;
			buf[p] = '\0';
			वापस 0;
		}
	}

	अगर ((offset == -FDT_ERR_NOTFOUND) || (offset >= 0))
		वापस -FDT_ERR_BADOFFSET;
	अन्यथा अगर (offset == -FDT_ERR_BADOFFSET)
		वापस -FDT_ERR_BADSTRUCTURE;

	वापस offset; /* error from fdt_next_node() */
}

पूर्णांक fdt_supernode_atdepth_offset(स्थिर व्योम *fdt, पूर्णांक nodeoffset,
				 पूर्णांक supernodedepth, पूर्णांक *nodedepth)
{
	पूर्णांक offset, depth;
	पूर्णांक supernodeoffset = -FDT_ERR_INTERNAL;

	FDT_RO_PROBE(fdt);

	अगर (supernodedepth < 0)
		वापस -FDT_ERR_NOTFOUND;

	क्रम (offset = 0, depth = 0;
	     (offset >= 0) && (offset <= nodeoffset);
	     offset = fdt_अग्र_गाँठ(fdt, offset, &depth)) {
		अगर (depth == supernodedepth)
			supernodeoffset = offset;

		अगर (offset == nodeoffset) {
			अगर (nodedepth)
				*nodedepth = depth;

			अगर (supernodedepth > depth)
				वापस -FDT_ERR_NOTFOUND;
			अन्यथा
				वापस supernodeoffset;
		}
	}

	अगर (!can_assume(VALID_निवेश)) {
		अगर ((offset == -FDT_ERR_NOTFOUND) || (offset >= 0))
			वापस -FDT_ERR_BADOFFSET;
		अन्यथा अगर (offset == -FDT_ERR_BADOFFSET)
			वापस -FDT_ERR_BADSTRUCTURE;
	}

	वापस offset; /* error from fdt_next_node() */
}

पूर्णांक fdt_गाँठ_depth(स्थिर व्योम *fdt, पूर्णांक nodeoffset)
{
	पूर्णांक nodedepth;
	पूर्णांक err;

	err = fdt_supernode_atdepth_offset(fdt, nodeoffset, 0, &nodedepth);
	अगर (err)
		वापस (can_assume(LIBFDT_FLAWLESS) || err < 0) ? err :
			-FDT_ERR_INTERNAL;
	वापस nodedepth;
}

पूर्णांक fdt_parent_offset(स्थिर व्योम *fdt, पूर्णांक nodeoffset)
{
	पूर्णांक nodedepth = fdt_गाँठ_depth(fdt, nodeoffset);

	अगर (nodedepth < 0)
		वापस nodedepth;
	वापस fdt_supernode_atdepth_offset(fdt, nodeoffset,
					    nodedepth - 1, शून्य);
}

पूर्णांक fdt_गाँठ_offset_by_prop_मूल्य(स्थिर व्योम *fdt, पूर्णांक startoffset,
				  स्थिर अक्षर *propname,
				  स्थिर व्योम *propval, पूर्णांक proplen)
{
	पूर्णांक offset;
	स्थिर व्योम *val;
	पूर्णांक len;

	FDT_RO_PROBE(fdt);

	/* FIXME: The algorithm here is pretty horrible: we scan each
	 * property of a node in fdt_getprop(), then if that didn't
	 * find what we want, we scan over them again making our way
	 * to the next node.  Still it's the easiest to implement
	 * approach; performance can come later. */
	क्रम (offset = fdt_अग्र_गाँठ(fdt, startoffset, शून्य);
	     offset >= 0;
	     offset = fdt_अग्र_गाँठ(fdt, offset, शून्य)) {
		val = fdt_getprop(fdt, offset, propname, &len);
		अगर (val && (len == proplen)
		    && (स_भेद(val, propval, len) == 0))
			वापस offset;
	}

	वापस offset; /* error from fdt_next_node() */
}

पूर्णांक fdt_गाँठ_offset_by_phandle(स्थिर व्योम *fdt, uint32_t phandle)
{
	पूर्णांक offset;

	अगर ((phandle == 0) || (phandle == ~0U))
		वापस -FDT_ERR_BADPHANDLE;

	FDT_RO_PROBE(fdt);

	/* FIXME: The algorithm here is pretty horrible: we
	 * potentially scan each property of a node in
	 * fdt_get_phandle(), then if that didn't find what
	 * we want, we scan over them again making our way to the next
	 * node.  Still it's the easiest to implement approach;
	 * performance can come later. */
	क्रम (offset = fdt_अग्र_गाँठ(fdt, -1, शून्य);
	     offset >= 0;
	     offset = fdt_अग्र_गाँठ(fdt, offset, शून्य)) {
		अगर (fdt_get_phandle(fdt, offset) == phandle)
			वापस offset;
	}

	वापस offset; /* error from fdt_next_node() */
}

पूर्णांक fdt_stringlist_contains(स्थिर अक्षर *strlist, पूर्णांक listlen, स्थिर अक्षर *str)
{
	पूर्णांक len = म_माप(str);
	स्थिर अक्षर *p;

	जबतक (listlen >= len) {
		अगर (स_भेद(str, strlist, len+1) == 0)
			वापस 1;
		p = स_प्रथम(strlist, '\0', listlen);
		अगर (!p)
			वापस 0; /* malformed strlist.. */
		listlen -= (p-strlist) + 1;
		strlist = p + 1;
	}
	वापस 0;
}

पूर्णांक fdt_stringlist_गिनती(स्थिर व्योम *fdt, पूर्णांक nodeoffset, स्थिर अक्षर *property)
{
	स्थिर अक्षर *list, *end;
	पूर्णांक लंबाई, गिनती = 0;

	list = fdt_getprop(fdt, nodeoffset, property, &लंबाई);
	अगर (!list)
		वापस लंबाई;

	end = list + लंबाई;

	जबतक (list < end) {
		लंबाई = strnlen(list, end - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		अगर (list + लंबाई > end)
			वापस -FDT_ERR_BADVALUE;

		list += लंबाई;
		गिनती++;
	}

	वापस गिनती;
}

पूर्णांक fdt_stringlist_खोज(स्थिर व्योम *fdt, पूर्णांक nodeoffset, स्थिर अक्षर *property,
			  स्थिर अक्षर *string)
{
	पूर्णांक लंबाई, len, idx = 0;
	स्थिर अक्षर *list, *end;

	list = fdt_getprop(fdt, nodeoffset, property, &लंबाई);
	अगर (!list)
		वापस लंबाई;

	len = म_माप(string) + 1;
	end = list + लंबाई;

	जबतक (list < end) {
		लंबाई = strnlen(list, end - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		अगर (list + लंबाई > end)
			वापस -FDT_ERR_BADVALUE;

		अगर (लंबाई == len && स_भेद(list, string, लंबाई) == 0)
			वापस idx;

		list += लंबाई;
		idx++;
	}

	वापस -FDT_ERR_NOTFOUND;
}

स्थिर अक्षर *fdt_stringlist_get(स्थिर व्योम *fdt, पूर्णांक nodeoffset,
			       स्थिर अक्षर *property, पूर्णांक idx,
			       पूर्णांक *lenp)
{
	स्थिर अक्षर *list, *end;
	पूर्णांक लंबाई;

	list = fdt_getprop(fdt, nodeoffset, property, &लंबाई);
	अगर (!list) {
		अगर (lenp)
			*lenp = लंबाई;

		वापस शून्य;
	}

	end = list + लंबाई;

	जबतक (list < end) {
		लंबाई = strnlen(list, end - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		अगर (list + लंबाई > end) {
			अगर (lenp)
				*lenp = -FDT_ERR_BADVALUE;

			वापस शून्य;
		}

		अगर (idx == 0) {
			अगर (lenp)
				*lenp = लंबाई - 1;

			वापस list;
		}

		list += लंबाई;
		idx--;
	}

	अगर (lenp)
		*lenp = -FDT_ERR_NOTFOUND;

	वापस शून्य;
}

पूर्णांक fdt_गाँठ_check_compatible(स्थिर व्योम *fdt, पूर्णांक nodeoffset,
			      स्थिर अक्षर *compatible)
{
	स्थिर व्योम *prop;
	पूर्णांक len;

	prop = fdt_getprop(fdt, nodeoffset, "compatible", &len);
	अगर (!prop)
		वापस len;

	वापस !fdt_stringlist_contains(prop, len, compatible);
}

पूर्णांक fdt_गाँठ_offset_by_compatible(स्थिर व्योम *fdt, पूर्णांक startoffset,
				  स्थिर अक्षर *compatible)
{
	पूर्णांक offset, err;

	FDT_RO_PROBE(fdt);

	/* FIXME: The algorithm here is pretty horrible: we scan each
	 * property of a node in fdt_node_check_compatible(), then if
	 * that didn't find what we want, we scan over them again
	 * making our way to the next node.  Still it's the easiest to
	 * implement approach; performance can come later. */
	क्रम (offset = fdt_अग्र_गाँठ(fdt, startoffset, शून्य);
	     offset >= 0;
	     offset = fdt_अग्र_गाँठ(fdt, offset, शून्य)) {
		err = fdt_गाँठ_check_compatible(fdt, offset, compatible);
		अगर ((err < 0) && (err != -FDT_ERR_NOTFOUND))
			वापस err;
		अन्यथा अगर (err == 0)
			वापस offset;
	}

	वापस offset; /* error from fdt_next_node() */
}
