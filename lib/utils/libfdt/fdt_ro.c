// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#அந்தர்பாவ "libfdt_env.h"

#அந்தர்பாவ <fdt.h>
#அந்தர்பாவ <libfdt.h>

#அந்தர்பாவ "libfdt_internal.h"

நிலையான முழு fdt_nodename_eq_(மாறிலி வெறுமை *fdt, முழு offset,
			    மாறிலி எழுத்து *s, முழு len)
{
	முழு olen;
	மாறிலி எழுத்து *p = fdt_get_name(fdt, offset, &olen);

	எனில் (!p || olen < len)
		/* short match */
		திரும்பு 0;

	எனில் (memcmp(p, s, len) != 0)
		திரும்பு 0;

	எனில் (p[len] == '\0')
		திரும்பு 1;
	இல்லையெனில் எனில் (!memchr(s, '@', len) && (p[len] == '@'))
		திரும்பு 1;
	இல்லையெனில்
		திரும்பு 0;
}

மாறிலி எழுத்து *fdt_get_string(மாறிலி வெறுமை *fdt, முழு stroffset, முழு *lenp)
{
	int32_t totalsize;
	uint32_t absoffset;
	size_t len;
	முழு err;
	மாறிலி எழுத்து *s, *n;

	எனில் (can_assume(VALID_உள்ளீடு)) {
		s = (மாறிலி எழுத்து *)fdt + fdt_off_dt_strings(fdt) + stroffset;

		எனில் (lenp)
			*lenp = strlen(s);
		திரும்பு s;
	}
	totalsize = fdt_ro_probe_(fdt);
	err = totalsize;
	எனில் (totalsize < 0)
		செல் fail;

	err = -FDT_ERR_BADOFFSET;
	absoffset = stroffset + fdt_off_dt_strings(fdt);
	எனில் (absoffset >= (unsigned)totalsize)
		செல் fail;
	len = totalsize - absoffset;

	எனில் (fdt_magic(fdt) == FDT_MAGIC) {
		எனில் (stroffset < 0)
			செல் fail;
		எனில் (can_assume(LATEST) || fdt_version(fdt) >= 17) {
			எனில் ((unsigned)stroffset >= fdt_size_dt_strings(fdt))
				செல் fail;
			எனில் ((fdt_size_dt_strings(fdt) - stroffset) < len)
				len = fdt_size_dt_strings(fdt) - stroffset;
		}
	} இல்லையெனில் எனில் (fdt_magic(fdt) == FDT_SW_MAGIC) {
		unsigned முழு sw_stroffset = -stroffset;

		எனில் ((stroffset >= 0) ||
		    (sw_stroffset > fdt_size_dt_strings(fdt)))
			செல் fail;
		எனில் (sw_stroffset < len)
			len = sw_stroffset;
	} இல்லையெனில் {
		err = -FDT_ERR_INTERNAL;
		செல் fail;
	}

	s = (மாறிலி எழுத்து *)fdt + absoffset;
	n = memchr(s, '\0', len);
	எனில் (!n) {
		/* missing terminating NULL */
		err = -FDT_ERR_TRUNCATED;
		செல் fail;
	}

	எனில் (lenp)
		*lenp = n - s;
	திரும்பு s;

fail:
	எனில் (lenp)
		*lenp = err;
	திரும்பு வெற்று;
}

மாறிலி எழுத்து *fdt_string(மாறிலி வெறுமை *fdt, முழு stroffset)
{
	திரும்பு fdt_get_string(fdt, stroffset, வெற்று);
}

நிலையான முழு fdt_string_eq_(மாறிலி வெறுமை *fdt, முழு stroffset,
			  மாறிலி எழுத்து *s, முழு len)
{
	முழு slen;
	மாறிலி எழுத்து *p = fdt_get_string(fdt, stroffset, &slen);

	திரும்பு p && (slen == len) && (memcmp(p, s, len) == 0);
}

முழு fdt_find_max_phandle(மாறிலி வெறுமை *fdt, uint32_t *phandle)
{
	uint32_t max = 0;
	முழு offset = -1;

	போது (true) {
		uint32_t மதிப்பு;

		offset = fdt_next_முனை(fdt, offset, வெற்று);
		எனில் (offset < 0) {
			எனில் (offset == -FDT_ERR_NOTFOUND)
				நிறுத்து;

			திரும்பு offset;
		}

		மதிப்பு = fdt_get_phandle(fdt, offset);

		எனில் (மதிப்பு > max)
			max = மதிப்பு;
	}

	எனில் (phandle)
		*phandle = max;

	திரும்பு 0;
}

முழு fdt_generate_phandle(மாறிலி வெறுமை *fdt, uint32_t *phandle)
{
	uint32_t max;
	முழு err;

	err = fdt_find_max_phandle(fdt, &max);
	எனில் (err < 0)
		திரும்பு err;

	எனில் (max == FDT_MAX_PHANDLE)
		திரும்பு -FDT_ERR_NOPHANDLES;

	எனில் (phandle)
		*phandle = max + 1;

	திரும்பு 0;
}

நிலையான மாறிலி கட்டமைப்பு fdt_reserve_entry *fdt_mem_rsv(மாறிலி வெறுமை *fdt, முழு n)
{
	unsigned முழு offset = n * அளவு(கட்டமைப்பு fdt_reserve_entry);
	unsigned முழு absoffset = fdt_off_mem_rsvmap(fdt) + offset;

	எனில் (!can_assume(VALID_உள்ளீடு)) {
		எனில் (absoffset < fdt_off_mem_rsvmap(fdt))
			திரும்பு வெற்று;
		எனில் (absoffset > fdt_totalsize(fdt) -
		    அளவு(கட்டமைப்பு fdt_reserve_entry))
			திரும்பு வெற்று;
	}
	திரும்பு fdt_mem_rsv_(fdt, n);
}

முழு fdt_get_mem_rsv(மாறிலி வெறுமை *fdt, முழு n, uint64_t *address, uint64_t *size)
{
	மாறிலி கட்டமைப்பு fdt_reserve_entry *re;

	FDT_RO_PROBE(fdt);
	re = fdt_mem_rsv(fdt, n);
	எனில் (!can_assume(VALID_உள்ளீடு) && !re)
		திரும்பு -FDT_ERR_BADOFFSET;

	*address = fdt64_ld_(&re->address);
	*size = fdt64_ld_(&re->size);
	திரும்பு 0;
}

முழு fdt_இலக்கம்_mem_rsv(மாறிலி வெறுமை *fdt)
{
	முழு i;
	மாறிலி கட்டமைப்பு fdt_reserve_entry *re;

	க்கு (i = 0; (re = fdt_mem_rsv(fdt, i)) != வெற்று; i++) {
		எனில் (fdt64_ld_(&re->size) == 0)
			திரும்பு i;
	}
	திரும்பு -FDT_ERR_TRUNCATED;
}

நிலையான முழு nextprop_(மாறிலி வெறுமை *fdt, முழு offset)
{
	uint32_t tag;
	முழு nextoffset;

	செய் {
		tag = fdt_next_tag(fdt, offset, &nextoffset);

		தேர்வு (tag) {
		நிலை FDT_END:
			எனில் (nextoffset >= 0)
				திரும்பு -FDT_ERR_BADSTRUCTURE;
			இல்லையெனில்
				திரும்பு nextoffset;

		நிலை FDT_PROP:
			திரும்பு offset;
		}
		offset = nextoffset;
	} போது (tag == FDT_NOP);

	திரும்பு -FDT_ERR_NOTFOUND;
}

முழு fdt_subnode_offset_namelen(மாறிலி வெறுமை *fdt, முழு offset,
			       மாறிலி எழுத்து *name, முழு namelen)
{
	முழு depth;

	FDT_RO_PROBE(fdt);

	க்கு (depth = 0;
	     (offset >= 0) && (depth >= 0);
	     offset = fdt_next_முனை(fdt, offset, &depth))
		எனில் ((depth == 1)
		    && fdt_nodename_eq_(fdt, offset, name, namelen))
			திரும்பு offset;

	எனில் (depth < 0)
		திரும்பு -FDT_ERR_NOTFOUND;
	திரும்பு offset; /* error */
}

முழு fdt_subnode_offset(மாறிலி வெறுமை *fdt, முழு parentoffset,
		       மாறிலி எழுத்து *name)
{
	திரும்பு fdt_subnode_offset_namelen(fdt, parentoffset, name, strlen(name));
}

முழு fdt_path_offset_namelen(மாறிலி வெறுமை *fdt, மாறிலி எழுத்து *path, முழு namelen)
{
	மாறிலி எழுத்து *end = path + namelen;
	மாறிலி எழுத்து *p = path;
	முழு offset = 0;

	FDT_RO_PROBE(fdt);

	/* see if we have an alias */
	எனில் (*path != '/') {
		மாறிலி எழுத்து *q = memchr(path, '/', end - p);

		எனில் (!q)
			q = end;

		p = fdt_get_alias_namelen(fdt, p, q - p);
		எனில் (!p)
			திரும்பு -FDT_ERR_BADPATH;
		offset = fdt_path_offset(fdt, p);

		p = q;
	}

	போது (p < end) {
		மாறிலி எழுத்து *q;

		போது (*p == '/') {
			p++;
			எனில் (p == end)
				திரும்பு offset;
		}
		q = memchr(p, '/', end - p);
		எனில் (! q)
			q = end;

		offset = fdt_subnode_offset_namelen(fdt, offset, p, q-p);
		எனில் (offset < 0)
			திரும்பு offset;

		p = q;
	}

	திரும்பு offset;
}

முழு fdt_path_offset(மாறிலி வெறுமை *fdt, மாறிலி எழுத்து *path)
{
	திரும்பு fdt_path_offset_namelen(fdt, path, strlen(path));
}

மாறிலி எழுத்து *fdt_get_name(மாறிலி வெறுமை *fdt, முழு nodeoffset, முழு *len)
{
	மாறிலி கட்டமைப்பு fdt_முனை_header *nh = fdt_offset_ptr_(fdt, nodeoffset);
	மாறிலி எழுத்து *nameptr;
	முழு err;

	எனில் (((err = fdt_ro_probe_(fdt)) < 0)
	    || ((err = fdt_check_முனை_offset(fdt, nodeoffset)) < 0))
			செல் fail;

	nameptr = nh->name;

	எனில் (!can_assume(LATEST) && fdt_version(fdt) < 0x10) {
		/*
		 * For old FDT versions, match the naming conventions of V16:
		 * give only the leaf name (after all /). The actual tree
		 * contents are loosely checked.
		 */
		மாறிலி எழுத்து *leaf;
		leaf = strrchr(nameptr, '/');
		எனில் (leaf == வெற்று) {
			err = -FDT_ERR_BADSTRUCTURE;
			செல் fail;
		}
		nameptr = leaf+1;
	}

	எனில் (len)
		*len = strlen(nameptr);

	திரும்பு nameptr;

 fail:
	எனில் (len)
		*len = err;
	திரும்பு வெற்று;
}

முழு fdt_first_property_offset(மாறிலி வெறுமை *fdt, முழு nodeoffset)
{
	முழு offset;

	எனில் ((offset = fdt_check_முனை_offset(fdt, nodeoffset)) < 0)
		திரும்பு offset;

	திரும்பு nextprop_(fdt, offset);
}

முழு fdt_next_property_offset(மாறிலி வெறுமை *fdt, முழு offset)
{
	எனில் ((offset = fdt_check_prop_offset_(fdt, offset)) < 0)
		திரும்பு offset;

	திரும்பு nextprop_(fdt, offset);
}

நிலையான மாறிலி கட்டமைப்பு fdt_property *fdt_get_property_by_offset_(மாறிலி வெறுமை *fdt,
						              முழு offset,
						              முழு *lenp)
{
	முழு err;
	மாறிலி கட்டமைப்பு fdt_property *prop;

	எனில் (!can_assume(VALID_உள்ளீடு) &&
	    (err = fdt_check_prop_offset_(fdt, offset)) < 0) {
		எனில் (lenp)
			*lenp = err;
		திரும்பு வெற்று;
	}

	prop = fdt_offset_ptr_(fdt, offset);

	எனில் (lenp)
		*lenp = fdt32_ld_(&prop->len);

	திரும்பு prop;
}

மாறிலி கட்டமைப்பு fdt_property *fdt_get_property_by_offset(மாறிலி வெறுமை *fdt,
						      முழு offset,
						      முழு *lenp)
{
	/* Prior to version 16, properties may need realignment
	 * and this API does not work. fdt_getprop_*() will, however. */

	எனில் (!can_assume(LATEST) && fdt_version(fdt) < 0x10) {
		எனில் (lenp)
			*lenp = -FDT_ERR_BADVERSION;
		திரும்பு வெற்று;
	}

	திரும்பு fdt_get_property_by_offset_(fdt, offset, lenp);
}

நிலையான மாறிலி கட்டமைப்பு fdt_property *fdt_get_property_namelen_(மாறிலி வெறுமை *fdt,
						            முழு offset,
						            மாறிலி எழுத்து *name,
						            முழு namelen,
							    முழு *lenp,
							    முழு *poffset)
{
	க்கு (offset = fdt_first_property_offset(fdt, offset);
	     (offset >= 0);
	     (offset = fdt_next_property_offset(fdt, offset))) {
		மாறிலி கட்டமைப்பு fdt_property *prop;

		prop = fdt_get_property_by_offset_(fdt, offset, lenp);
		எனில் (!can_assume(LIBFDT_FLAWLESS) && !prop) {
			offset = -FDT_ERR_INTERNAL;
			நிறுத்து;
		}
		எனில் (fdt_string_eq_(fdt, fdt32_ld_(&prop->nameoff),
				   name, namelen)) {
			எனில் (poffset)
				*poffset = offset;
			திரும்பு prop;
		}
	}

	எனில் (lenp)
		*lenp = offset;
	திரும்பு வெற்று;
}


மாறிலி கட்டமைப்பு fdt_property *fdt_get_property_namelen(மாறிலி வெறுமை *fdt,
						    முழு offset,
						    மாறிலி எழுத்து *name,
						    முழு namelen, முழு *lenp)
{
	/* Prior to version 16, properties may need realignment
	 * and this API does not work. fdt_getprop_*() will, however. */
	எனில் (!can_assume(LATEST) && fdt_version(fdt) < 0x10) {
		எனில் (lenp)
			*lenp = -FDT_ERR_BADVERSION;
		திரும்பு வெற்று;
	}

	திரும்பு fdt_get_property_namelen_(fdt, offset, name, namelen, lenp,
					 வெற்று);
}


மாறிலி கட்டமைப்பு fdt_property *fdt_get_property(மாறிலி வெறுமை *fdt,
					    முழு nodeoffset,
					    மாறிலி எழுத்து *name, முழு *lenp)
{
	திரும்பு fdt_get_property_namelen(fdt, nodeoffset, name,
					strlen(name), lenp);
}

மாறிலி வெறுமை *fdt_getprop_namelen(மாறிலி வெறுமை *fdt, முழு nodeoffset,
				மாறிலி எழுத்து *name, முழு namelen, முழு *lenp)
{
	முழு poffset;
	மாறிலி கட்டமைப்பு fdt_property *prop;

	prop = fdt_get_property_namelen_(fdt, nodeoffset, name, namelen, lenp,
					 &poffset);
	எனில் (!prop)
		திரும்பு வெற்று;

	/* Handle realignment */
	எனில் (!can_assume(LATEST) && fdt_version(fdt) < 0x10 &&
	    (poffset + அளவு(*prop)) % 8 && fdt32_ld_(&prop->len) >= 8)
		திரும்பு prop->data + 4;
	திரும்பு prop->data;
}

மாறிலி வெறுமை *fdt_getprop_by_offset(மாறிலி வெறுமை *fdt, முழு offset,
				  மாறிலி எழுத்து **namep, முழு *lenp)
{
	மாறிலி கட்டமைப்பு fdt_property *prop;

	prop = fdt_get_property_by_offset_(fdt, offset, lenp);
	எனில் (!prop)
		திரும்பு வெற்று;
	எனில் (namep) {
		மாறிலி எழுத்து *name;
		முழு namelen;

		எனில் (!can_assume(VALID_உள்ளீடு)) {
			name = fdt_get_string(fdt, fdt32_ld_(&prop->nameoff),
					      &namelen);
			எனில் (!name) {
				எனில் (lenp)
					*lenp = namelen;
				திரும்பு வெற்று;
			}
			*namep = name;
		} இல்லையெனில் {
			*namep = fdt_string(fdt, fdt32_ld_(&prop->nameoff));
		}
	}

	/* Handle realignment */
	எனில் (!can_assume(LATEST) && fdt_version(fdt) < 0x10 &&
	    (offset + அளவு(*prop)) % 8 && fdt32_ld_(&prop->len) >= 8)
		திரும்பு prop->data + 4;
	திரும்பு prop->data;
}

மாறிலி வெறுமை *fdt_getprop(மாறிலி வெறுமை *fdt, முழு nodeoffset,
			மாறிலி எழுத்து *name, முழு *lenp)
{
	திரும்பு fdt_getprop_namelen(fdt, nodeoffset, name, strlen(name), lenp);
}

uint32_t fdt_get_phandle(மாறிலி வெறுமை *fdt, முழு nodeoffset)
{
	மாறிலி fdt32_t *php;
	முழு len;

	/* FIXME: This is a bit sub-optimal, since we potentially scan
	 * over all the properties twice. */
	php = fdt_getprop(fdt, nodeoffset, "phandle", &len);
	எனில் (!php || (len != அளவு(*php))) {
		php = fdt_getprop(fdt, nodeoffset, "linux,phandle", &len);
		எனில் (!php || (len != அளவு(*php)))
			திரும்பு 0;
	}

	திரும்பு fdt32_ld_(php);
}

மாறிலி எழுத்து *fdt_get_alias_namelen(மாறிலி வெறுமை *fdt,
				  மாறிலி எழுத்து *name, முழு namelen)
{
	முழு aliasoffset;

	aliasoffset = fdt_path_offset(fdt, "/aliases");
	எனில் (aliasoffset < 0)
		திரும்பு வெற்று;

	திரும்பு fdt_getprop_namelen(fdt, aliasoffset, name, namelen, வெற்று);
}

மாறிலி எழுத்து *fdt_get_alias(மாறிலி வெறுமை *fdt, மாறிலி எழுத்து *name)
{
	திரும்பு fdt_get_alias_namelen(fdt, name, strlen(name));
}

முழு fdt_get_path(மாறிலி வெறுமை *fdt, முழு nodeoffset, எழுத்து *buf, முழு buflen)
{
	முழு pdepth = 0, p = 0;
	முழு offset, depth, namelen;
	மாறிலி எழுத்து *name;

	FDT_RO_PROBE(fdt);

	எனில் (buflen < 2)
		திரும்பு -FDT_ERR_NOSPACE;

	க்கு (offset = 0, depth = 0;
	     (offset >= 0) && (offset <= nodeoffset);
	     offset = fdt_next_முனை(fdt, offset, &depth)) {
		போது (pdepth > depth) {
			செய் {
				p--;
			} போது (buf[p-1] != '/');
			pdepth--;
		}

		எனில் (pdepth >= depth) {
			name = fdt_get_name(fdt, offset, &namelen);
			எனில் (!name)
				திரும்பு namelen;
			எனில் ((p + namelen + 1) <= buflen) {
				memcpy(buf + p, name, namelen);
				p += namelen;
				buf[p++] = '/';
				pdepth++;
			}
		}

		எனில் (offset == nodeoffset) {
			எனில் (pdepth < (depth + 1))
				திரும்பு -FDT_ERR_NOSPACE;

			எனில் (p > 1) /* special case so that root path is "/", not "" */
				p--;
			buf[p] = '\0';
			திரும்பு 0;
		}
	}

	எனில் ((offset == -FDT_ERR_NOTFOUND) || (offset >= 0))
		திரும்பு -FDT_ERR_BADOFFSET;
	இல்லையெனில் எனில் (offset == -FDT_ERR_BADOFFSET)
		திரும்பு -FDT_ERR_BADSTRUCTURE;

	திரும்பு offset; /* error from fdt_next_node() */
}

முழு fdt_supernode_atdepth_offset(மாறிலி வெறுமை *fdt, முழு nodeoffset,
				 முழு supernodedepth, முழு *nodedepth)
{
	முழு offset, depth;
	முழு supernodeoffset = -FDT_ERR_INTERNAL;

	FDT_RO_PROBE(fdt);

	எனில் (supernodedepth < 0)
		திரும்பு -FDT_ERR_NOTFOUND;

	க்கு (offset = 0, depth = 0;
	     (offset >= 0) && (offset <= nodeoffset);
	     offset = fdt_next_முனை(fdt, offset, &depth)) {
		எனில் (depth == supernodedepth)
			supernodeoffset = offset;

		எனில் (offset == nodeoffset) {
			எனில் (nodedepth)
				*nodedepth = depth;

			எனில் (supernodedepth > depth)
				திரும்பு -FDT_ERR_NOTFOUND;
			இல்லையெனில்
				திரும்பு supernodeoffset;
		}
	}

	எனில் (!can_assume(VALID_உள்ளீடு)) {
		எனில் ((offset == -FDT_ERR_NOTFOUND) || (offset >= 0))
			திரும்பு -FDT_ERR_BADOFFSET;
		இல்லையெனில் எனில் (offset == -FDT_ERR_BADOFFSET)
			திரும்பு -FDT_ERR_BADSTRUCTURE;
	}

	திரும்பு offset; /* error from fdt_next_node() */
}

முழு fdt_முனை_depth(மாறிலி வெறுமை *fdt, முழு nodeoffset)
{
	முழு nodedepth;
	முழு err;

	err = fdt_supernode_atdepth_offset(fdt, nodeoffset, 0, &nodedepth);
	எனில் (err)
		திரும்பு (can_assume(LIBFDT_FLAWLESS) || err < 0) ? err :
			-FDT_ERR_INTERNAL;
	திரும்பு nodedepth;
}

முழு fdt_parent_offset(மாறிலி வெறுமை *fdt, முழு nodeoffset)
{
	முழு nodedepth = fdt_முனை_depth(fdt, nodeoffset);

	எனில் (nodedepth < 0)
		திரும்பு nodedepth;
	திரும்பு fdt_supernode_atdepth_offset(fdt, nodeoffset,
					    nodedepth - 1, வெற்று);
}

முழு fdt_முனை_offset_by_prop_மதிப்பு(மாறிலி வெறுமை *fdt, முழு startoffset,
				  மாறிலி எழுத்து *propname,
				  மாறிலி வெறுமை *propval, முழு proplen)
{
	முழு offset;
	மாறிலி வெறுமை *val;
	முழு len;

	FDT_RO_PROBE(fdt);

	/* FIXME: The algorithm here is pretty horrible: we scan each
	 * property of a node in fdt_getprop(), then if that didn't
	 * find what we want, we scan over them again making our way
	 * to the next node.  Still it's the easiest to implement
	 * approach; performance can come later. */
	க்கு (offset = fdt_next_முனை(fdt, startoffset, வெற்று);
	     offset >= 0;
	     offset = fdt_next_முனை(fdt, offset, வெற்று)) {
		val = fdt_getprop(fdt, offset, propname, &len);
		எனில் (val && (len == proplen)
		    && (memcmp(val, propval, len) == 0))
			திரும்பு offset;
	}

	திரும்பு offset; /* error from fdt_next_node() */
}

முழு fdt_முனை_offset_by_phandle(மாறிலி வெறுமை *fdt, uint32_t phandle)
{
	முழு offset;

	எனில் ((phandle == 0) || (phandle == ~0U))
		திரும்பு -FDT_ERR_BADPHANDLE;

	FDT_RO_PROBE(fdt);

	/* FIXME: The algorithm here is pretty horrible: we
	 * potentially scan each property of a node in
	 * fdt_get_phandle(), then if that didn't find what
	 * we want, we scan over them again making our way to the next
	 * node.  Still it's the easiest to implement approach;
	 * performance can come later. */
	க்கு (offset = fdt_next_முனை(fdt, -1, வெற்று);
	     offset >= 0;
	     offset = fdt_next_முனை(fdt, offset, வெற்று)) {
		எனில் (fdt_get_phandle(fdt, offset) == phandle)
			திரும்பு offset;
	}

	திரும்பு offset; /* error from fdt_next_node() */
}

முழு fdt_stringlist_contains(மாறிலி எழுத்து *strlist, முழு listlen, மாறிலி எழுத்து *str)
{
	முழு len = strlen(str);
	மாறிலி எழுத்து *p;

	போது (listlen >= len) {
		எனில் (memcmp(str, strlist, len+1) == 0)
			திரும்பு 1;
		p = memchr(strlist, '\0', listlen);
		எனில் (!p)
			திரும்பு 0; /* malformed strlist.. */
		listlen -= (p-strlist) + 1;
		strlist = p + 1;
	}
	திரும்பு 0;
}

முழு fdt_stringlist_மொத்தம்(மாறிலி வெறுமை *fdt, முழு nodeoffset, மாறிலி எழுத்து *property)
{
	மாறிலி எழுத்து *list, *end;
	முழு length, மொத்தம் = 0;

	list = fdt_getprop(fdt, nodeoffset, property, &length);
	எனில் (!list)
		திரும்பு length;

	end = list + length;

	போது (list < end) {
		length = strnlen(list, end - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		எனில் (list + length > end)
			திரும்பு -FDT_ERR_BADVALUE;

		list += length;
		மொத்தம்++;
	}

	திரும்பு மொத்தம்;
}

முழு fdt_stringlist_தேடல்(மாறிலி வெறுமை *fdt, முழு nodeoffset, மாறிலி எழுத்து *property,
			  மாறிலி எழுத்து *string)
{
	முழு length, len, idx = 0;
	மாறிலி எழுத்து *list, *end;

	list = fdt_getprop(fdt, nodeoffset, property, &length);
	எனில் (!list)
		திரும்பு length;

	len = strlen(string) + 1;
	end = list + length;

	போது (list < end) {
		length = strnlen(list, end - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		எனில் (list + length > end)
			திரும்பு -FDT_ERR_BADVALUE;

		எனில் (length == len && memcmp(list, string, length) == 0)
			திரும்பு idx;

		list += length;
		idx++;
	}

	திரும்பு -FDT_ERR_NOTFOUND;
}

மாறிலி எழுத்து *fdt_stringlist_get(மாறிலி வெறுமை *fdt, முழு nodeoffset,
			       மாறிலி எழுத்து *property, முழு idx,
			       முழு *lenp)
{
	மாறிலி எழுத்து *list, *end;
	முழு length;

	list = fdt_getprop(fdt, nodeoffset, property, &length);
	எனில் (!list) {
		எனில் (lenp)
			*lenp = length;

		திரும்பு வெற்று;
	}

	end = list + length;

	போது (list < end) {
		length = strnlen(list, end - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		எனில் (list + length > end) {
			எனில் (lenp)
				*lenp = -FDT_ERR_BADVALUE;

			திரும்பு வெற்று;
		}

		எனில் (idx == 0) {
			எனில் (lenp)
				*lenp = length - 1;

			திரும்பு list;
		}

		list += length;
		idx--;
	}

	எனில் (lenp)
		*lenp = -FDT_ERR_NOTFOUND;

	திரும்பு வெற்று;
}

முழு fdt_முனை_check_compatible(மாறிலி வெறுமை *fdt, முழு nodeoffset,
			      மாறிலி எழுத்து *compatible)
{
	மாறிலி வெறுமை *prop;
	முழு len;

	prop = fdt_getprop(fdt, nodeoffset, "compatible", &len);
	எனில் (!prop)
		திரும்பு len;

	திரும்பு !fdt_stringlist_contains(prop, len, compatible);
}

முழு fdt_முனை_offset_by_compatible(மாறிலி வெறுமை *fdt, முழு startoffset,
				  மாறிலி எழுத்து *compatible)
{
	முழு offset, err;

	FDT_RO_PROBE(fdt);

	/* FIXME: The algorithm here is pretty horrible: we scan each
	 * property of a node in fdt_node_check_compatible(), then if
	 * that didn't find what we want, we scan over them again
	 * making our way to the next node.  Still it's the easiest to
	 * implement approach; performance can come later. */
	க்கு (offset = fdt_next_முனை(fdt, startoffset, வெற்று);
	     offset >= 0;
	     offset = fdt_next_முனை(fdt, offset, வெற்று)) {
		err = fdt_முனை_check_compatible(fdt, offset, compatible);
		எனில் ((err < 0) && (err != -FDT_ERR_NOTFOUND))
			திரும்பு err;
		இல்லையெனில் எனில் (err == 0)
			திரும்பு offset;
	}

	திரும்பு offset; /* error from fdt_next_node() */
}
