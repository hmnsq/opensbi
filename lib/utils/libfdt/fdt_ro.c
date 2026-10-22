// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#अन्तर्भाव "libfdt_env.h"

#अन्तर्भाव <fdt.h>
#अन्तर्भाव <libfdt.h>

#अन्तर्भाव "libfdt_internal.h"

स्थिर पूर्णांक fdt_nodename_eq_(स्थिरांक शून्य *fdt, पूर्णांक offset,
			    स्थिरांक अक्षर *s, पूर्णांक len)
{
	पूर्णांक olen;
	स्थिरांक अक्षर *p = fdt_get_name(fdt, offset, &olen);

	जर (!p || olen < len)
		/* short match */
		परत 0;

	जर (स_भेद(p, s, len) != 0)
		परत 0;

	जर (p[len] == '\0')
		परत 1;
	अन्यथा जर (!स_प्रथम(s, '@', len) && (p[len] == '@'))
		परत 1;
	अन्यथा
		परत 0;
}

स्थिरांक अक्षर *fdt_get_string(स्थिरांक शून्य *fdt, पूर्णांक stroffset, पूर्णांक *lenp)
{
	int32_t totalsize;
	uint32_t absoffset;
	माप_प्रकार len;
	पूर्णांक err;
	स्थिरांक अक्षर *s, *n;

	जर (can_assume(VALID_आदान)) {
		s = (स्थिरांक अक्षर *)fdt + fdt_off_dt_strings(fdt) + stroffset;

		जर (lenp)
			*lenp = म_माप(s);
		परत s;
	}
	totalsize = fdt_ro_probe_(fdt);
	err = totalsize;
	जर (totalsize < 0)
		जा fail;

	err = -FDT_ERR_BADOFFSET;
	absoffset = stroffset + fdt_off_dt_strings(fdt);
	जर (absoffset >= (अचिन्हित)totalsize)
		जा fail;
	len = totalsize - absoffset;

	जर (fdt_magic(fdt) == FDT_MAGIC) {
		जर (stroffset < 0)
			जा fail;
		जर (can_assume(LATEST) || fdt_version(fdt) >= 17) {
			जर ((अचिन्हित)stroffset >= fdt_size_dt_strings(fdt))
				जा fail;
			जर ((fdt_size_dt_strings(fdt) - stroffset) < len)
				len = fdt_size_dt_strings(fdt) - stroffset;
		}
	} अन्यथा जर (fdt_magic(fdt) == FDT_SW_MAGIC) {
		अचिन्हित पूर्णांक sw_stroffset = -stroffset;

		जर ((stroffset >= 0) ||
		    (sw_stroffset > fdt_size_dt_strings(fdt)))
			जा fail;
		जर (sw_stroffset < len)
			len = sw_stroffset;
	} अन्यथा {
		err = -FDT_ERR_INTERNAL;
		जा fail;
	}

	s = (स्थिरांक अक्षर *)fdt + absoffset;
	n = स_प्रथम(s, '\0', len);
	जर (!n) {
		/* missing terminating NULL */
		err = -FDT_ERR_TRUNCATED;
		जा fail;
	}

	जर (lenp)
		*lenp = n - s;
	परत s;

fail:
	जर (lenp)
		*lenp = err;
	परत रिक्त;
}

स्थिरांक अक्षर *fdt_string(स्थिरांक शून्य *fdt, पूर्णांक stroffset)
{
	परत fdt_get_string(fdt, stroffset, रिक्त);
}

स्थिर पूर्णांक fdt_string_eq_(स्थिरांक शून्य *fdt, पूर्णांक stroffset,
			  स्थिरांक अक्षर *s, पूर्णांक len)
{
	पूर्णांक slen;
	स्थिरांक अक्षर *p = fdt_get_string(fdt, stroffset, &slen);

	परत p && (slen == len) && (स_भेद(p, s, len) == 0);
}

पूर्णांक fdt_find_max_phandle(स्थिरांक शून्य *fdt, uint32_t *phandle)
{
	uint32_t max = 0;
	पूर्णांक offset = -1;

	पर्यंत (true) {
		uint32_t मूल्य;

		offset = fdt_next_गाठ(fdt, offset, रिक्त);
		जर (offset < 0) {
			जर (offset == -FDT_ERR_NOTFOUND)
				थांबा;

			परत offset;
		}

		मूल्य = fdt_get_phandle(fdt, offset);

		जर (मूल्य > max)
			max = मूल्य;
	}

	जर (phandle)
		*phandle = max;

	परत 0;
}

पूर्णांक fdt_generate_phandle(स्थिरांक शून्य *fdt, uint32_t *phandle)
{
	uint32_t max;
	पूर्णांक err;

	err = fdt_find_max_phandle(fdt, &max);
	जर (err < 0)
		परत err;

	जर (max == FDT_MAX_PHANDLE)
		परत -FDT_ERR_NOPHANDLES;

	जर (phandle)
		*phandle = max + 1;

	परत 0;
}

स्थिर स्थिरांक संरचना fdt_reserve_entry *fdt_mem_rsv(स्थिरांक शून्य *fdt, पूर्णांक n)
{
	अचिन्हित पूर्णांक offset = n * आकार(संरचना fdt_reserve_entry);
	अचिन्हित पूर्णांक absoffset = fdt_off_mem_rsvmap(fdt) + offset;

	जर (!can_assume(VALID_आदान)) {
		जर (absoffset < fdt_off_mem_rsvmap(fdt))
			परत रिक्त;
		जर (absoffset > fdt_totalsize(fdt) -
		    आकार(संरचना fdt_reserve_entry))
			परत रिक्त;
	}
	परत fdt_mem_rsv_(fdt, n);
}

पूर्णांक fdt_get_mem_rsv(स्थिरांक शून्य *fdt, पूर्णांक n, uint64_t *address, uint64_t *size)
{
	स्थिरांक संरचना fdt_reserve_entry *re;

	FDT_RO_PROBE(fdt);
	re = fdt_mem_rsv(fdt, n);
	जर (!can_assume(VALID_आदान) && !re)
		परत -FDT_ERR_BADOFFSET;

	*address = fdt64_ld_(&re->address);
	*size = fdt64_ld_(&re->size);
	परत 0;
}

पूर्णांक fdt_अंक_mem_rsv(स्थिरांक शून्य *fdt)
{
	पूर्णांक i;
	स्थिरांक संरचना fdt_reserve_entry *re;

	साठी (i = 0; (re = fdt_mem_rsv(fdt, i)) != रिक्त; i++) {
		जर (fdt64_ld_(&re->size) == 0)
			परत i;
	}
	परत -FDT_ERR_TRUNCATED;
}

स्थिर पूर्णांक nextprop_(स्थिरांक शून्य *fdt, पूर्णांक offset)
{
	uint32_t tag;
	पूर्णांक nextoffset;

	करा {
		tag = fdt_next_tag(fdt, offset, &nextoffset);

		निवड (tag) {
		स्थिती FDT_END:
			जर (nextoffset >= 0)
				परत -FDT_ERR_BADSTRUCTURE;
			अन्यथा
				परत nextoffset;

		स्थिती FDT_PROP:
			परत offset;
		}
		offset = nextoffset;
	} पर्यंत (tag == FDT_NOP);

	परत -FDT_ERR_NOTFOUND;
}

पूर्णांक fdt_subnode_offset_namelen(स्थिरांक शून्य *fdt, पूर्णांक offset,
			       स्थिरांक अक्षर *name, पूर्णांक namelen)
{
	पूर्णांक depth;

	FDT_RO_PROBE(fdt);

	साठी (depth = 0;
	     (offset >= 0) && (depth >= 0);
	     offset = fdt_next_गाठ(fdt, offset, &depth))
		जर ((depth == 1)
		    && fdt_nodename_eq_(fdt, offset, name, namelen))
			परत offset;

	जर (depth < 0)
		परत -FDT_ERR_NOTFOUND;
	परत offset; /* error */
}

पूर्णांक fdt_subnode_offset(स्थिरांक शून्य *fdt, पूर्णांक parentoffset,
		       स्थिरांक अक्षर *name)
{
	परत fdt_subnode_offset_namelen(fdt, parentoffset, name, म_माप(name));
}

पूर्णांक fdt_path_offset_namelen(स्थिरांक शून्य *fdt, स्थिरांक अक्षर *path, पूर्णांक namelen)
{
	स्थिरांक अक्षर *end = path + namelen;
	स्थिरांक अक्षर *p = path;
	पूर्णांक offset = 0;

	FDT_RO_PROBE(fdt);

	/* see if we have an alias */
	जर (*path != '/') {
		स्थिरांक अक्षर *q = स_प्रथम(path, '/', end - p);

		जर (!q)
			q = end;

		p = fdt_get_alias_namelen(fdt, p, q - p);
		जर (!p)
			परत -FDT_ERR_BADPATH;
		offset = fdt_path_offset(fdt, p);

		p = q;
	}

	पर्यंत (p < end) {
		स्थिरांक अक्षर *q;

		पर्यंत (*p == '/') {
			p++;
			जर (p == end)
				परत offset;
		}
		q = स_प्रथम(p, '/', end - p);
		जर (! q)
			q = end;

		offset = fdt_subnode_offset_namelen(fdt, offset, p, q-p);
		जर (offset < 0)
			परत offset;

		p = q;
	}

	परत offset;
}

पूर्णांक fdt_path_offset(स्थिरांक शून्य *fdt, स्थिरांक अक्षर *path)
{
	परत fdt_path_offset_namelen(fdt, path, म_माप(path));
}

स्थिरांक अक्षर *fdt_get_name(स्थिरांक शून्य *fdt, पूर्णांक nodeoffset, पूर्णांक *len)
{
	स्थिरांक संरचना fdt_गाठ_header *nh = fdt_offset_ptr_(fdt, nodeoffset);
	स्थिरांक अक्षर *nameptr;
	पूर्णांक err;

	जर (((err = fdt_ro_probe_(fdt)) < 0)
	    || ((err = fdt_check_गाठ_offset(fdt, nodeoffset)) < 0))
			जा fail;

	nameptr = nh->name;

	जर (!can_assume(LATEST) && fdt_version(fdt) < 0x10) {
		/*
		 * For old FDT versions, match the naming conventions of V16:
		 * give only the leaf name (after all /). The actual tree
		 * contents are loosely checked.
		 */
		स्थिरांक अक्षर *leaf;
		leaf = म_खोजप(nameptr, '/');
		जर (leaf == रिक्त) {
			err = -FDT_ERR_BADSTRUCTURE;
			जा fail;
		}
		nameptr = leaf+1;
	}

	जर (len)
		*len = म_माप(nameptr);

	परत nameptr;

 fail:
	जर (len)
		*len = err;
	परत रिक्त;
}

पूर्णांक fdt_first_property_offset(स्थिरांक शून्य *fdt, पूर्णांक nodeoffset)
{
	पूर्णांक offset;

	जर ((offset = fdt_check_गाठ_offset(fdt, nodeoffset)) < 0)
		परत offset;

	परत nextprop_(fdt, offset);
}

पूर्णांक fdt_next_property_offset(स्थिरांक शून्य *fdt, पूर्णांक offset)
{
	जर ((offset = fdt_check_prop_offset_(fdt, offset)) < 0)
		परत offset;

	परत nextprop_(fdt, offset);
}

स्थिर स्थिरांक संरचना fdt_property *fdt_get_property_by_offset_(स्थिरांक शून्य *fdt,
						              पूर्णांक offset,
						              पूर्णांक *lenp)
{
	पूर्णांक err;
	स्थिरांक संरचना fdt_property *prop;

	जर (!can_assume(VALID_आदान) &&
	    (err = fdt_check_prop_offset_(fdt, offset)) < 0) {
		जर (lenp)
			*lenp = err;
		परत रिक्त;
	}

	prop = fdt_offset_ptr_(fdt, offset);

	जर (lenp)
		*lenp = fdt32_ld_(&prop->len);

	परत prop;
}

स्थिरांक संरचना fdt_property *fdt_get_property_by_offset(स्थिरांक शून्य *fdt,
						      पूर्णांक offset,
						      पूर्णांक *lenp)
{
	/* Prior to version 16, properties may need realignment
	 * and this API does not work. fdt_getprop_*() will, however. */

	जर (!can_assume(LATEST) && fdt_version(fdt) < 0x10) {
		जर (lenp)
			*lenp = -FDT_ERR_BADVERSION;
		परत रिक्त;
	}

	परत fdt_get_property_by_offset_(fdt, offset, lenp);
}

स्थिर स्थिरांक संरचना fdt_property *fdt_get_property_namelen_(स्थिरांक शून्य *fdt,
						            पूर्णांक offset,
						            स्थिरांक अक्षर *name,
						            पूर्णांक namelen,
							    पूर्णांक *lenp,
							    पूर्णांक *poffset)
{
	साठी (offset = fdt_first_property_offset(fdt, offset);
	     (offset >= 0);
	     (offset = fdt_next_property_offset(fdt, offset))) {
		स्थिरांक संरचना fdt_property *prop;

		prop = fdt_get_property_by_offset_(fdt, offset, lenp);
		जर (!can_assume(LIBFDT_FLAWLESS) && !prop) {
			offset = -FDT_ERR_INTERNAL;
			थांबा;
		}
		जर (fdt_string_eq_(fdt, fdt32_ld_(&prop->nameoff),
				   name, namelen)) {
			जर (poffset)
				*poffset = offset;
			परत prop;
		}
	}

	जर (lenp)
		*lenp = offset;
	परत रिक्त;
}


स्थिरांक संरचना fdt_property *fdt_get_property_namelen(स्थिरांक शून्य *fdt,
						    पूर्णांक offset,
						    स्थिरांक अक्षर *name,
						    पूर्णांक namelen, पूर्णांक *lenp)
{
	/* Prior to version 16, properties may need realignment
	 * and this API does not work. fdt_getprop_*() will, however. */
	जर (!can_assume(LATEST) && fdt_version(fdt) < 0x10) {
		जर (lenp)
			*lenp = -FDT_ERR_BADVERSION;
		परत रिक्त;
	}

	परत fdt_get_property_namelen_(fdt, offset, name, namelen, lenp,
					 रिक्त);
}


स्थिरांक संरचना fdt_property *fdt_get_property(स्थिरांक शून्य *fdt,
					    पूर्णांक nodeoffset,
					    स्थिरांक अक्षर *name, पूर्णांक *lenp)
{
	परत fdt_get_property_namelen(fdt, nodeoffset, name,
					म_माप(name), lenp);
}

स्थिरांक शून्य *fdt_getprop_namelen(स्थिरांक शून्य *fdt, पूर्णांक nodeoffset,
				स्थिरांक अक्षर *name, पूर्णांक namelen, पूर्णांक *lenp)
{
	पूर्णांक poffset;
	स्थिरांक संरचना fdt_property *prop;

	prop = fdt_get_property_namelen_(fdt, nodeoffset, name, namelen, lenp,
					 &poffset);
	जर (!prop)
		परत रिक्त;

	/* Handle realignment */
	जर (!can_assume(LATEST) && fdt_version(fdt) < 0x10 &&
	    (poffset + आकार(*prop)) % 8 && fdt32_ld_(&prop->len) >= 8)
		परत prop->data + 4;
	परत prop->data;
}

स्थिरांक शून्य *fdt_getprop_by_offset(स्थिरांक शून्य *fdt, पूर्णांक offset,
				  स्थिरांक अक्षर **namep, पूर्णांक *lenp)
{
	स्थिरांक संरचना fdt_property *prop;

	prop = fdt_get_property_by_offset_(fdt, offset, lenp);
	जर (!prop)
		परत रिक्त;
	जर (namep) {
		स्थिरांक अक्षर *name;
		पूर्णांक namelen;

		जर (!can_assume(VALID_आदान)) {
			name = fdt_get_string(fdt, fdt32_ld_(&prop->nameoff),
					      &namelen);
			जर (!name) {
				जर (lenp)
					*lenp = namelen;
				परत रिक्त;
			}
			*namep = name;
		} अन्यथा {
			*namep = fdt_string(fdt, fdt32_ld_(&prop->nameoff));
		}
	}

	/* Handle realignment */
	जर (!can_assume(LATEST) && fdt_version(fdt) < 0x10 &&
	    (offset + आकार(*prop)) % 8 && fdt32_ld_(&prop->len) >= 8)
		परत prop->data + 4;
	परत prop->data;
}

स्थिरांक शून्य *fdt_getprop(स्थिरांक शून्य *fdt, पूर्णांक nodeoffset,
			स्थिरांक अक्षर *name, पूर्णांक *lenp)
{
	परत fdt_getprop_namelen(fdt, nodeoffset, name, म_माप(name), lenp);
}

uint32_t fdt_get_phandle(स्थिरांक शून्य *fdt, पूर्णांक nodeoffset)
{
	स्थिरांक fdt32_t *php;
	पूर्णांक len;

	/* FIXME: This is a bit sub-optimal, since we potentially scan
	 * over all the properties twice. */
	php = fdt_getprop(fdt, nodeoffset, "phandle", &len);
	जर (!php || (len != आकार(*php))) {
		php = fdt_getprop(fdt, nodeoffset, "linux,phandle", &len);
		जर (!php || (len != आकार(*php)))
			परत 0;
	}

	परत fdt32_ld_(php);
}

स्थिरांक अक्षर *fdt_get_alias_namelen(स्थिरांक शून्य *fdt,
				  स्थिरांक अक्षर *name, पूर्णांक namelen)
{
	पूर्णांक aliasoffset;

	aliasoffset = fdt_path_offset(fdt, "/aliases");
	जर (aliasoffset < 0)
		परत रिक्त;

	परत fdt_getprop_namelen(fdt, aliasoffset, name, namelen, रिक्त);
}

स्थिरांक अक्षर *fdt_get_alias(स्थिरांक शून्य *fdt, स्थिरांक अक्षर *name)
{
	परत fdt_get_alias_namelen(fdt, name, म_माप(name));
}

पूर्णांक fdt_get_path(स्थिरांक शून्य *fdt, पूर्णांक nodeoffset, अक्षर *buf, पूर्णांक buflen)
{
	पूर्णांक pdepth = 0, p = 0;
	पूर्णांक offset, depth, namelen;
	स्थिरांक अक्षर *name;

	FDT_RO_PROBE(fdt);

	जर (buflen < 2)
		परत -FDT_ERR_NOSPACE;

	साठी (offset = 0, depth = 0;
	     (offset >= 0) && (offset <= nodeoffset);
	     offset = fdt_next_गाठ(fdt, offset, &depth)) {
		पर्यंत (pdepth > depth) {
			करा {
				p--;
			} पर्यंत (buf[p-1] != '/');
			pdepth--;
		}

		जर (pdepth >= depth) {
			name = fdt_get_name(fdt, offset, &namelen);
			जर (!name)
				परत namelen;
			जर ((p + namelen + 1) <= buflen) {
				स_नकल(buf + p, name, namelen);
				p += namelen;
				buf[p++] = '/';
				pdepth++;
			}
		}

		जर (offset == nodeoffset) {
			जर (pdepth < (depth + 1))
				परत -FDT_ERR_NOSPACE;

			जर (p > 1) /* special case so that root path is "/", not "" */
				p--;
			buf[p] = '\0';
			परत 0;
		}
	}

	जर ((offset == -FDT_ERR_NOTFOUND) || (offset >= 0))
		परत -FDT_ERR_BADOFFSET;
	अन्यथा जर (offset == -FDT_ERR_BADOFFSET)
		परत -FDT_ERR_BADSTRUCTURE;

	परत offset; /* error from fdt_next_node() */
}

पूर्णांक fdt_supernode_atdepth_offset(स्थिरांक शून्य *fdt, पूर्णांक nodeoffset,
				 पूर्णांक supernodedepth, पूर्णांक *nodedepth)
{
	पूर्णांक offset, depth;
	पूर्णांक supernodeoffset = -FDT_ERR_INTERNAL;

	FDT_RO_PROBE(fdt);

	जर (supernodedepth < 0)
		परत -FDT_ERR_NOTFOUND;

	साठी (offset = 0, depth = 0;
	     (offset >= 0) && (offset <= nodeoffset);
	     offset = fdt_next_गाठ(fdt, offset, &depth)) {
		जर (depth == supernodedepth)
			supernodeoffset = offset;

		जर (offset == nodeoffset) {
			जर (nodedepth)
				*nodedepth = depth;

			जर (supernodedepth > depth)
				परत -FDT_ERR_NOTFOUND;
			अन्यथा
				परत supernodeoffset;
		}
	}

	जर (!can_assume(VALID_आदान)) {
		जर ((offset == -FDT_ERR_NOTFOUND) || (offset >= 0))
			परत -FDT_ERR_BADOFFSET;
		अन्यथा जर (offset == -FDT_ERR_BADOFFSET)
			परत -FDT_ERR_BADSTRUCTURE;
	}

	परत offset; /* error from fdt_next_node() */
}

पूर्णांक fdt_गाठ_depth(स्थिरांक शून्य *fdt, पूर्णांक nodeoffset)
{
	पूर्णांक nodedepth;
	पूर्णांक err;

	err = fdt_supernode_atdepth_offset(fdt, nodeoffset, 0, &nodedepth);
	जर (err)
		परत (can_assume(LIBFDT_FLAWLESS) || err < 0) ? err :
			-FDT_ERR_INTERNAL;
	परत nodedepth;
}

पूर्णांक fdt_parent_offset(स्थिरांक शून्य *fdt, पूर्णांक nodeoffset)
{
	पूर्णांक nodedepth = fdt_गाठ_depth(fdt, nodeoffset);

	जर (nodedepth < 0)
		परत nodedepth;
	परत fdt_supernode_atdepth_offset(fdt, nodeoffset,
					    nodedepth - 1, रिक्त);
}

पूर्णांक fdt_गाठ_offset_by_prop_मूल्य(स्थिरांक शून्य *fdt, पूर्णांक startoffset,
				  स्थिरांक अक्षर *propname,
				  स्थिरांक शून्य *propval, पूर्णांक proplen)
{
	पूर्णांक offset;
	स्थिरांक शून्य *val;
	पूर्णांक len;

	FDT_RO_PROBE(fdt);

	/* FIXME: The algorithm here is pretty horrible: we scan each
	 * property of a node in fdt_getprop(), then if that didn't
	 * find what we want, we scan over them again making our way
	 * to the next node.  Still it's the easiest to implement
	 * approach; performance can come later. */
	साठी (offset = fdt_next_गाठ(fdt, startoffset, रिक्त);
	     offset >= 0;
	     offset = fdt_next_गाठ(fdt, offset, रिक्त)) {
		val = fdt_getprop(fdt, offset, propname, &len);
		जर (val && (len == proplen)
		    && (स_भेद(val, propval, len) == 0))
			परत offset;
	}

	परत offset; /* error from fdt_next_node() */
}

पूर्णांक fdt_गाठ_offset_by_phandle(स्थिरांक शून्य *fdt, uint32_t phandle)
{
	पूर्णांक offset;

	जर ((phandle == 0) || (phandle == ~0U))
		परत -FDT_ERR_BADPHANDLE;

	FDT_RO_PROBE(fdt);

	/* FIXME: The algorithm here is pretty horrible: we
	 * potentially scan each property of a node in
	 * fdt_get_phandle(), then if that didn't find what
	 * we want, we scan over them again making our way to the next
	 * node.  Still it's the easiest to implement approach;
	 * performance can come later. */
	साठी (offset = fdt_next_गाठ(fdt, -1, रिक्त);
	     offset >= 0;
	     offset = fdt_next_गाठ(fdt, offset, रिक्त)) {
		जर (fdt_get_phandle(fdt, offset) == phandle)
			परत offset;
	}

	परत offset; /* error from fdt_next_node() */
}

पूर्णांक fdt_stringlist_contains(स्थिरांक अक्षर *strlist, पूर्णांक listlen, स्थिरांक अक्षर *str)
{
	पूर्णांक len = म_माप(str);
	स्थिरांक अक्षर *p;

	पर्यंत (listlen >= len) {
		जर (स_भेद(str, strlist, len+1) == 0)
			परत 1;
		p = स_प्रथम(strlist, '\0', listlen);
		जर (!p)
			परत 0; /* malformed strlist.. */
		listlen -= (p-strlist) + 1;
		strlist = p + 1;
	}
	परत 0;
}

पूर्णांक fdt_stringlist_मोजणी(स्थिरांक शून्य *fdt, पूर्णांक nodeoffset, स्थिरांक अक्षर *property)
{
	स्थिरांक अक्षर *list, *end;
	पूर्णांक length, मोजणी = 0;

	list = fdt_getprop(fdt, nodeoffset, property, &length);
	जर (!list)
		परत length;

	end = list + length;

	पर्यंत (list < end) {
		length = strnlen(list, end - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		जर (list + length > end)
			परत -FDT_ERR_BADVALUE;

		list += length;
		मोजणी++;
	}

	परत मोजणी;
}

पूर्णांक fdt_stringlist_शोध(स्थिरांक शून्य *fdt, पूर्णांक nodeoffset, स्थिरांक अक्षर *property,
			  स्थिरांक अक्षर *string)
{
	पूर्णांक length, len, idx = 0;
	स्थिरांक अक्षर *list, *end;

	list = fdt_getprop(fdt, nodeoffset, property, &length);
	जर (!list)
		परत length;

	len = म_माप(string) + 1;
	end = list + length;

	पर्यंत (list < end) {
		length = strnlen(list, end - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		जर (list + length > end)
			परत -FDT_ERR_BADVALUE;

		जर (length == len && स_भेद(list, string, length) == 0)
			परत idx;

		list += length;
		idx++;
	}

	परत -FDT_ERR_NOTFOUND;
}

स्थिरांक अक्षर *fdt_stringlist_get(स्थिरांक शून्य *fdt, पूर्णांक nodeoffset,
			       स्थिरांक अक्षर *property, पूर्णांक idx,
			       पूर्णांक *lenp)
{
	स्थिरांक अक्षर *list, *end;
	पूर्णांक length;

	list = fdt_getprop(fdt, nodeoffset, property, &length);
	जर (!list) {
		जर (lenp)
			*lenp = length;

		परत रिक्त;
	}

	end = list + length;

	पर्यंत (list < end) {
		length = strnlen(list, end - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		जर (list + length > end) {
			जर (lenp)
				*lenp = -FDT_ERR_BADVALUE;

			परत रिक्त;
		}

		जर (idx == 0) {
			जर (lenp)
				*lenp = length - 1;

			परत list;
		}

		list += length;
		idx--;
	}

	जर (lenp)
		*lenp = -FDT_ERR_NOTFOUND;

	परत रिक्त;
}

पूर्णांक fdt_गाठ_check_compatible(स्थिरांक शून्य *fdt, पूर्णांक nodeoffset,
			      स्थिरांक अक्षर *compatible)
{
	स्थिरांक शून्य *prop;
	पूर्णांक len;

	prop = fdt_getprop(fdt, nodeoffset, "compatible", &len);
	जर (!prop)
		परत len;

	परत !fdt_stringlist_contains(prop, len, compatible);
}

पूर्णांक fdt_गाठ_offset_by_compatible(स्थिरांक शून्य *fdt, पूर्णांक startoffset,
				  स्थिरांक अक्षर *compatible)
{
	पूर्णांक offset, err;

	FDT_RO_PROBE(fdt);

	/* FIXME: The algorithm here is pretty horrible: we scan each
	 * property of a node in fdt_node_check_compatible(), then if
	 * that didn't find what we want, we scan over them again
	 * making our way to the next node.  Still it's the easiest to
	 * implement approach; performance can come later. */
	साठी (offset = fdt_next_गाठ(fdt, startoffset, रिक्त);
	     offset >= 0;
	     offset = fdt_next_गाठ(fdt, offset, रिक्त)) {
		err = fdt_गाठ_check_compatible(fdt, offset, compatible);
		जर ((err < 0) && (err != -FDT_ERR_NOTFOUND))
			परत err;
		अन्यथा जर (err == 0)
			परत offset;
	}

	परत offset; /* error from fdt_next_node() */
}
